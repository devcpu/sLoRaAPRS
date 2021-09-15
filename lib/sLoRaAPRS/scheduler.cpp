/*
 * File: scheduler.cpp
 * Project: sLoRaAPRS
 * File Created: 2021-09-15 1:16
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-15 12:55
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2021 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <scheduler.h>

extern xOneButton xob;

TimerHandle_t DisplayTimer;

TaskHandle_t ButtonHandle = NULL;

TaskHandle_t WSHandle = NULL;

void scheduler_init() {
  /**
   * @brief handle for start, stop button scheduler tick
   *
   */
  if (xTaskCreate(xbutton_tick, "ButtonTick", 10000, NULL, 2, &ButtonHandle) ==
      pdPASS) {
    ESP_LOGE(TAG, "Task ButtonTick created!");
  }

  if (xTaskCreate(xws_tick, "WebServerTick", 10000, NULL, 2, &WSHandle) ==
      pdPASS) {
    ESP_LOGE(TAG, "Task WebServerTick created!");
  }

  /***************** Timers *******************/

  DisplayTimer = xTimerCreate("DisplayTimer", pdMS_TO_TICKS(DISPLAY_KINO_TIME),
                              pdTRUE, reinterpret_cast<void *>(23),
                              DisplayTimerCB);  // @FIXME replace 23
  if (DisplayTimer == NULL) {
    ESP_LOGE(TAG, "Creation of DisplayTimer failed");
  }
  if (xTimerStart(DisplayTimer, 0) != pdPASS) {
    ESP_LOGE(TAG, "Start of DisplayTimer failed");
  } else {
    ESP_LOGE(TAG, "DisplayTimer started");
  }
}

void xbutton_tick(void *pvParameters) {
  (void)pvParameters;
  xob.tick();
}

void xws_tick(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    APRSWebServerTick();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void DisplayTimerCB(TimerHandle_t pxTimer) {
  // ESP_LOGE(TAG, "Timer fire");
}
