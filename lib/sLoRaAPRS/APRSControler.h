#ifndef APRS_CONTROLER_H
#define APRS_CONTROLER_H

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

#endif