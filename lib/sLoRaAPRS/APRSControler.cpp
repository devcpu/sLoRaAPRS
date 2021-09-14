/*
 * File: APRSControler.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-13 1:35
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <APRSControler.h>
#include <uxa_debug.h>

extern APRSControler maincontroler;


/**
 * APRSControler::getCurrentDisplayMode.
 * 
 * returns current DisplayMode
 *
 * @author	(DL7UXA) Johannes G.  Arlt
 * @global
 * @param	void	
 * @return	DisplayMode current DisplayMode
 */
DisplayMode APRSControler::getCurrentDisplayMode(void) {
  return (DisplayMode)display_mode;
}


/**
 * @brief sets the next display mode by calling APRSControler::nextDisplayMode() and return it.
 * 
 * @return <DisplayMode> next disply mode
 */
DisplayMode APRSControler::getNextDisplayMode(void) {
  nextDisplayMode();
  return (DisplayMode)display_mode;
}

/**
 * @brief set the next display mode for status display
 * 
 */
void APRSControler::nextDisplayMode(void) {
  uint8_t max = displayModeEND - 1;
  if (display_mode < max) {
    display_mode++;
  } else {
    display_mode = 0;
  }
}

// APRSControler::APRSControler() {
// }

/**
 * @brief callback 
 * 
 */
void APRSControler::singleClick() { button_state->singleClick(*this); }

void APRSControler::doubleClick(void) { button_state->doubleClick(*this); }

void APRSControler::longClick(void) { button_state->longClick(*this); }

void singleClick_CB(void) { maincontroler.singleClick(); }

void doubleClick_CB(void) { maincontroler.doubleClick(); }

void longClick_CB(void) { maincontroler.longClick(); }

void kinoTimer_CB(TimerHandle_t xExpiredTimer) {
  Serial.printf("\ncall of kinoTimer_CB\n");
  maincontroler.button_state->kino();
}
