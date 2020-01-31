#include <ButtonState.h>
#include <OneButtonHandler.h>
#include <TrackerDisplay.h>
#include <uxa_debug.h>

TimerHandle_t _tmr;



AbstracButtonState::~AbstracButtonState() {
  DDD("AbstracButtonState::~AbstracButtonState");
}

void AbstracButtonState::setState(APRSControler& aprs_controler,
                                  AbstracButtonState* state) {
  AbstracButtonState* aux = aprs_controler.button_state;
  aprs_controler.button_state = state;
  delete aux;
}

void AbstracButtonState::kino(void) {}

void AbstracButtonState::longClick(APRSControler& aprs_controler) {
  aprs_controler.display_change = true;
  aprs_controler.display_update = true;
  setState(aprs_controler, new ButtonNeutral());
}

/***************************************************************************/

void ButtonDisplayMode::singleClick(APRSControler& aprs_controler) {
  DDD("ButtonDisplayMode.singleClick");
  aprs_controler.nextDisplayMode();
  aprs_controler.next_display_time = 0;
}
void ButtonDisplayMode::doubleClick(APRSControler& aprs_controler) {
  DDD("ButtonDisplayMode.doubleClick");
  aprs_controler.display_change = true;
  aprs_controler.nextDisplayMode();
  aprs_controler.next_display_time = 0;
  setState(aprs_controler, new ButtonNeutral());
}

/***************************************************************************/

void ButtonNeutral::singleClick(APRSControler& aprs_controler) {
  DDD("ButtonNeutral.singleClick");
  aprs_controler.display_change = false;
  setState(aprs_controler, new ButtonDisplayMode());
}

void ButtonNeutral::doubleClick(APRSControler& aprs_controler) {
  DDD("ButtonNeutral.doubleClick");
}

void ButtonNeutral::longClick(APRSControler& aprs_controler) {
  DDD("ButtonNeutral.longClick()");
  aprs_controler.display_change = false;
  aprs_controler.display_update = false;
  setState(aprs_controler, new ButtonConfig());
  write3Line("Cnfg Mode", "single click for next", "double click to enter",
             false, 0);
}

/***************************************************************************/

void ButtonConfigCall::singleClick(APRSControler& aprs_controler) {
  DDD("ButtonConfigCall.singleClick");
}

void ButtonConfigCall::doubleClick(APRSControler& aprs_controler) {
  DDD("ButtonConfigCall.doubleClick");
  write3Line("Cnfg Call", "not implemented yet", "I'm so sad!", false, 0);
}

/***************************************************************************/

void ButtonConfig::singleClick(APRSControler& aprs_controler) {
  DDD("ButtonConfig.singleClick");
  uint8_t max_items = 2;
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
      write3Line("Cnfg Mode", "configure run mode", "double click to enter",
                 false, 0);
      break;
    default:
      write3Line("ERROR", "something wars going wrong", "please try again",
                 false, 2000);
      setState(aprs_controler, new ButtonConfig());
      break;
  }
}

void ButtonConfig::doubleClick(APRSControler& aprs_controler) {
  DDD("ButtonConfig.doubleClick");
  const char select_list_call[40] = {
      ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
      'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y',
      'Z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

  switch (button_config_mode) {
    case 0:
      setState(aprs_controler, new ButtonConfigSelector(
                                   "Cfg Call", select_list_call, &reg.call));
      break;
    case 1:
      setState(aprs_controler, new ButtonConfigMode());
      write3Line("Cfg Mode", "geht auch noch nicht", "Schade, so schade", false,
                 0);
      break;

    default:
      write3Line("ERROR", "something wars going wrong", "please try again",
                 false, 2000);
      setState(aprs_controler, new ButtonConfig());
      break;
  }
}

/***************************************************************************/

void ButtonConfigMode::singleClick(APRSControler& aprs_controler) {
  DDD("ButtonConfigMode.singleClick");
  write3Line("Cnfg Mode", "not implemented yet", "I'm so sad!", false, 0);
}

void ButtonConfigMode::doubleClick(APRSControler& aprs_controler) {
  DDD("ButtonConfigMode.doubleClick");
  write3Line("Cnfg Mode", "not implemented yet", "I'm so sad!", false, 0);
}

/***************************************************************************/

ButtonConfigSelector::ButtonConfigSelector(char* head, const char* select_list,
                                           String* toChange) {
  DDD("ButtonConfigSelector::ButtonConfigSelector");

  strncpy(_select_list, select_list, 40);
  _toChange = toChange;
  _tmp = *toChange;
  _head = head;
  int id = 1;
  _actual = (char)_tmp.charAt(_pos);
  for (_select_list_idx = 0; _select_list_idx < 36; _select_list_idx++) {
    if (_actual == _select_list[_select_list_idx]) {
      break;
    }
  }
  _tmr = xTimerCreate("MyTimer", pdMS_TO_TICKS(1000), pdTRUE, (void*)id,
                      &kinoTimer_CB);
  if (_tmr == NULL) {
    Serial.println("Timer can not be created");
  }

  if (xTimerStart(_tmr, 0) == pdPASS) {
    Serial.printf("timer started, start vTaskStartScheduler\n");
  } else {
    Serial.printf("Problem Timer start\n");
  }
}

void ButtonConfigSelector::singleClick(APRSControler& aprs_controler) {
  DDD("ButtonConfigSelector.singleClick");
  _pos++;
  if (_pos > 7) {
    _pos = 0;
  }
  _arrow_line = " " + _arrow_line;
  _actual = (char)_tmp.charAt(_pos);
  for (_select_list_idx = 0; _select_list_idx < 37; _select_list_idx++) {
    if (_actual == _select_list[_select_list_idx]) {
      break;
    }
  }
  _count = false;
}

void ButtonConfigSelector::kino(void) {
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

  //_showText(_tmp.c_str(), _arrow_line.c_str());
  reg.oled_message.head = _head;
  reg.oled_message.line1 = _tmp;
  reg.oled_message.line2 = _arrow_line;
  reg.oled_message.line3 = _hint_line0;
  reg.oled_message.line4 = _hint_line1;
  reg.oled_message.active = true;

  _count = true;
}

void ButtonConfigSelector::doubleClick(APRSControler& aprs_controler) {
  DDD("ButtonConfigSelector.doubleClick");
  xTimerStop( _tmr, 0 );
  _tmp.trim();
  _toChange = &_tmp;
  setState(aprs_controler, new ButtonConfig());
  Serial.printf("new call: %s\n", reg.call.c_str());
  // write to nvs
}

void ButtonConfigSelector::_showText(const char* line0, const char* line1) {
  write2Display("hallo", "Welt", "alles", "schick", "oder");
}

void ButtonConfigSelector::longClick(APRSControler& aprs_controler) {
  xTimerStop( _tmr, 0 );
  aprs_controler.display_change = true;
  aprs_controler.display_update = true;
  setState(aprs_controler, new ButtonNeutral());
}


/***************************************************************************/

// void ButtonConfig::singleClick(APRSControler& aprs_controler) {
//   DDD("ButtonConfig.singleClick");
// }

// void ButtonConfig::doubleClick(APRSControler& aprs_controler) {
//   DDD("ButtonConfig.doubleClick");
// }

/***************************************************************************/

// void ButtonConfig::singleClick(APRSControler& aprs_controler) {
//   DDD("ButtonConfig.singleClick");
// }

// void ButtonConfig::doubleClick(APRSControler& aprs_controler) {
//   DDD("ButtonConfig.doubleClick");
// }

/***************************************************************************/

// void ButtonConfig::singleClick(APRSControler& aprs_controler) {
//   DDD("ButtonConfig.singleClick");
// }

// void ButtonConfig::doubleClick(APRSControler& aprs_controler) {
//   DDD("ButtonConfig.doubleClick");
// }

/***************************************************************************/


// void initOneButton() {
//   Serial.println("Init OneButton");
// }

// void oneClick() {
//   Serial.println("one Click");
//   if (!maincontroler.config_mode) {
//     if (maincontroler.display_change == false) {
//       DDD("set nextDisplayMode()");
//       maincontroler.nextDisplayMode();
//       maincontroler.next_display_time = 0;
//     }
//     if (maincontroler.display_change == true) {
//       DDD("set display_change = false");
//       maincontroler.display_change = false;
//     }
//   }
// }


void doubleClick() {
  Serial.println("double Click");
  if (!maincontroler.config_mode) {
    if (maincontroler.display_change == false) {
      maincontroler.display_change = true;
    }
  }
}
void longPressStart() { Serial.println("longPressStart"); }
void longPressStop() { Serial.println("longPressStop"); }
void longPress() { Serial.println("longPress"); }
