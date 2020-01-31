#pragma once
#include <APRSControler.h>
#include <Arduino.h>
#include <uxa_debug.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

class APRSControler;

class AbstracButtonState {
 public:
  virtual ~AbstracButtonState();

  virtual void singleClick(APRSControler& aprs_controler) = 0;
  virtual void doubleClick(APRSControler& aprs_controler) = 0;
  virtual void longClick(APRSControler& aprs_controler);
  virtual void kino();

 protected:
  void setState(APRSControler& aprs_controler, AbstracButtonState* st);
};

class ButtonDisplayMode : public AbstracButtonState {
 public:
  virtual void singleClick(APRSControler& aprs_controler);
  virtual void doubleClick(APRSControler& aprs_controler);
};

class ButtonNeutral : public AbstracButtonState {
 public:
  virtual void singleClick(APRSControler& aprs_controler);
  virtual void doubleClick(APRSControler& aprs_controler);
  virtual void longClick(APRSControler& aprs_controler);
};

class ButtonConfigCall : public AbstracButtonState {
 public:
  virtual void singleClick(APRSControler& aprs_controler);
  virtual void doubleClick(APRSControler& aprs_controler);
};

class ButtonConfigMode : public AbstracButtonState {
 public:
  virtual void singleClick(APRSControler& aprs_controler);
  virtual void doubleClick(APRSControler& aprs_controler);
};

class ButtonConfig : public AbstracButtonState {
 public:
  virtual void singleClick(APRSControler& aprs_controler);
  virtual void doubleClick(APRSControler& aprs_controler);

 private:
  int8_t button_config_mode = -1;
};

class ButtonConfigSelector : public AbstracButtonState {
 public:
  ButtonConfigSelector(char* head,  const char* select_list, String* toChange);
  virtual void singleClick(APRSControler&);
  virtual void doubleClick(APRSControler&);
  void kino(void);

 private:
  char _select_list[40];
  int8_t _select_list_idx = -1;
  String* _toChange;
  char _actual = '0';
  uint8_t _pos = 0;
  String _tmp = "";
  const char* _hint_line0 = "1click: select";
  const char* _hint_line1 = "2click takes";
  uint16_t _wait = 2000;
  char* _head;
  String _arrow_line = "^";
  boolean _count = false;

  void _showText(const char* line0, const char* line1);
  void _xdelay(uint16_t w);


};

// class  : public AbstracButtonState {
//     public:
//     virtual void singleClick(APRSControler&);
//     virtual void doubleClick(APRSControler&);

// };

// class  : public AbstracButtonState {
//     public:
//     virtual void singleClick(APRSControler&);
//     virtual void doubleClick(APRSControler&);

// };
