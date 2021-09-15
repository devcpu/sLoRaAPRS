/*
 * File: ButtonState.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-28 1:36
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <Arduino.h>
#include <ButtonState.h>
#include <Config.h>
#include <TrackerDisplay.h>
#include <uxa_debug.h>
#include <xOneButton.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

char wifi_mode_txt[3][16] = {"OFF", "AP", "STA"};
char run_mode_txt[6][16] = {"Tracker", "WXTracker", "WX Fix",
                            "Digi",    "Gateway",   "GW-Digi"};

TimerHandle_t call_config_timer;

AbstracButtonState::~AbstracButtonState() {
  // DDD("AbstracButtonState::~AbstracButtonState");
}

void AbstracButtonState::setState(APRSControler &aprs_controler,
                                  AbstracButtonState *state) {
  AbstracButtonState *aux = aprs_controler.button_state;
  aprs_controler.button_state = state;
  delete aux;
}

void AbstracButtonState::kino(void) {}

void AbstracButtonState::longClick(APRSControler &aprs_controler) {
  aprs_controler.display_change = true;
  aprs_controler.display_update = true;
  setState(aprs_controler, new StateDefault());
}

/* -------------------------------------------------------------------------- */
/*   Display Mode   */
/* -------------------------------------------------------------------------- */

void StateDisplayMode::singleClick(APRSControler &aprs_controler) {
  // DDD("StateDisplayMode.singleClick");
  aprs_controler.nextDisplayMode();
  aprs_controler.next_display_time = 0;
}
void StateDisplayMode::doubleClick(APRSControler &aprs_controler) {
  // DDD("StateDisplayMode.doubleClick");
  aprs_controler.display_change = true;
  aprs_controler.nextDisplayMode();
  aprs_controler.next_display_time = 0;
  setState(aprs_controler, new StateDefault());
}

/* -------------------------------------------------------------------------- */
/*   Default   */
/* -------------------------------------------------------------------------- */

void StateDefault::singleClick(APRSControler &aprs_controler) {
  // DDD("StateDefault.singleClick");
  aprs_controler.display_change = false;
  setState(aprs_controler, new StateDisplayMode());
}

void StateDefault::doubleClick(APRSControler &aprs_controler) {
  // DDD("StateDefault.doubleClick");
}

void StateDefault::longClick(APRSControler &aprs_controler) {
  // DDD("StateDefault.longClick()");
  aprs_controler.display_change = false;
  aprs_controler.display_update = false;
  setState(aprs_controler, new StateConfigMenue());
}

/* -------------------------------------------------------------------------- */
/*   Config Menue   */
/* -------------------------------------------------------------------------- */

StateConfigMenue::StateConfigMenue(void) {
  // DDD("StateConfigMenue::StateConfigMenue");
  // vTaskDelay(pdMS_TO_TICKS(500));
  write3Line("Cnfg Mode", "1clck nxt, 2clck ", "long click exit", false, 0);
}

void StateConfigMenue::singleClick(APRSControler &aprs_controler) {
  // DDD("StateConfigMenue.singleClick");
  uint8_t max_items = 3;  // count menue items
  if (button_config_mode < max_items - 1) {
    button_config_mode++;
  } else {
    button_config_mode = 0;
  }
  switch (button_config_mode) {
    case 0:
      write3Line("Cnfg Call", "configure call", "double click to enter", false,
                 0);
      break;
    case 1:
      write3Line("Cfg WiFi", "configure WiFi mode", "double click to enter",
                 false, 0);
      break;
    case 2:
      write3Line("Cfg Run", "configure run mode", "double click to enter",
                 false, 0);
      break;
    default:
      write3Line("ERROR", "something wars going wrong", "please try again",
                 false, 2000);
      setState(aprs_controler, new StateConfigMenue());
      break;
  }
  DDE("button_config_mode=", String(button_config_mode));
}

void StateConfigMenue::doubleClick(APRSControler &aprs_controler) {
  // DDD("StateConfigMenue.doubleClick");
  const char select_list_call[40] = {
      ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
      'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
      'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

  switch (button_config_mode) {
    case 0:
      setState(aprs_controler,
               new ConfigStringSelector("Cfg Call", select_list_call));
      break;
    case 1:
      setState(aprs_controler, new StateConfigWiFi("Cfg WiFi"));
      break;

    case 2:
      setState(aprs_controler, new StateConfigRun("Cfg Run"));
      break;

    default:
      write3Line("ERROR", "something wars going wrong", "please try again",
                 false, 2000);
      setState(aprs_controler, new StateConfigMenue());
      break;
  }
}

/* -------------------------------------------------------------------------- */
/* String Selector (Abstrakt String Config)   */
/* -------------------------------------------------------------------------- */

ConfigStringSelector::ConfigStringSelector(const char *head,
                                           const char *select_list) {
  // DDD("ConfigStringSelector::ConfigStringSelector");

  strncpy(_select_list, select_list, 40);
  _head = head;
  _tmp = cfg.call;
  int id = 1;
  _actual = static_cast<char>(_tmp.charAt(_pos));
  for (_select_list_idx = 0; _select_list_idx < 36; _select_list_idx++) {
    if (_actual == _select_list[_select_list_idx]) {
      break;
    }
  }
  call_config_timer =
      xTimerCreate("CallConfigTimer", pdMS_TO_TICKS(1000), pdTRUE,
                   reinterpret_cast<void *>(id), &kinoTimer_CB);
  if (call_config_timer == NULL) {
    Serial.println("Timer can not be created");
  }

  if (xTimerStart(call_config_timer, 0) == pdPASS) {
    Serial.printf("timer started, start vTaskStartScheduler\n");
  } else {
    Serial.printf("Problem Timer start\n");
  }
}

void ConfigStringSelector::singleClick(APRSControler &aprs_controler) {
  // DDD("ConfigStringSelector.singleClick");
  _pos++;
  if (_pos > 7) {
    _pos = 0;
  }
  _arrow_line = " " + _arrow_line;
  _actual = static_cast<char>(_tmp.charAt(_pos));
  for (_select_list_idx = 0; _select_list_idx < 37; _select_list_idx++) {
    if (_actual == _select_list[_select_list_idx]) {
      break;
    }
  }
  _count = false;
}

void ConfigStringSelector::kino(void) {
  Serial.printf("select_list: %s\n", _select_list);
  Serial.printf("_actual: %c\n", _actual);
  Serial.printf("_pos: %d\n", _pos);
  if (_count) {
    _select_list_idx++;
    if (_select_list_idx > 37) {
      _select_list_idx = 0;
    }
  }

  Serial.printf("select_list_idx=%d\n", _select_list_idx);
  _actual = _select_list[_select_list_idx];
  _tmp.setCharAt(_pos, _actual);

  Serial.printf("       _tmp: %s\n", _tmp.c_str());
  Serial.printf("_arrow_line: %s\n", _arrow_line.c_str());

  // _showText(_tmp.c_str(), _arrow_line.c_str());
  cfg.oled_message.head = _head;
  cfg.oled_message.line1 = _tmp;
  cfg.oled_message.line2 = _arrow_line;
  cfg.oled_message.line3 = _hint_line0;
  cfg.oled_message.line4 = _hint_line1;
  cfg.oled_message.active = true;

  _count = true;
}

void ConfigStringSelector::doubleClick(APRSControler &aprs_controler) {
  // DDD("ConfigStringSelector.doubleClick");
  xTimerStop(call_config_timer, 0);
  _tmp.trim();
  cfg.call = _tmp;
  setState(aprs_controler, new StateConfigMenue());
  Serial.printf("new call: %s\n", cfg.call.c_str());
  setPrefsString(PREFS_CALL, cfg.call);
}

void ConfigStringSelector::_showText(const char *line0, const char *line1) {
  write2Display("hallo", "Welt", "alles", "schick", "oder");
}

void ConfigStringSelector::longClick(APRSControler &aprs_controler) {
  xTimerStop(call_config_timer, 0);
  aprs_controler.display_change = true;
  aprs_controler.display_update = true;
  setState(aprs_controler, new StateDefault());
}

/* -------------------------------------------------------------------------- */
/*   Config Mode Selector   */
/* -------------------------------------------------------------------------- */

StateConfigWiFi::StateConfigWiFi(const char *head) {
  // DDD("StateConfigMenu.StateConfigMenu");
  _head = head;
  _show();
}

void StateConfigWiFi::singleClick(APRSControler &aprs_controler) {
  // DDD("StateConfigWiFi.singleClick");
  // DDD(String(cfg.current_wifi_mode));
  if (cfg.current_wifi_mode > 1) {
    cfg.current_wifi_mode = wifi_off;
  } else {
    uint8_t mode = cfg.current_wifi_mode;
    mode++;
    cfg.current_wifi_mode = static_cast<wifi_mode>(mode);
  }
  _show();
}

void StateConfigWiFi::doubleClick(APRSControler &aprs_controler) {
  // DDD("StateConfigWiFi.doubleClick");
  setState(aprs_controler, new StateConfigMenue());
  ESP.restart();
}

void StateConfigWiFi::_show(void) {
  char buf[12] = {0};
  strncat(buf, "  ", 12);
  strncat(buf, wifi_mode_txt[cfg.current_wifi_mode], 12);
  write3Line(_head, buf, "1clck nxt, 2clck ok", false, 0);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

StateConfigRun::StateConfigRun(const char *head) {
  // DDD("StateConfigMenu.StateConfigMenu");
  _head = head;
  _show();
}

void StateConfigRun::singleClick(APRSControler &aprs_controler) {
  // DDD("StateConfigRun.singleClick");
  // DDD(String(cfg.current_run_mode));
  if (cfg.current_run_mode > 4) {
    cfg.current_run_mode = mode_tracker;
  } else {
    uint8_t mode = cfg.current_run_mode;
    mode++;
    cfg.current_run_mode = static_cast<run_mode>(mode);
  }
  _show();
}

void StateConfigRun::doubleClick(APRSControler &aprs_controler) {
  // DDD("StateConfigRun.doubleClick");
  setState(aprs_controler, new StateConfigMenue());
  ESP.restart();
}

void StateConfigRun::_show(void) {
  char buf[12] = {0};
  snprintf(buf, sizeof(buf), " %s", run_mode_txt[cfg.current_run_mode]);
  write3Line(_head, buf, "1clck nxt, 2clck ok", false, 0);
}

/***************************************************************************/

// void StateConfigMenue::singleClick(APRSControler& aprs_controler) {
//   DDD("StateConfigMenue.singleClick");
// }

// void StateConfigMenue::doubleClick(APRSControler& aprs_controler) {
//   DDD("StateConfigMenue.doubleClick");
// }

/***************************************************************************/

// void StateConfigMenue::singleClick(APRSControler& aprs_controler) {
//   DDD("StateConfigMenue.singleClick");
// }

// void StateConfigMenue::doubleClick(APRSControler& aprs_controler) {
//   DDD("StateConfigMenue.doubleClick");
// }

/***************************************************************************/
