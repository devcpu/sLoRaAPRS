#ifndef APRS_CONTROLER_H
#define APRS_CONTROLER_H

#include <ButtonState.h>
#include <Arduino.h>


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

// struct ControlData {
//   boolean gps_update = false;
//   boolean new_mode = false;
//   boolean newTxMesg = false;
//   boolean newRxMesg = false;
//   DisplayMode current_display_mode;
//   boolean display_change = true;
// };

#endif