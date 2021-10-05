/*
 * File: ButtonState.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-29 0:02
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_BUTTONSTATE_H_
#define LIB_SLORAAPRS_BUTTONSTATE_H_

#include <Arduino.h>
#include <Scheduler.h>
#include <apptypes.h>

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

// https://stackoverflow.com/questions/14676709/c-code-for-state-machine
// https://www.philipphauer.de/study/se/design-pattern/state.php

class Scheduler;

class AbstracButtonState {
 public:
  virtual ~AbstracButtonState();

  virtual void singleClick(Scheduler &taskScheduler) = 0;
  virtual void doubleClick(Scheduler &taskScheduler) = 0;
  virtual void longClick(Scheduler &taskScheduler);
  virtual void kino();

 protected:
  void setState(Scheduler &taskScheduler, AbstracButtonState *st);
};

class StateDisplayMode : public AbstracButtonState {
 public:
  virtual void singleClick(Scheduler &taskScheduler);
  virtual void doubleClick(Scheduler &taskScheduler);
};

class StateDefault : public AbstracButtonState {
 public:
  virtual void singleClick(Scheduler &taskScheduler);
  virtual void doubleClick(Scheduler &taskScheduler);
  virtual void longClick(Scheduler &taskScheduler);
};

class StateConfigCall : public AbstracButtonState {
 public:
  virtual void singleClick(Scheduler &taskScheduler);
  virtual void doubleClick(Scheduler &taskScheduler);
};

class StateConfigMenue : public AbstracButtonState {
 public:
  StateConfigMenue(void);
  virtual void singleClick(Scheduler &taskScheduler);
  virtual void doubleClick(Scheduler &taskScheduler);

 private:
  /**
   * @var		mixed	-1 switch for config menue
   */
  int8_t button_config_mode = -1;
};

class ConfigStringSelector : public AbstracButtonState {
 public:
  ConfigStringSelector(const char *head, const char *select_list);
  virtual void singleClick(Scheduler &taskScheduler);
  virtual void doubleClick(Scheduler &taskScheduler);
  virtual void longClick(Scheduler &taskScheduler);
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
  virtual void singleClick(Scheduler &taskScheduler);
  virtual void doubleClick(Scheduler &taskScheduler);

 private:
  const char *_head;
  void _show(void);
};

class StateConfigRun : public AbstracButtonState {
 public:
  explicit StateConfigRun(const char *head);
  virtual void singleClick(Scheduler &taskScheduler);
  virtual void doubleClick(Scheduler &taskScheduler);

 private:
  const char *_head;
  void _show(void);
};

// class  : public AbstracButtonState {
//     public:
//     virtual void singleClick(Scheduler&);
//     virtual void doubleClick(Scheduler&);

// };

// class  : public AbstracButtonState {
//     public:
//     virtual void singleClick(Scheduler&);
//     virtual void doubleClick(Scheduler&);

// };

#endif // LIB_SLORAAPRS_BUTTONSTATE_H_
