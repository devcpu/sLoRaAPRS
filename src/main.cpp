/*
 * File: main.cpp
 * Project: src
 * File Created: 2021-03-07 20:08
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-16 0:27
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

// ==== Debug and Test options ==================
#define _DEBUG_
// #define _TEST_

//===== Debugging macros ========================
#ifdef _DEBUG_
#define SerialD Serial
#define _PM(a)                                                                                                         \
  SerialD.print(millis());                                                                                             \
  SerialD.print(": ");                                                                                                 \
  SerialD.println(a)
#define _PP(a) SerialD.print(a)
#define _PL(a) SerialD.println(a)
#define _PX(a) SerialD.println(a, HEX)
#else
#define _PM(a)
#define _PP(a)
#define _PL(a)
#define _PX(a)
#endif

// ==== INCLUDES ==================================================================================

#include <main.h>

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

// ==== Scheduler ==============================
Scheduler ts;

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

  button_handler.button_state = new StateDefault();
}

void loop() {
  vTaskDelay(portMAX_DELAY); // don't remove it!
  ESP_LOGE(TAG, "xxxxxxxxxx   ERROR xxx IN xxx LOOP   xxxxxxxxxx");
}

// void button_tick(void *pvParameters) {
//   (void)pvParameters;
//   int btn_old = 0;
//   int btn_now = 0;
//   int bstate = LOW;
//   uint32_t btn_long_press_old = 0;
//   // Flag marking if btn press a long press to preventing long press trigger
//   a
//   // short btn press event at the same time.
//   int longPressFlag = false;

//   while (1) {
//     // ESP_LOGE(TAG, "check button ...");
//     bstate = digitalRead(BUTTON);
//     if (bstate == LOW) {
//       btn_now = 1;
//     } else {
//       btn_now = 0;
//     }
//     if (btn_now != btn_old) {
//       if (btn_now == 1) {  // btn pressed down
//         // ESP_LOGE(TAG, "BTN get pressed down.");
//         btn_long_press_old = xTaskGetTickCount();
//         longPressFlag = false;
//       } else {  // btn released
//         // ESP_LOGE(TAG, "BTN get released.");
//         if (abs(xTaskGetTickCount() - btn_long_press_old) <
//                 1000 / portTICK_PERIOD_MS &&
//             !longPressFlag) {
//           ESP_LOGE(TAG, "short press");
//         }
//       }
//       btn_old = btn_now;
//       btn_long_press_old = xTaskGetTickCount();
//     } else if (btn_now == 1) {  // Check if this is a long press btn event
//       if (abs(xTaskGetTickCount() - btn_long_press_old) >
//           1000 / portTICK_PERIOD_MS) {
//         ESP_LOGE(TAG, "long btn press get triggered");
//         btn_long_press_old =
//             xTaskGetTickCount();  // Update timer preparing trigger the next
//                                   // long btn press event
//         longPressFlag = true;
//       }
//     } else {
//       // ESP_LOGE(TAG, "Button stays unpressed.");
//     }
//     vTaskDelay(100 / portTICK_PERIOD_MS);
//   }
// }
