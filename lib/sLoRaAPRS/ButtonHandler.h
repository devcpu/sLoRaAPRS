/*
 * File: ButtonHandler.h
 * Project: sLoRaAPRS
 * File Created: 2021-10-15 23:12
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-16 0:06
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2021 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_BUTTONHANDLER_H_
#define LIB_SLORAAPRS_BUTTONHANDLER_H_

#include <Config.h>
#include <OneButton.h>
#include <GPSSensor.h>
#include <ButtonState.h>
#include <APRSWebServer.h>
#include <TrackerDisplay.h>

class AbstracButtonState;

class ButtonHandler {
  friend class AbstracButtonState;

 public:
  /**
   * @brief Construct a new Scheduler object
   *
   */
  ButtonHandler();

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
void tracker_display_CB(void *pvParameters);

extern HardwareSerial ss;
extern ButtonHandler button_handler;

#endif // LIB_SLORAAPRS_BUTTONHANDLER_H_
