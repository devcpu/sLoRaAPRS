/*
 * File: scheduler.cpp
 * Project: sLoRaAPRS
 * File Created: 2021-09-15 1:16
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-04 10:09
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2021 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <Scheduler.h>

extern xOneButton xob;
extern Scheduler taskScheduler;
extern Config cfg;
extern TrackerDisplay td;

/**
 * @brief Handler to stop/start etc button task
 *
 */
TaskHandle_t ButtonHandle = NULL;

/**
 * @brief
 *
 */
TaskHandle_t WSHandle = NULL;

TimerHandle_t DisplayTimer;

Scheduler::Scheduler() { ESP_LOGD(TAG, "call Scheduler::Scheduler"); }

void Scheduler::init(void) {
  ESP_LOGD(TAG, "call Scheduler::init");

  // if (xTaskCreate(xws_tick, "WebServerTick", 100000, NULL, 2, &WSHandle) ==
  //     pdPASS) {
  //   ESP_LOGD(TAG, "Task WebServerTick created!");
  // } else {
  //   ESP_LOGE(TAG, "can't create WebServerTick");
  // }

  // if (xTaskCreate(xbutton_tick, "ButtonTick", 10000, NULL, 2, &ButtonHandle)
  // ==
  //     pdPASS) {
  //   ESP_LOGD(TAG, "Task ButtonTick created!");
  // } else {
  //   ESP_LOGE(TAG, "Can't create ButtonTick");
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
    ESP_LOGD(TAG, "DisplayTimer started");
  }

  // esp_register_freertos_idle_hook(GPSReadIdleHookCB);
}

void Scheduler::ResumeForOLD(void) {
  vTaskResume(ButtonHandle);
  vTaskResume(WSHandle);
}

void Scheduler::SuspendForOLD(void) {
  vTaskSuspend(ButtonHandle);
  vTaskSuspend(WSHandle);
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

void DisplayTimerCB(TimerHandle_t pxTimer) {
  // ESP_LOGE(TAG, "Timer fire");
  td.nextDisplayMode();
  // taskScheduler.SuspendForOLD();

  td.showDisplayMode();

  // taskScheduler.ResumeForOLD();
  // write3Line("  Enjoy", "   the", "   day", true, 0);
}

bool GPSReadIdleHookCB() {
  if (ss.available() > 0) {  // @TODO is this nessesery?
    if (gps.encode(ss.read())) {
      setGPSData();
    }
    // vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  return true;
}

void Scheduler::singleClick() {
  ESP_LOGD(TAG, "button fired");
  button_state->singleClick(*this);
}

void Scheduler::doubleClick(void) {
  ESP_LOGD(TAG, "button fired");
  button_state->doubleClick(*this);
}

void Scheduler::longClick(void) {
  ESP_LOGD(TAG, "button fired");
  button_state->longClick(*this);
}

void Scheduler::nextDisplayMode(void) { ESP_LOGD(TAG, "fire nextDisplayMode"); }

void singleClick_CB(void) {
  ESP_LOGD(TAG, "button fired");
  taskScheduler.singleClick();
}

void doubleClick_CB(void) {
  ESP_LOGD(TAG, "button fired");
  taskScheduler.doubleClick();
}

void longClick_CB(void) {
  ESP_LOGD(TAG, "button fired");
  taskScheduler.longClick();
}

// void kinoTimer_CB(TimerHandle_t xExpiredTimer) {
//   taskScheduler.button_state->kino();
// }
