/*
 * File: APRSControler.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-28 0:35
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_APRSCONTROLER_H_
#define LIB_SLORAAPRS_APRSCONTROLER_H_

#include <Arduino.h>
#include <ButtonState.h>

enum DisplayMode {
  displayModeUTC,
  displayModeGPS,
  displayModeWX,
  displayModeWiFiStatus,
  displayModeEND
};

class AbstracButtonState;

class APRSControler {
  friend class AbstracButtonState;

 public:
  boolean config_mode = false;

  boolean gps_update = false;

  boolean display_change = true;
  boolean display_update = true;
  uint64_t next_display_time = 0;
  void nextDisplayMode(void);
  DisplayMode getCurrentDisplayMode(void);
  DisplayMode getNextDisplayMode(void);

  void singleClick();
  void doubleClick();
  void longClick();

  AbstracButtonState* button_state;

 private:
  uint8_t display_mode;
};

/* -------------------------------------------------------------------------- */

void singleClick_CB(void);
void doubleClick_CB(void);
void longClick_CB(void);
void kinoTimer_CB(TimerHandle_t xExpiredTimer);

#endif  // LIB_SLORAAPRS_APRSCONTROLER_H_
