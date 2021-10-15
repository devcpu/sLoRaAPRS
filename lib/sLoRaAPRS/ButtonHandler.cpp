/*
 * File: ButtonHandler.cpp
 * Project: sLoRaAPRS
 * File Created: 2021-10-15 23:12
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-16 0:12
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2021 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <ButtonHandler.h>

extern OneButton one_button;
extern Config cfg;

void xbutton_tick() { one_button.tick(); }

void xws_tick() {
  while (1) {
    APRSWebServerTick();
  }
}

bool GPSReadIdleHookCB() {
  if (ss.available() > 0) { // @TODO is this nessesery?
    if (gps.encode(ss.read())) {
      setGPSData();
    }
    // vTaskDelay(50 / portTICK_PERIOD_MS);
  }
  return true;
}

ButtonHandler::ButtonHandler() { ESP_LOGD(TAG, "call ButtonHandler::ButtonHandler"); }

void ButtonHandler::singleClick() {
  ESP_LOGD(TAG, "button fired");
  button_state->singleClick(*this);
}

void ButtonHandler::doubleClick(void) {
  ESP_LOGD(TAG, "button fired");
  button_state->doubleClick(*this);
}

void ButtonHandler::longClick(void) {
  ESP_LOGD(TAG, "button fired");
  button_state->longClick(*this);
}

void ButtonHandler::nextDisplayMode(void) { ESP_LOGD(TAG, "fire nextDisplayMode"); }

void singleClick_CB(void) {
  ESP_LOGD(TAG, "button fired");
  button_handler.singleClick();
}

void doubleClick_CB(void) {
  ESP_LOGD(TAG, "button fired");
  button_handler.doubleClick();
}

void longClick_CB(void) {
  ESP_LOGD(TAG, "button fired");
  button_handler.longClick();
}

void tracker_display_CB(void *pvParameters) {
  while (1) {
    td.nextDisplayMode();
    td.showDisplayMode();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}
