/*
 * File: scheduler.h
 * Project: sLoRaAPRS
 * File Created: 2021-09-15 1:16
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-04 0:53
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_SCHEDULER_H_
#define LIB_SLORAAPRS_SCHEDULER_H_

#include <APRSWebServer.h>
#include <Arduino.h>
#include <ButtonState.h>
#include <GPSSensor.h>
#include <LoRaAPRSConfig.h>
#include <TinyGPS++.h>
#include <TrackerDisplay.h>
#include <esp_freertos_hooks.h>
#include <xOneButton.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

extern TinyGPSPlus gps;
extern HardwareSerial ss;

class AbstracButtonState;

class Scheduler {
  friend class AbstracButtonState;

 public:
  /**
   * @brief Construct a new Scheduler object
   *
   */
  Scheduler();

  void init(void);

  boolean config_mode = false;

  boolean gps_update = false;

  boolean display_change = true;
  boolean display_update = true;
  uint64_t next_display_time = 0;
  void nextDisplayMode(void);

  void singleClick();
  void doubleClick();
  void longClick();

  void ResumeForOLD(void);
  void SuspendForOLD(void);

  AbstracButtonState *button_state;

 private:
};

/**
 * @brief xTask callback function for default button tick
 *
 * @param pvParameters optional
 */
void xbutton_tick(void *pvParameters);

/**
 * @brief xTask callback function for webserver tick to start/stop etc.
 *
 * @param pvParameters optional
 */
void xws_tick(void *pvParameters);

/**
 * @brief call back function for OLED display show
 *
 * @param pxTimer
 */
void DisplayTimerCB(TimerHandle_t pxTimer);

/**
 * @brief Idle task call back, read from GPS-Sensor
 *
 * @return true ever
 * @return false never
 */
bool GPSReadIdleHookCB();

void singleClick_CB(void);
void doubleClick_CB(void);
void longClick_CB(void);
void kinoTimer_CB(TimerHandle_t xExpiredTimer);

#endif  // LIB_SLORAAPRS_SCHEDULER_H_
