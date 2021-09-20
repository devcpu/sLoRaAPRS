/*
 * File: main_old.cpp
 * Project: attic
 * File Created: 2021-09-15 2:45
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-18 23:21
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2021 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <APRSControler.h>
#include <APRSWebServer.h>
#include <APRSWiFi.h>
#include <APRS_MSG.h>
#include <Arduino.h>
#include <AsyncTCP.h>
#include <BMEHandler.h>
#include <ButtonState.h>
#include <Esp.h>
#include <GPSSensor.h>
#include <LoRaAPRSConfig.h>
#include <LoRaHandler.h>
#include <OneButton.h>
#include <SPI.h>
#include <TrackerDisplay.h>
// #include <fap.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <main_old.h>

#include "freertos/timers.h"

// #include <iGate.h>

extern Config reg;  // config & system status

APRSControler maincontroler;

// @TODO APRS_MSG deprecated?
APRS_MSG tx_msg;         // converts all data to APRS messages
extern TinyGPSPlus gps;  // driver for GPS

QueueHandle_t LoRaTXQueue, LoRaRXQueue, WWWTXQueue, WWWRXQueue;

TimerHandle_t button_timer;

uint32_t waitTxTr = 0L, nextTxTr = 50000L,
         nextTxTrRand = 15;  // ticker for APRS messages
uint32_t waitTxDg = 0L, nextTxDg = 50000L, nextTxDgRand = 15;

uint32_t wait_wx_update = 0, next_wx_update = 10000;
uint64_t nextRX = 0;
uint8_t gps_error = 0;
char txmsg[254];

uint8_t odd = 0;
char satbuf[24] = "";

char *input;
unsigned int input_len;
// fap_packet_t* packet;

OneButton button(BUTTON, true);

void setup() {
  Serial.begin(115200);
  delay(1000);

  /* --------------------------------------------------------------------------
   */
  /*   hardware check   */
  /* --------------------------------------------------------------------------
   */

  ConfigInit();
  // write3Line("Init Pref.", "Prefernces", "   +OK", true,
  // DISPLAY_DELAY_SHORT);

  if (Wire.begin(SDA, SCL)) {
    // write3toSerial("Init I2C", "  System", "   +OK", DISPLAY_DELAY_SHORT);
  } else {
    // write3toSerial("Init I2C", "   -ERR", "check wire", DISPLAY_DELAY_LONG);
  }

  if (DisplayInit()) {
    // write3Line("Init Disp.", "Display", "   +OK", true, DISPLAY_DELAY_SHORT);
    reg.hardware.OLED = true;
  } else {
    // write3Line("Init Disp.", "   -ERR", "check wire", true,
    // DISPLAY_DELAY_LONG);
    reg.hardware.OLED = false;
  }

  /*
    // write3Line({{""}, {""}, {""}}, true, 1);
  #ifdef T_BEAM_V1_0
    if (initAXP()) {
      // write3Line("Init AXP", " AXP 192", "   +OK", true,
  DISPLAY_DELAY_SHORT); reg.hardware.AXP192 = true; } else {
      // write3Line("Init AXP", "   -ERR", "check wire", true,
  DISPLAY_DELAY_LONG); reg.hardware.AXP192 = false;
    }
  #endif
  */
  if (BMEHandlerInit()) {
    // write3Line("Init BME", "  BME280", "   +OK", true, DISPLAY_DELAY_SHORT);
    reg.hardware.BME280 = true;
  } else {
    // write3Line("Init BME", "   -ERR", "check wire", true,
    // DISPLAY_DELAY_LONG);
    reg.hardware.BME280 = false;
  }

  if (ESPFSInit()) {
    // write3Line("Init SPIFS", "  SPIFFS", "   +OK", true,
    // DISPLAY_DELAY_SHORT);
  } else {
    // write3Line("Init SPIFS", "   -ERR", "check chip", true,
    // DISPLAY_DELAY_SHORT);
  }

  ss.begin(GPS_BAUD, SERIAL_8N1, TXPin, RXPin);
  // write3Line("Init UART", " GPS UART", "   +OK", true, DISPLAY_DELAY_SHORT);

  if (LoRa_init()) {
    // write3Line("Init LoRa", "LoDaDevive", "   +OK", true,
    // DISPLAY_DELAY_SHORT);
  } else {
    // write3Line("Init LoRa", "   -ERR", "check wire", true,
    // DISPLAY_DELAY_SHORT);
  }
  // lora.onReceive(lora.processMessage);
  // lora.receive();

  if (reg.hardware.OLED) {
    maincontroler.button_state = new StateDefault();
    button.attachClick(singleClick_CB);
    button.attachDoubleClick(doubleClick_CB);
    button.attachLongPressStop(longClick_CB);
    // write3Line("Init 1BUT", "OneButton", "   +OK", true,
    // DISPLAY_DELAY_SHORT);
    int timerid = 2;
    button_timer =
        xTimerCreate("ButtonTimer", pdMS_TO_TICKS(100), pdTRUE,
                     reinterpret_cast<void *>(timerid), &button_tick);
    if (NULL == button_timer) {
      Serial.printf("-ERR: can't create ButtonTimer\n");
    }
    if (xTimerStart(button_timer, 0) == pdFALSE) {
      Serial.printf("-ERR: can't start ButtonTimer! timer-queue full?\n");
    }
  }
  pinMode(TXLED, OUTPUT);

  /* --------------------------------------------------------------------------
   */
  /*   run mode   */
  /* --------------------------------------------------------------------------
   */

  switch (reg.current_run_mode) {
    case mode_tracker:

      break;

    case mode_wxtracker:

      break;

    case mode_wxfix:

      break;

    case mode_digi:

      break;

    case mode_gateway:

      break;

    case mode_digi_gateway:

      break;

    default:
      break;
  }

  // reg.current_wifi_mode = wifi_client;
  reg.current_wifi_mode = wifi_ap;

  write3Line(" RUN MODE", getRunMode().c_str(), "", true, DISPLAY_DELAY_MEDIUM);
  write3Line("WiFi MODE", getWifiMode().c_str(), "", true,
             DISPLAY_DELAY_MEDIUM);

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

  // Scanner();

  LoRaTXQueue = xQueueCreate(3, sizeof(char) * 256);
  LoRaRXQueue = xQueueCreate(3, sizeof(char) * 256);
  WWWTXQueue = xQueueCreate(3, sizeof(char) * 256);
  WWWRXQueue = xQueueCreate(3, sizeof(char) * 256);

  write3Line("sLoRaAPRS", "  up &", " running", true, 0);
  smartDelay(2000);
  write3Line("  Hello", (String("  ") + reg.call).c_str(), "  nice to be back",
             true, 0);
  smartDelay(2000);
  write3Line("  Enjoy", "   the", "   day", true, 0);
  smartDelay(2000);
  setWXData();

  // fap_init();
  // randomSeed(ESP.getCycleCount()); read from GPS if time valide pick up a
  // good value (ymd+(hour*60*60+min*60)) ESP.deepSleep(1, WAKE_RF_DISABLED);
}

void loop() {
  setGPSData();
  // Serial.print('*');
  // if (reg.oled_message.active == true) {
  //   write2Display(reg.oled_message.head, reg.oled_message.line1,
  //                 reg.oled_message.line2, reg.oled_message.line3,
  //                 reg.oled_message.line4);
  //   reg.oled_message.active = false;
  // }

  // if (wait_wx_update < millis()) {
  //   setWXData();
  //   wait_wx_update = next_wx_update + millis();
  // }
  // watchdog();
  APRSWebServerTick();
  // iGate_process_udp();
  // if (reg.TxMsg.newmessage) {
  //   Serial.println("########################");
  //   Serial.println("to: " + reg.TxMsg.to);
  //   Serial.println("via: " + reg.TxMsg.gateways);
  //   Serial.println("msg: " + reg.TxMsg.msg);
  //   reg.TxMsg.processed = true;
  //   reg.TxMsg.newmessage = false;
  //   Serial.println("########################");
  // }

  button_tick();
  LoRa_tick();
  tracker_display_tick();
  Sensor_tick();

  // if (waitTxTr < millis() && !lora_control.isSend) {  // start Tx
  //    char msg_buf[256] = {0};
  //    char aprs_buf[32] = {0};
  //    char wx_buf[128] = {0};
  //    char track_buf[128] = {0};
  //   snprintf(msg_buf, 256, "%s>APRS:!%s%s %s uptime [%u] send to tracker",
  //     reg_aprsCall().c_str(),
  //     APRS_MSG::computeAPRSPos(aprs_buf),
  //     APRS_MSG::computeTrackInfo(track_buf),
  //     APRS_MSG::computeWXField(wx_buf),
  //     millis()/1000);
  //   waitTxTr = millis() + nextTxTr + random(nextTxTrRand) * 1000;
  // }

  // if (waitTxDg < millis() && !lora_control.isSend) {  // start Tx
  //   uint64_t t0 = millis();
  //   char aprs_buf[32] = {0};
  //   char wx_buf[128] = {0};
  //   char track_buf[128] = {0};
  //   char msg_buf[256] = {0};
  //   snprintf(msg_buf, 256, "%s>APRS:!%s%s %s uptime [%lu] send to digi",
  //            reg_aprsCall().c_str(), APRS_MSG::computeAPRSPos(aprs_buf),
  //            APRS_MSG::computeTrackInfo(track_buf),
  //            APRS_MSG::computeWXField(wx_buf), millis() / 1000);
  //   sendMessage(msg_buf, true);
  //   uint64_t t1 = millis();
  //   Serial.printf("sendtime: %llu\n", t1 - t0);
  //   waitTxDg = millis() + nextTxDg + random(nextTxDgRand) * 1000;
  // }

  smartDelay(50);
}

/**
 * initAXP.
 *
 * @author	Johannes Arlt
 * @return	boolean
 */

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
  uint64_t start = millis();
  do {
    while (ss.available()) {
      gps.encode(ss.read());
    }
  } while (millis() - start < ms);
}

/**
 * @brief restarts system if millis() is overruned.
 * This function must called in every loop.
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

void button_tick(TimerHandle_t xExpiredTimer) { button.tick(); }

void button_tick() { button.tick(); }
