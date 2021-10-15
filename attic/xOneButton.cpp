/**
 * @file xOneButton.cpp
 *
 * @brief Library for detecting button clicks, doubleclicks and long press
 * pattern on a single button.
 *
 * @author Matthias Hertel, https://www.mathertel.de
 * @Copyright Copyright (c) by Matthias Hertel, https://www.mathertel.de.
 *
 * This work is licensed under a BSD style license. See
 * http://www.mathertel.de/License.aspx
 *
 * More information on: https://www.mathertel.de/Arduino/OneButtonLibrary.aspx
 *
 * Changelog: see xOneButton.h
 */

#include "xOneButton.h"

// ----- Initialization and Default Values -----

/**
 * @brief Construct a new xOneButton object but not (yet) initialize the IO pin.
 */
xOneButton::xOneButton() {
  _pin = -1;
  // further initialization has moved to xOneButton.h
}

/**
 * Initialize the xOneButton library.
 * @param pin The pin to be used for input from a momentary button.
 * @param activeLow Set to true when the input level is LOW when the button is
 * pressed, Default is true.
 * @param pullupActive Activate the internal pullup when available. Default is
 * true.
 */
xOneButton::xOneButton(int pin, boolean activeLow, bool pullupActive) {
  // xOneButton();
  _pin = pin;

  if (activeLow) {
    // the button connects the input pin to GND when pressed.
    _buttonPressed = LOW;

  } else {
    // the button connects the input pin to VCC when pressed.
    _buttonPressed = HIGH;
  } // if

  if (pullupActive) {
    // use the given pin as input and activate internal PULLUP resistor.
    pinMode(pin, INPUT_PULLUP);
  } else {
    // use the given pin as input
    pinMode(pin, INPUT);
  } // if
} // xOneButton

// explicitly set the number of millisec that have to pass by before a click is
// assumed as safe.
void xOneButton::setDebounceTicks(int ticks) { _debounceTicks = ticks; } // setDebounceTicks

// explicitly set the number of millisec that have to pass by before a click is
// detected.
void xOneButton::setClickTicks(int ticks) { _clickTicks = ticks; } // setClickTicks

// explicitly set the number of millisec that have to pass by before a long
// button press is detected.
void xOneButton::setPressTicks(int ticks) { _pressTicks = ticks; } // setPressTicks

// save function for click event
void xOneButton::attachClick(callbackFunction newFunction) { _clickFunc = newFunction; } // attachClick

// save function for parameterized click event
void xOneButton::attachClick(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramClickFunc = newFunction;
  _clickFuncParam = parameter;
} // attachClick

// save function for doubleClick event
void xOneButton::attachDoubleClick(callbackFunction newFunction) {
  _doubleClickFunc = newFunction;
} // attachDoubleClick

// save function for parameterized doubleClick event
void xOneButton::attachDoubleClick(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramDoubleClickFunc = newFunction;
  _doubleClickFuncParam = parameter;
} // attachDoubleClick

// save function for press event
// DEPRECATED, is replaced by attachLongPressStart, attachLongPressStop,
// attachDuringLongPress,
void xOneButton::attachPress(callbackFunction newFunction) { _pressFunc = newFunction; } // attachPress

void xOneButton::attachPressStart(callbackFunction newFunction) { _pressStartFunc = newFunction; } // attachPressStart

// save function for longPressStart event
void xOneButton::attachLongPressStart(callbackFunction newFunction) {
  _longPressStartFunc = newFunction;
} // attachLongPressStart

// save function for parameterized longPressStart event
void xOneButton::attachLongPressStart(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramLongPressStartFunc = newFunction;
  _longPressStartFuncParam = parameter;
} // attachLongPressStart

// save function for longPressStop event
void xOneButton::attachLongPressStop(callbackFunction newFunction) {
  _longPressStopFunc = newFunction;
} // attachLongPressStop

// save function for parameterized longPressStop event
void xOneButton::attachLongPressStop(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramLongPressStopFunc = newFunction;
  _longPressStopFuncParam = parameter;
} // attachLongPressStop

// save function for during longPress event
void xOneButton::attachDuringLongPress(callbackFunction newFunction) {
  _duringLongPressFunc = newFunction;
} // attachDuringLongPress

// save function for parameterized during longPress event
void xOneButton::attachDuringLongPress(parameterizedCallbackFunction newFunction, void *parameter) {
  _paramDuringLongPressFunc = newFunction;
  _duringLongPressFuncParam = parameter;
} // attachDuringLongPress

// function to get the current long pressed state
bool xOneButton::isLongPressed() { return _isLongPressed; }

int xOneButton::getPressedTicks() { return _stopTime - _startTime; }

void xOneButton::reset(void) {
  _state = 0; // restart.
  _startTime = 0;
  _stopTime = 0;
  _isLongPressed = false;
}

/**
 * @brief Check input of the configured pin and then advance the finite state
 * machine (FSM).
 */
void xOneButton::tick(void) {
  if (_pin >= 0) {
    while (1) {
      ticker(digitalRead(_pin) == _buttonPressed);
      vTaskDelay(1 / portTICK_PERIOD_MS);
    }
  }
}

/**
 * @brief Advance the finite state machine (FSM) using the given level.
 */
void xOneButton::ticker(bool activeLevel) {
  uint64_t now = xTaskGetTickCount(); // current (relative) time in msecs.

  // Implementation of the state machine

  if (_state == 0) { // waiting for menu pin being pressed.
    if (activeLevel) {
      _state = 1;       // step to state 1
      _startTime = now; // remember starting time
    }                   // if

  } else if (_state == 1) { // waiting for menu pin being released.

    if ((!activeLevel) && ((uint64_t)(now - _startTime) < _debounceTicks)) {
      // button was released to quickly so I assume some debouncing.
      // go back to state 0 without calling a function.
      _state = 0;

    } else if (!activeLevel) {
      _state = 2;      // step to state 2
      _stopTime = now; // remember stopping time

    } else if ((activeLevel) && ((uint64_t)(now - _startTime) > _pressTicks)) {
      _stopTime = now;       // remember stopping time
      _isLongPressed = true; // Keep track of long press state
      if (_pressFunc)
        _pressFunc();
      if (_longPressStartFunc)
        _longPressStartFunc();
      if (_paramLongPressStartFunc)
        _paramLongPressStartFunc(_longPressStartFuncParam);
      if (_duringLongPressFunc)
        _duringLongPressFunc();
      if (_paramDuringLongPressFunc)
        _paramDuringLongPressFunc(_duringLongPressFuncParam);
      _state = 6; // step to state 6
    } else {
      // Button was pressed down. wait. Stay in this state.
      // if a pressStart event is registered, call it:
      if (_pressStartFunc)
        _pressStartFunc();
    } // if

  } else if (_state == 2) {
    // waiting for menu pin being pressed the second time or timeout.
    if ((_doubleClickFunc == NULL && _paramDoubleClickFunc == NULL) || (uint64_t)(now - _startTime) > _clickTicks) {
      // this was only a single short click
      if (_clickFunc)
        _clickFunc();
      if (_paramClickFunc)
        _paramClickFunc(_clickFuncParam);
      _state = 0; // restart.

    } else if ((activeLevel) && ((uint64_t)(now - _stopTime) > _debounceTicks)) {
      _state = 3;       // step to state 3
      _startTime = now; // remember starting time
    }                   // if

  } else if (_state == 3) { // waiting for menu pin being released finally.
    // Stay here for at least _debounceTicks because else we might end up in
    // state 1 if the button bounces for too long.
    if ((!activeLevel) && ((uint64_t)(now - _startTime) > _debounceTicks)) {
      _stopTime = now; // remember stopping time
      // this was a 2 click sequence.
      if (_doubleClickFunc)
        _doubleClickFunc();
      if (_paramDoubleClickFunc)
        _paramDoubleClickFunc(_doubleClickFuncParam);
      _state = 0; // restart.
    }             // if

  } else if (_state == 6) {
    // waiting for menu pin being release after long press.
    if (!activeLevel) {
      _isLongPressed = false; // Keep track of long press state
      _stopTime = now;        // remember stopping time
      if (_longPressStopFunc)
        _longPressStopFunc();
      if (_paramLongPressStopFunc)
        _paramLongPressStopFunc(_longPressStopFuncParam);
      _state = 0; // restart.
    } else {
      // button is being long pressed
      _stopTime = now;       // remember stopping time
      _isLongPressed = true; // Keep track of long press state
      if (_duringLongPressFunc)
        _duringLongPressFunc();
      if (_paramDuringLongPressFunc)
        _paramDuringLongPressFunc(_duringLongPressFuncParam);
    } // if
  }   // if
} // xOneButton.tick()

// end.
