#ifndef ONE_BUTTON_HANDLER_H
#define ONE_BUTTON_HANDLER_H

#include <OneButton.h>

extern bool displayCange;

extern OneButton button;

void initOneButton();

void oneClick();
void doubleClick();
void longPressStart();
void longPressStop();
void longPress();


#endif
