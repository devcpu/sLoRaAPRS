/*
 * File: APRSControler.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-27 23:58
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_APRSCONTROLER_H_
#define LIB_SLORAAPRS_APRSCONTROLER_H_

#include <Arduino.h>
#include <ButtonState.h>


class AbstracButtonState;

class APRSControler {
  friend class AbstracButtonState;

 public:

  APRSControler();


  void singleClick();
  void doubleClick();
  void longClick();

  AbstracButtonState *button_state;

 private:
  uint8_t display_mode;
};

/* -------------------------------------------------------------------------- */

void singleClick_CB(void);
void doubleClick_CB(void);
void longClick_CB(void);
void kinoTimer_CB(TimerHandle_t xExpiredTimer);

#endif  // LIB_SLORAAPRS_APRSCONTROLER_H_
