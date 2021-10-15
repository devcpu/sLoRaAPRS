/*
 * File: ButtonState.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-15 23:58
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <Arduino.h>
#include <ButtonState.h>
#include <Config.h>
#include <TrackerDisplay.h>
#include <apptypes.h>
#include <OneButton.h>

char wifi_mode_txt[3][16] = {"OFF", "AP", "STA"};
char run_mode_txt[6][16] = {"Tracker", "WXTracker", "WX Fix", "Digi", "Gateway", "GW-Digi"};

TimerHandle_t call_config_timer;
extern Config cfg;

AbstracButtonState::~AbstracButtonState() { ESP_LOGD(TAG, "AbstracButtonState::~AbstracButtonState"); }

void AbstracButtonState::setState(ButtonHandler &button_handler, AbstracButtonState *state) {
  AbstracButtonState *aux = button_handler.button_state;
  button_handler.button_state = state;
  delete aux;
}

void AbstracButtonState::kino(void) {}

void AbstracButtonState::longClick(ButtonHandler &button_handler) {
  button_handler.display_change = true;
  button_handler.display_update = true;
  setState(button_handler, new StateDefault());
}

/* -------------------------------------------------------------------------- */
/*   Display Mode   */
/* -------------------------------------------------------------------------- */

void StateDisplayMode::singleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateDisplayMode.singleClick");
  button_handler.nextDisplayMode();
  button_handler.next_display_time = 0;
}
void StateDisplayMode::doubleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateDisplayMode.doubleClick");
  button_handler.display_change = true;
  button_handler.nextDisplayMode();
  button_handler.next_display_time = 0;
  setState(button_handler, new StateDefault());
}

/* -------------------------------------------------------------------------- */
/*   Default   */
/* -------------------------------------------------------------------------- */

void StateDefault::singleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateDefault.singleClick");
  button_handler.display_change = false;
  setState(button_handler, new StateDisplayMode());
}

void StateDefault::doubleClick(ButtonHandler &button_handler) { ESP_LOGD(TAG, "StateDefault.doubleClick"); }

void StateDefault::longClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateDefault.longClick()");
  button_handler.display_change = false;
  button_handler.display_update = false;
  setState(button_handler, new StateConfigMenue());
}

/* -------------------------------------------------------------------------- */
/*   Config Menue   */
/* -------------------------------------------------------------------------- */

StateConfigMenue::StateConfigMenue(void) {
  ESP_LOGD(TAG, "StateConfigMenue::StateConfigMenue");
  // vTaskDelay(pdMS_TO_TICKS(500));
  td.write3Line("Cnfg Mode", "1clck nxt, 2clck ", "long click exit", false, 0);
}

void StateConfigMenue::singleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateConfigMenue.singleClick");
  uint8_t max_items = 3; // count menue items
  if (button_config_mode < max_items - 1) {
    button_config_mode++;
  } else {
    button_config_mode = 0;
  }
  switch (button_config_mode) {
  case 0:
    td.write3Line("Cnfg Call", "configure call", "double click to enter", false, 0);
    break;
  case 1:
    td.write3Line("Cfg WiFi", "configure WiFi mode", "double click to enter", false, 0);
    break;
  case 2:
    td.write3Line("Cfg Run", "configure run mode", "double click to enter", false, 0);
    break;
  default:
    td.write3Line("ERROR", "something wars going wrong", "please try again", false, 2000);
    setState(button_handler, new StateConfigMenue());
    break;
  }
  ESP_LOGD(TAG, "got %d config", button_config_mode);
}

void StateConfigMenue::doubleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateConfigMenue.doubleClick");
  const char select_list_call[40] = {' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
                                     'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
                                     'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

  switch (button_config_mode) {
  case 0:
    setState(button_handler, new ConfigStringSelector("Cfg Call", select_list_call));
    break;
  case 1:
    // setState(button_handler, new StateConfigWiFi("Cfg WiFi"));
    break;

  case 2:
    // setState(button_handler, new StateConfigRun("Cfg Run"));
    break;

  default:
    td.write3Line("ERROR", "something wars going wrong", "please try again", false, 2000);
    // setState(button_handler, new StateConfigMenue());
    break;
  }
}

/* -------------------------------------------------------------------------- */
/* String Selector (Abstrakt String Config)   */
/* -------------------------------------------------------------------------- */

ConfigStringSelector::ConfigStringSelector(const char *head, const char *select_list) {
  ESP_LOGD(TAG, "ConfigStringSelector::ConfigStringSelector");

  strncpy(_select_list, select_list, sizeof(_select_list) - 1);
  _head = head;
  _tmp = cfg.call;
  int id = 1;
  _actual = static_cast<char>(_tmp.charAt(_pos));
  for (_select_list_idx = 0; _select_list_idx < 36; _select_list_idx++) {
    if (_actual == _select_list[_select_list_idx]) {
      break;
    }
  }
  // call_config_timer =
  //     xTimerCreate("CallConfigTimer", pdMS_TO_TICKS(1000), pdTRUE,
  //                  reinterpret_cast<void *>(id), &kinoTimer_CB);
  // if (call_config_timer == NULL) {
  //   Serial.println("Timer can not be created");
  // }

  // if (xTimerStart(call_config_timer, 0) == pdPASS) {
  //   Serial.printf("timer started, start vTaskStartButtonHandler\n");
  // } else {
  //   Serial.printf("Problem Timer start\n");
  // }
}

void ConfigStringSelector::singleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "ConfigStringSelector.singleClick");
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

void ConfigStringSelector::doubleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "ConfigStringSelector.doubleClick");
  xTimerStop(call_config_timer, 0);
  _tmp.trim();
  cfg.call = _tmp;
  setState(button_handler, new StateConfigMenue());
  Serial.printf("new call: %s\n", cfg.call.c_str());
  setPrefsString(PREFS_CALL, cfg.call);
}

void ConfigStringSelector::_showText(const char *line0, const char *line1) {
  td.write2Display("hallo", "Welt", "alles", "schick", "oder");
}

void ConfigStringSelector::longClick(ButtonHandler &button_handler) {
  xTimerStop(call_config_timer, 0);
  button_handler.display_change = true;
  button_handler.display_update = true;
  setState(button_handler, new StateDefault());
}

/* -------------------------------------------------------------------------- */
/*   Config Mode Selector   */
/* -------------------------------------------------------------------------- */

StateConfigWiFi::StateConfigWiFi(const char *head) {
  ESP_LOGD(TAG, "StateConfigMenu.StateConfigMenu");
  _head = head;
  _show();
}

void StateConfigWiFi::singleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateConfigWiFi.singleClick");
  ESP_LOGD(TAG, "%s", cfg.current_wifi_mode);
  if (cfg.current_wifi_mode > 1) {
    cfg.current_wifi_mode = wifi_off;
  } else {
    uint8_t mode = cfg.current_wifi_mode;
    mode++;
    cfg.current_wifi_mode = static_cast<wifi_mode>(mode);
  }
  _show();
}

void StateConfigWiFi::doubleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateConfigWiFi.doubleClick");
  setState(button_handler, new StateConfigMenue());
  ESP.restart();
}

void StateConfigWiFi::_show(void) {
  char buf[12] = {0};
  strncat(buf, "  ", sizeof(buf) - 1);
  strncat(buf, wifi_mode_txt[cfg.current_wifi_mode], sizeof(buf) - 1);
  td.write3Line(_head, buf, "1clck nxt, 2clck ok", false, 0);
}

/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

StateConfigRun::StateConfigRun(const char *head) {
  ESP_LOGD(TAG, "StateConfigMenu.StateConfigMenu");
  _head = head;
  _show();
}

void StateConfigRun::singleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateConfigRun.singleClick");
  ESP_LOGD(TAG, "%s", cfg.current_run_mode);
  if (cfg.current_run_mode > 4) {
    cfg.current_run_mode = mode_tracker;
  } else {
    uint8_t mode = cfg.current_run_mode;
    mode++;
    cfg.current_run_mode = static_cast<run_mode>(mode);
  }
  _show();
}

void StateConfigRun::doubleClick(ButtonHandler &button_handler) {
  ESP_LOGD(TAG, "StateConfigRun.doubleClick");
  setState(button_handler, new StateConfigMenue());
  ESP.restart();
}

void StateConfigRun::_show(void) {
  char buf[12] = {0};
  snprintf(buf, sizeof(buf), " %s", run_mode_txt[cfg.current_run_mode]);
  td.write3Line(_head, buf, "1clck nxt, 2clck ok", false, 0);
}

/***************************************************************************/

// void StateConfigMenue::singleClick(ButtonHandler& button_handler) {
//   ESP_LOGD(TAG, "StateConfigMenue.singleClick");
// }

// void StateConfigMenue::doubleClick(ButtonHandler& button_handler) {
//   ESP_LOGD(TAG, "StateConfigMenue.doubleClick");
// }

/***************************************************************************/

// void StateConfigMenue::singleClick(ButtonHandler& button_handler) {
//   ESP_LOGD(TAG, "StateConfigMenue.singleClick");
// }

// void StateConfigMenue::doubleClick(ButtonHandler& button_handler) {
//   ESP_LOGD(TAG, "StateConfigMenue.doubleClick");
// }

/***************************************************************************/
