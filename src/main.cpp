/*
 * File: main.cpp
 * Project: src
 * File Created: 2021-03-07 20:08
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-19 0:46
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <main.h>

void singleClick() { ESP_LOGE(TAG, "singleClick"); }

void doubleClick() { ESP_LOGE(TAG, "doubleClick"); }

void longClick() {
  ESP_LOGE(TAG, "longClick");
  vTaskDelay(500 / portTICK_PERIOD_MS);
}

xOneButton xob(BUTTON, true);

APRSControler maincontroler;

// TTGO has HW serial to GPS // 1 = first UART
HardwareSerial ss(1);

void setup() {
  /*****   Init misc device BEGIN   *****/
  vTaskDelay(500 / portTICK_PERIOD_MS);
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
  if (!LoRa_init()) {
    ESP_LOGE(TAG, "ERROR init LoRa TRX");
  }

  pinMode(TXLED, OUTPUT);

  xob.attachClick(singleClick);
  xob.attachDoubleClick(doubleClick);
  xob.attachDuringLongPress(longClick);

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

  write3Line(" RUN MODE", getRunMode().c_str(), "", true, DISPLAY_DELAY_LONG);
  write3Line("WiFi MODE", getWifiMode().c_str(), "", true, DISPLAY_DELAY_LONG);

  if (cfg.current_wifi_mode == wifi_ap) {
    ESP_LOGD(TAG, "start wifi_ap");
    WifiAPInit();
  }
  if (cfg.current_wifi_mode == wifi_client) {
    ESP_LOGD(TAG, " start wifi_client");
    WifiConnect();
  }
  if (cfg.current_wifi_mode != wifi_off) {
    ESP_LOGD(TAG, " start WebServer");
    WebserverStart();
  }

  scheduler_init();

  write3Line("sLoRaAPRS", "  up &", " running", true, DISPLAY_DELAY_MEDIUM);
  write3Line("  Hello", (String("  ") + cfg.call).c_str(), "  nice to be back",
             true, 2000);
  write3Line("  Enjoy", "   the", "   day", true, DISPLAY_DELAY_MEDIUM);
}

void loop() {
  vTaskDelay(portMAX_DELAY);  // don't remove it!
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
