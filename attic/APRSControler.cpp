/*
 * File: APRSControler.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-28 23:52
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <Scheduler.h>

extern Scheduler taskScheduler;


APRSControler::APRSControler(){}


/**
 * @brief callback
 *
 */
void APRSControler::singleClick() { button_state->singleClick(*this); }

void APRSControler::doubleClick(void) { button_state->doubleClick(*this); }

void APRSControler::longClick(void) { button_state->longClick(*this); }

void singleClick_CB(void) { taskScheduler.singleClick(); }

void doubleClick_CB(void) { taskScheduler.doubleClick(); }

void longClick_CB(void) { taskScheduler.longClick(); }

void kinoTimer_CB(TimerHandle_t xExpiredTimer) {
  Serial.printf("\ncall of kinoTimer_CB\n");
  taskScheduler.button_state->kino();
}
