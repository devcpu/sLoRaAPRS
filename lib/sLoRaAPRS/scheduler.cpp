/*
 * File: scheduler.cpp
 * Project: sLoRaAPRS
 * File Created: 2021-09-15 1:16
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-25 23:27
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2021 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <scheduler.h>

extern xOneButton xob;
extern TinyGPSPlus gps;
extern HardwareSerial ss;

TaskHandle_t ButtonHandle = NULL;
TaskHandle_t WSHandle = NULL;
TaskHandle_t GPSReadHandle = NULL;

TimerHandle_t DisplayTimer;
TimerHandle_t GPSSetTimer;
TimerHandle_t GPSReadTimer;

void scheduler_init() {
  /**
   * @brief handle for start, stop button scheduler tick
   *
   */
  if (xTaskCreate(xbutton_tick, "ButtonTick", 10000, NULL, 2, &ButtonHandle) ==
      pdPASS) {
    ESP_LOGD(TAG, "Task ButtonTick created!");
  }

  if (xTaskCreate(xws_tick, "WebServerTick", 10000, NULL, 2, &WSHandle) ==
      pdPASS) {
    ESP_LOGD(TAG, "Task WebServerTick created!");
  }

  // if (xTaskCreate(gps_read, "GPS Read", 100000, NULL, 2, &GPSReadHandle) ==
  //     pdPASS) {
  //   ESP_LOGD(TAG, "Task GPSRead created");
  // }

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

  // GPSSetTimer = xTimerCreate("GPSSetTimer", pdMS_TO_TICKS(500),
  //                             pdTRUE, reinterpret_cast<void *>(17),
  //                             GPSSetTimerCB);  // @FIXME replace 17
  // if (GPSSetTimer == NULL) {
  //   ESP_LOGE(TAG, "Creation of GPSSetTimer failed");
  // }
  // if (xTimerStart(GPSSetTimer, 0) != pdPASS) {
  //   ESP_LOGE(TAG, "Start of GPSSetTimer failed");
  // } else {
  //   ESP_LOGE(TAG, "GPSSetTimer started");
  // }

  // GPSReadTimer = xTimerCreate("GPSReadTimer", pdMS_TO_TICKS(100),
  //                             pdTRUE, reinterpret_cast<void *>(17),
  //                             GPSReadTimerCB);  // @FIXME replace 17
  // if (GPSReadTimer == NULL) {
  //   ESP_LOGE(TAG, "Creation of GPSReadTimer failed");
  // }
  // if (xTimerStart(GPSReadTimer, 0) != pdPASS) {
  //   ESP_LOGE(TAG, "Start of GPSReadTimer failed");
  // } else {
  //   ESP_LOGE(TAG, "GPSReadTimer started");
  // }

  esp_register_freertos_idle_hook(GPSReadIdleHookCB);
}

void xbutton_tick(void *pvParameters) {
  (void)pvParameters;
  xob.tick();
  vTaskDelay(200 / portTICK_PERIOD_MS);
}

void xws_tick(void *pvParameters) {
  (void)pvParameters;
  while (1) {
    APRSWebServerTick();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void gps_read(void *pvParameters) {
  (void)pvParameters;
  while (ss.available() > 0) {
    gps.encode(ss.read());
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void DisplayTimerCB(TimerHandle_t pxTimer) {
  // ESP_LOGE(TAG, "Timer fire");
}

void GPSSetTimerCB(TimerHandle_t pxTimer) { setGPSData(); }

void GPSReadTimerCB(TimerHandle_t pxTimer) {
  if (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      setGPSData();
    }
  }
}

bool GPSReadIdleHookCB() {
  if (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      setGPSData();
    }
  }
  return true;
}
