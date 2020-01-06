#ifdef ARDUINO
#include <Arduino.h>
#endif

#ifndef ARDUINO
#include <tgmath.h>
#include <iostream>
#endif

#include <APRSWebServer.h>
#include <APRSWiFi.h>
#include <APRS_MSG.h>
#include <EEPROM.h>
#include <Esp.h>
#include <LoRaAPRSConfig.h>
#include <LoRaHandler.h>
#include <OneButtonHandler.h>
#include <Registry.h>
#include <SPI.h>
#include <TrackerDisplay.h>
#include <Wire.h>
#include <main.h>

extern Registry reg;  // config & system status

extern AsyncWebServer *WebServer;
extern AsyncWebSocket *ws;

// @TODO APRS_MSG deprecated?
APRS_MSG tx_msg;  // converts all data to APRS messages
TinyGPSPlus gps;  // driver fot GPS

uint32_t lastTx = 0L, nextTx = 10000L;  // ticker for APRS messages
uint8_t gps_error = 0;
char txmsg[254];

uint8_t odd = 0;
char satbuf[24] = "";

OneButton button(BUTTON, true);

void setup() {
  delay(500);
  // ESP.deepSleep(1, WAKE_RF_DISABLED);
  Serial.begin(115200);
  delay(200);
  Serial.println("sLoRaAPRS system starting ...");

  // #ifdef T_BEAM_V1_0
  //   initAXP();
  // #endif

  if (wxSensor.begin() ==
      false)  // Begin communication over I2C and Address 0x77
  {
    Serial.println("The sensor did not respond. Please check wiring.");

  } else {
    Serial.println(wxSensor.readFixedTempC() / 100.0);
    Serial.println(wxSensor.readFixedHumidity() / 1000.0);
    Serial.println(wxSensor.readFixedPressure() / 100.0);
  }
  delay(2000);

  initDisplay();

  ESPFSInit();

  RegistryInit();
  // dumpEEPROM();

  // @TOTO UART for ESP8266 GPS
  // #ifdef ESP32
  //   Serial.println("GPS UART setup ...");
  //   ss.begin(GPS_BAUD, SERIAL_8N1, TXPin, RXPin);
  // #elif defined(ESP8266)
  // #endif

  //  tx_msg.reset();

  // initLoRa();

  // pinMode(TXLED, OUTPUT);

  // initOneButton();

  Serial.printf("system run mode %d\n", reg.current_system_mode);
  Serial.printf("wifi mode %d\n", reg.current_wifi_mode);

  Serial.printf("system run mode %d\n", reg.current_system_mode);
  Serial.printf("wifi mode %d\n", reg.current_wifi_mode);

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

  Serial.println("sLoRaAPRS up & running");
  // Serial.print("sLoRaAPRS up & running");
  Serial.println("enjoy\n");
}

void loop() {
  // watchdog();
  APRSWebServerTick();
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
  // tracker_display_tick();
  // odd++;
  // // Serial.printf("++++++++++   Starte neue Loop-Runde  Zeit: %s-%s-%d
  // // ++++++++++\n", tx_msg._gps_hour, tx_msg._gps_minute, gps.time.second());
  // if ((lastTx + nextTx) < millis()) {  // start Tx
  //   // Serial.printf("%s-%s-%s age: %d \n", tx_msg._gps_year,
  //   tx_msg._gps_month,
  //   // tx_msg._gps_day, gps.date.age());
  //   Serial.printf("\nsatellites: %d ", gps.satellites.value());
  //   Serial.printf("hdop: %03.1f\n", gps.hdop.hdop());

  //   gps_error = setAllGPSData();
  //   if (!gps_error) {
  //     // sendMessage(tx_msg.getAPRSTxPos(txmsg));
  //     sendMessage(tx_msg.getAPRSTxTrackInfo(txmsg));
  //   } else {
  //     Serial.printf("GPS not ready %d\n", gps_error);
  //   }
  //   lastTx = millis();  // gps not ok we wait also
  // }
  // smartDelay(50);
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

void initAXP() {
  Wire.begin(21, 22);
  if (!axp.begin(Wire, AXP192_SLAVE_ADDRESS)) {
    Serial.println("AXP192 Begin PASS");
  } else {
    Serial.println("AXP192 Begin FAIL");
  }
  axp.setPowerOutPut(AXP192_LDO2, AXP202_ON);
  axp.setPowerOutPut(AXP192_LDO3, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC2, AXP202_ON);
  axp.setPowerOutPut(AXP192_EXTEN, AXP202_ON);
  axp.setPowerOutPut(AXP192_DCDC1, AXP202_ON);
  axp.setDCDC1Voltage(3300);
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
