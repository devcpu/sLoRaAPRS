#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifndef ARDUINO
#include <tgmath.h>
#include <iostream>
#endif

#include <main.h>
#include <Wire.h>
#include <APRSWebServer.h>
#include <APRSWiFi.h>
#include <APRS_MSG.h>
#include <Esp.h>
#include <LoRaAPRSConfig.h>
#include <LoRaHandler.h>
#include <OneButtonHandler.h>
#include <SPI.h>
#include <TrackerDisplay.h>
#include <BMEHandler.h>
#include <fap.h>

//#include <iGate.h>

extern Registry reg;  // config & system status

// @TODO APRS_MSG deprecated?
APRS_MSG tx_msg;  // converts all data to APRS messages
extern TinyGPSPlus gps;  // driver fot GPS

uint32_t waitTxTr = 0L, nextTxTr = 10000L, nextTxTrRand = 10;  // ticker for APRS messages
uint32_t waitTxDg = 0L, nextTxDg = 50000L, nextTxDgRand = 10;
uint32_t wait_wx_update = 0, next_wx_update = 10000;
uint64_t nextRX = 0;
uint8_t gps_error = 0;
char txmsg[254];

uint8_t odd = 0;
char satbuf[24] = "";

OneButton button(BUTTON, true);


void setup() {
  randomSeed(ESP.getCycleCount());
  // ESP.deepSleep(1, WAKE_RF_DISABLED);
  Serial.begin(115200);
  delay(1000);

  if (Wire.begin(SDA, SCL)) {
    write3toSerial("Init I2C", "  System", "   +OK", DISPLA_DELAY_SHORT);  
  } else {
    write3toSerial("Init I2C", "   -ERR", "check wire", DISPLA_DELAY_LONG);
  }

  if (initDisplay()) {
    write3Line("Init Disp.", "Display", "   +OK", true, DISPLA_DELAY_SHORT);  
  } else {
    write3Line("Init Disp.", "   -ERR", "check wire", true, DISPLA_DELAY_LONG);  
  }

  //write3Line({{""}, {""}, {""}}, true, 1);
#ifdef T_BEAM_V1_0
    if (initAXP()) {
      write3Line("Init AXP", " AXP 192", "   +OK", true, DISPLA_DELAY_SHORT);
    } else {
      write3Line("Init AXP", "   -ERR", "check wire", true, DISPLA_DELAY_LONG);
    }
#endif


  RegistryInit();
  write3Line("Init Pref.", "Prefernces", "   +OK", true, DISPLA_DELAY_SHORT);

  if (BMEHandlerInit()) {
    write3Line("Init BME", "  BME280", "   +OK", true, DISPLA_DELAY_SHORT);
  } else {
    write3Line("Init BME", "   -ERR", "check wire", true, DISPLA_DELAY_LONG);
  }

  if (ESPFSInit()) {
    write3Line("Init SPIFS", "  SPIFFS", "   +OK", true, DISPLA_DELAY_SHORT);
  } else {
    write3Line("Init SPIFS", "   -ERR", "check chip", true, DISPLA_DELAY_SHORT);
  }


  ss.begin(GPS_BAUD, SERIAL_8N1, TXPin, RXPin);
  write3Line("Init UART", " GPS UART", "   +OK", true, DISPLA_DELAY_SHORT);


  if (LoRa_init()) {
    write3Line("Init LoRa", "LoDaDevive", "   +OK", true, DISPLA_DELAY_SHORT);
  } else {
    write3Line("Init LoRa", "   -ERR", "check wire", true, DISPLA_DELAY_SHORT);
  }
  // lora.onReceive(lora.processMessage);
  // lora.receive();

  

  pinMode(TXLED, OUTPUT);

  initOneButton();
  write3Line("Init 1BUT", "OneButton", "   +OK", true, DISPLA_DELAY_SHORT);

  reg.current_wifi_mode = wifi_client;
  //reg.current_wifi_mode = wifi_ap;
  write3Line(" RUN MODE", getRunMode().c_str(), "", true, DISPLA_DELAY_MEDIUM);
  write3Line("WiFi MODE", getWifiMode().c_str(), "", true, DISPLA_DELAY_MEDIUM);

  if (reg.current_wifi_mode == wifi_ap) {
    Serial.println("start wifi_ap");
    WifiAPInit();
  }
  if (reg.current_wifi_mode == wifi_client) {
    Serial.println(" start wifi_client");
    WifiConnect();
  }
  if (reg.current_wifi_mode != wifi_off) {
    Serial.println(" start WebServer");
    WebserverStart();
  }

  // SPIFFS.esp_partition_find();
  // SPIFFSConfig
  // ESP8266
  // dumpEEPROM();

  //Scanner();

  write3Line("sLoRaAPRS", "  up &", " running", true, 2000);
  write3Line("  Enjoy", "   the", "   day", true, 2000);
  setWXData();
}

void loop() {
  if (wait_wx_update < millis()) {
    setWXData();
    wait_wx_update = next_wx_update + millis();
  }
  // watchdog();
  APRSWebServerTick();
  //iGate_process_udp();
  // if (reg.TxMsg.newmessage) {
  //   Serial.println("########################");
  //   Serial.println("to: " + reg.TxMsg.to);
  //   Serial.println("via: " + reg.TxMsg.gateways);
  //   Serial.println("msg: " + reg.TxMsg.msg);
  //   reg.TxMsg.processed = true;
  //   reg.TxMsg.newmessage = false;
  //   Serial.println("########################");
  // }


  button.tick();
  LoRa_tick();
  //tracker_display_tick();

  
  if (waitTxTr < millis() && !lora_control.isSend) {  // start Tx
     char msg_buf[256] = {0};
     char aprs_buf[32] = {0};
     char wx_buf[128] = {0};
     char track_buf[128] = {0};
    snprintf(msg_buf, 256, "%s>APRS:!%s%s %s uptime [%u] send to tracker", 
      reg_aprsCall().c_str(), 
      APRS_MSG::computeAPRSPos(aprs_buf), 
      APRS_MSG::computeTrackInfo(track_buf),
      APRS_MSG::computeWXField(wx_buf),
      millis()/1000);
      
    sendMessage(msg_buf, false);
    //   Serial.printf("sending +OK\n");
    // } else {
    //   Serial.printf("sending -NOK\n");
    // }
    waitTxTr = millis() + nextTxTr + random(nextTxTrRand) * 1000;
  }

  // if (waitTxDg  < millis()&& !lora_control.isSend) {  // start Tx
  //   uint64_t t0 = millis();
  //   char msg_buf[256] = {0};
  //   snprintf(msg_buf, 256, "DL7UXA-7>APRS:!5229.16N/01334.52E_359/031/A=000127 uptime [%ul] send to digi", millis()/1000);
  //   sendMessage(msg_buf, true);
  //   uint64_t t1 = millis();
  //   Serial.printf("sendtime: %d\n", t1 - t0);
  //   waitTxDg = millis() + nextTxDg + random(nextTxDgRand) * 1000;
  // }

  smartDelay(100);
}


bool initAXP() {
  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
    Serial.println("AXP192 Begin PASS");
  } else {
    Serial.println("AXP192 Begin FAIL");
    return false;
  }
  axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
  axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
  axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
  axp.setDCDC1Voltage(3300);
  return true;
}

static void smartDelay(uint32_t ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) {
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

/**
 * @brief restarts system if millis() is overruned.
 * This function must called in every loop.:q
 *
 *
 * after ~50 days, millis() will start by 0, because
 * it is an uint32_t. After that all systems with tick, will not work any more.
 * So we will restart the system.
 *
 * There are some other paranoid cases here.
 *
 */
static void watchdog(void) {
  uint32_t lastseen = 0;

  if (lastseen > millis()) {
    ESP.restart();
  }

  if (lastseen + 10000L < millis()) {
    ESP.restart();
  }
}

// @TODO one version only @see APRSWebServer.cpp
void restart(void) {
  delay(300);
  WifiDisconnect();
  delay(300);
  ESP.restart();
}


void Scanner() {
  Serial.println();
  Serial.println("I2C scanner. Scanning ...");
  byte count = 0;

  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);        // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0)  // Receive 0 = success (ACK response)
    {
      Serial.print("Found address: ");
      Serial.print(i, DEC);
      Serial.print(" (0x");
      Serial.print(i, HEX);  // PCF8574 7 bit address
      Serial.println(")");
      count++;
    }
  }
  Serial.print("Found ");
  Serial.print(count, DEC);  // numbers of devices
  Serial.println(" device(s).");
}


void setGPSInfo(void) {
  if (gps.location.isValid()) {
    reg.gps_location.latitude = gps.location.lat();
    reg.gps_location.longitude = gps.location.lng();
  }
  if (gps.altitude.isValid()) {
    reg.gps_location.altitude = gps.altitude.meters();
  }
  if (gps.speed.isValid()) {
    reg.gps_move.speed = gps.speed.kmph();
  }
  if (gps.course.isValid()) {
    reg.gps_move.course = gps.course.deg();
  }
}