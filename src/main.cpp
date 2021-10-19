/*
 * File: main.cpp
 * Project: src
 * File Created: 2021-03-07 20:08
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-19 4:34
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

// ==== Debug and Test options ==================
// #define _DEBUG_
// #define _TEST_

//===== Debugging macros ========================
// #ifdef _DEBUG_
// #define SerialD Serial SerialD.println(a)
// #define _PP(a) SerialD.print(a)
// #define _PL(a) SerialD.println(a)
// #define _PX(a) SerialD.println(a, HEX)
// #else
// #define _PM(a)
// #define _PP(a)
// #define _PL(a)
// #define _PX(a)
// #endif

// ==== INCLUDES ==================================================================================

#include <main.h>
#include <APRSWebServer.h>
#include <APRSWiFi.h>
#include <ESPFS.h>
#include <GPSSensor.h>
#include <I2CScanner.h>
#include <LoRaAPRSConfig.h>
#include <LoRaHandler.h>
#include <SPI.h>
#include <ButtonHandler.h>
#include <ButtonState.h>
#include <TrackerDisplay.h>
#include <OneButton.h>
#include <GPSSensor.h>
#include <Config.h>

// ==== Uncomment desired compile options =================================
// #define _TASK_SLEEP_ON_IDLE_RUN  // Enable 1 ms SLEEP_IDLE powerdowns between tasks if no callback methods were
// invoked during the pass
#define _TASK_TIMECRITICAL // Enable monitoring scheduling overruns
// #define _TASK_STATUS_REQUEST     // Compile with support for StatusRequest functionality - triggering tasks on status
// change events in addition to time only #define _TASK_WDT_IDS            // Compile with support for wdt control
// points and task ids #define _TASK_LTS_POINTER        // Compile with support for local task storage pointer #define
// _TASK_PRIORITY           // Support for layered scheduling priority #define _TASK_MICRO_RES          // Support for
// microsecond resolution #define _TASK_STD_FUNCTION       // Support for std::function (ESP8266 and ESP32 ONLY) #define
// _TASK_DEBUG              // Make all methods and variables public for debug purposes #define _TASK_INLINE // Make all
// methods "inline" - needed to support some multi-tab, multi-file implementations #define _TASK_TIMEOUT            //
// Support for overall task timeout #define _TASK_OO_CALLBACKS       // Support for dynamic callback method binding
// #define _TASK_DEFINE_MILLIS      // Force forward declaration of millis() and micros() "C" style
// #define _TASK_EXPOSE_CHAIN       // Methods to access tasks in the task chain
// #define _TASK_SCHEDULING_OPTIONS // Support for multiple scheduling options

#include <TaskScheduler.h>

// ==== GLOBALS ===================================================================================

ButtonHandler button_handler;
OneButton one_button(BUTTON, true);
TrackerDisplay td;
LoRaHandler lora_handler;
TinyGPSPlus gps;
Config cfg;
SendMsgForm sform;

// ==== Scheduler ==============================

Scheduler ts;

// Tasks
Task DisplayChanger(DISPLAY_CHANGE_TIME, TASK_FOREVER, &tracker_display_CB, &ts, true);
Task GPSReader(0, TASK_FOREVER, &GPSReadIdleHookCB, &ts, true);
Task WebSocketTick(333, TASK_FOREVER, &websocket_tickCB, &ts, true);
Task ButtonTick(100, TASK_FOREVER, &onebutton_tickCB, &ts, true);
Task LoRaTick(1000, TASK_FOREVER, &lorahandler_tickCB, &ts, true);
Task SendMSG(1050, TASK_FOREVER, &lora_send_tickCB, &ts, true);

void tasks_disable_all(void) {
  ESP_LOGD(TAG, "disable all tasks");
  // delay(3000);
  ts.disableAll();
  // delay(3000);
  ESP_LOGD(TAG, "all tasks disabled");
}

void tasks_enable_all(void) {
  ESP_LOGD(TAG, "enabel all tasks");
  // delay(3000);
  ts.enableAll();
  // delay(3000);
  ESP_LOGD(TAG, "all tasks enabled");
}

// TTGO has HW serial to GPS // 1 = first free UART
HardwareSerial ss(1);

void setup() {
  /*****   Init misc device BEGIN   *****/
  delay(500);
  Serial.begin(115200);

  ConfigInit();

  Serial.println("\n--------------------------");
  Serial.printf("Version: %s\n", cfg.Version.c_str());
  Serial.printf("Release: %s\n", cfg.Release.c_str());
  Serial.printf("Boot Count: %d\n", cfg.boot_count);
  Serial.println("--------------------------\n");

  if (!ESPFSInit()) {
    ESP_LOGE(TAG, "ERROR: Problem init ESP file system");
  }

  // Init UART1 for GPS device
  ss.begin(GPS_BAUD, SERIAL_8N1, TXPin, RXPin);

  /****   Init misc device END   ****/

  /****   Init I2C Begin   ****/
  ESP_LOGI(TAG, "Initialize I2C ... ");
  if (!Wire.begin(SDA, SCL)) {
    ESP_LOGE(TAG, "ERROR initialze I2C!");
  }
  I2CScanner i2csanner;
  i2csanner.scan();
  i2csanner.initialize();

  /****   Init I2C End   ****/

  // keine Ahnung, aber muss als Letztes gemacht werden.
  if (!lora_handler.begin()) {
    ESP_LOGE(TAG, "ERROR init LoRa TRX");
  }

  pinMode(TXLED, OUTPUT);

  one_button.attachClick(singleClick_CB);
  one_button.attachDoubleClick(doubleClick_CB);
  one_button.attachLongPressStop(longClick_CB);

  switch (cfg.current_run_mode) {
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

  // Debughilfe!
  // cfg.current_wifi_mode = wifi_client;
  // cfg.current_wifi_mode = wifi_ap;

  td.write3Line("RUN MODE", getRunMode().c_str(), "", true, DISPLAY_DELAY_LONG);
  td.write3Line("WiFi MODE", getWifiMode().c_str(), "", true, DISPLAY_DELAY_LONG);

  if (cfg.current_wifi_mode == wifi_ap) {
    ESP_LOGD(TAG, "start wifi_ap");
    WifiAPInit();
  }
  if (cfg.current_wifi_mode == wifi_client) {
    ESP_LOGD(TAG, "start wifi_client");
    WifiConnect();
  }
  if (cfg.current_wifi_mode != wifi_off) {
    ESP_LOGD(TAG, "start WebServer");
    WebserverStart();
  }

  td.write3Line("sLoRaAPRS", "up &", "running", true, DISPLAY_DELAY_MEDIUM);
  td.write3Line("Hello", cfg.call.c_str(), "nice to be back", true, DISPLAY_DELAY_MEDIUM);
  td.write3Line("Enjoy", "the", "day", true, DISPLAY_DELAY_MEDIUM);

  lora_handler.begin();
  button_handler.button_state = new StateDefault();
  ts.startNow();
}

void loop() { ts.execute(); }
