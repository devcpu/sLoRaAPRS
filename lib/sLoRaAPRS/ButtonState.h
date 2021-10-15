/*
 * File: ButtonState.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-16 0:03
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_BUTTONSTATE_H_
#define LIB_SLORAAPRS_BUTTONSTATE_H_

#include <Arduino.h>
#include <apptypes.h>
#include <ButtonHandler.h>

// https://stackoverflow.com/questions/14676709/c-code-for-state-machine
// https://www.philipphauer.de/study/se/design-pattern/state.php

class ButtonHandler;

class AbstracButtonState {
 public:
  virtual ~AbstracButtonState();

  virtual void singleClick(ButtonHandler &button_handler) = 0;
  virtual void doubleClick(ButtonHandler &button_handler) = 0;
  virtual void longClick(ButtonHandler &button_handler);
  virtual void kino();

 protected:
  void setState(ButtonHandler &button_handler, AbstracButtonState *st);
};

class StateDisplayMode : public AbstracButtonState {
 public:
  virtual void singleClick(ButtonHandler &button_handler);
  virtual void doubleClick(ButtonHandler &button_handler);
};

class StateDefault : public AbstracButtonState {
 public:
  virtual void singleClick(ButtonHandler &button_handler);
  virtual void doubleClick(ButtonHandler &button_handler);
  virtual void longClick(ButtonHandler &button_handler);
};

class StateConfigCall : public AbstracButtonState {
 public:
  virtual void singleClick(ButtonHandler &button_handler);
  virtual void doubleClick(ButtonHandler &button_handler);
};

class StateConfigMenue : public AbstracButtonState {
 public:
  StateConfigMenue(void);
  virtual void singleClick(ButtonHandler &button_handler);
  virtual void doubleClick(ButtonHandler &button_handler);

 private:
  /**
   * @var		mixed	-1 switch for config menue
   */
  int8_t button_config_mode = -1;
};

class ConfigStringSelector : public AbstracButtonState {
 public:
  ConfigStringSelector(const char *head, const char *select_list);
  virtual void singleClick(ButtonHandler &button_handler);
  virtual void doubleClick(ButtonHandler &button_handler);
  virtual void longClick(ButtonHandler &button_handler);
  void kino(void);

 private:
  char _select_list[40];
  int8_t _select_list_idx = -1;
  char _actual = '0';
  String _tmp;
  uint8_t _pos = 0;
  const char *_hint_line0 = "1click: select";
  const char *_hint_line1 = "2click takes";
  uint16_t _wait = 2000;
  const char *_head;
  String _arrow_line = "^";
  boolean _count = false;

  void _showText(const char *line0, const char *line1);
  void _xdelay(uint16_t w);
};

class StateConfigWiFi : public AbstracButtonState {
 public:
  explicit StateConfigWiFi(const char *head);
  virtual void singleClick(ButtonHandler &button_handler);
  virtual void doubleClick(ButtonHandler &button_handler);

 private:
  const char *_head;
  void _show(void);
};

class StateConfigRun : public AbstracButtonState {
 public:
  explicit StateConfigRun(const char *head);
  virtual void singleClick(ButtonHandler &button_handler);
  virtual void doubleClick(ButtonHandler &button_handler);

 private:
  const char *_head;
  void _show(void);
};

// class  : public AbstracButtonState {
//     public:
//     virtual void singleClick(ButtonHandler&);
//     virtual void doubleClick(ButtonHandler&);

// };

// class  : public AbstracButtonState {
//     public:
//     virtual void singleClick(ButtonHandler&);
//     virtual void doubleClick(ButtonHandler&);

// };

#endif // LIB_SLORAAPRS_BUTTONSTATE_H_
