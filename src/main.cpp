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
#include <EEPROM.h>
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
TinyGPSPlus gps;  // driver fot GPS

uint32_t lastTx = 0L, nextTx = 20000L;  // ticker for APRS messages
uint8_t gps_error = 0;
char txmsg[254];

uint8_t odd = 0;
char satbuf[24] = "";

OneButton button(BUTTON, true);


void setup() {
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

  tx_msg.reset();

if (initLoRa()) {
    write3Line("Init LoRa", "LoDaDevive", "   +OK", true, DISPLA_DELAY_SHORT);
  } else {
    write3Line("Init LoRa", "   -ERR", "check wire", true, DISPLA_DELAY_SHORT);
  }

  

  pinMode(TXLED, OUTPUT);

  initOneButton();
  write3Line("Init 1BUT", "OneButton", "   +OK", true, DISPLA_DELAY_SHORT);

  reg.current_wifi_mode = wifi_client;

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

}

void loop() {
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

  //button.tick();
  //tracker_display_tick();
  // odd++;
  // // Serial.printf("++++++++++   Starte neue Loop-Runde  Zeit: %s-%s-%d
  // // ++++++++++\n", tx_msg._gps_hour, tx_msg._gps_minute, gps.time.second());
  if ((lastTx + nextTx) < millis()) {  // start Tx
    // Serial.printf("%s-%s-%s age: %d \n", tx_msg._gps_year,
    // tx_msg._gps_month,
    // tx_msg._gps_day, gps.date.age());
    uint64_t t0 = millis();
    char msg_buf[256] = {0};
    snprintf(msg_buf, 256, "DL7UXA-1>APRS:!5229.16N/01334.52E_359/031/A=000127 uptime [%d]", millis()/1000);
    sendMessage(msg_buf);
    uint64_t t1 = millis();
    Serial.printf("Zeit für Zeichen %d\n", t1 - t0);
    Serial.printf("\nTime: %d ", millis()/1000UL);
    Serial.printf("\nsatellites: %d ", gps.satellites.value());
    Serial.printf("hdop: %03.1f\n", gps.hdop.hdop());

    gps_error = setAllGPSData();
    // if (!gps_error) {
      Serial.printf("%s-%s-%s age: %d \n", tx_msg._gps_year, tx_msg._gps_month, tx_msg._gps_day, gps.date.age());
      Serial.printf("%s:%s:%s \n", tx_msg._gps_hour, tx_msg._gps_minute, tx_msg._gps_second);
  //     // sendMessage(tx_msg.getAPRSTxPos(txmsg));
  //     sendMessage(tx_msg.getAPRSTxTrackInfo(txmsg));
    // } else {
    //   Serial.printf("GPS not ready %d\n", gps_error);
    // }
    lastTx = millis();  // gps not ok we wait also
    smartDelay(50);
  }

}

uint8_t setAllGPSData(void) {
  uint8_t gps_error = 0;
  if (gps.time.isValid()) {
    tx_msg.setHour(gps.time.hour());
    tx_msg.setMinute(gps.time.minute());
    tx_msg.setSecond(gps.time.second());
  } else {
    Serial.println("gps time not ready");
    gps_error = 1;
  }
  if (gps.date.isValid()) {
    tx_msg.setYear(gps.date.year());
    tx_msg.setMonth(gps.date.month());
    tx_msg.setDay(gps.date.day());
  } else {
    Serial.println("gps date not ready");
    gps_error = gps_error + 3;
  }
  if (gps.location.isValid()) {
    tx_msg.setLat(gps.location.lat());
    tx_msg.setLng(gps.location.lng());
  } else {
    Serial.println("gps location not ready");
    gps_error = gps_error + 7;
  }

  if (gps.altitude.isValid()) {
    tx_msg.setAltitudeFeed(gps.altitude.feet());
  } else {
    Serial.println("gps altitude not ready");
    gps_error = gps_error + 15;
  }
  if (gps.course.isValid()) {
    tx_msg.setCourse(gps.course.deg());
  } else {
    Serial.println("gps course not ready");
    gps_error = gps_error + 31;
  }

  if (gps.speed.isValid()) {
    tx_msg.setSpeedmph(gps.speed.mph());
  } else {
    Serial.println("gps speed not ready");
    gps_error = gps_error + 63;
  }

  // Serial.printf("lat:%s lng:%s\n", tx_msg._aprs_lat, tx_msg._aprs_lng);

  return gps_error;
};

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
