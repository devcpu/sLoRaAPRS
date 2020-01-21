

#include <OneButtonHandler.h>
#include <Registry.h>
#include <uxa_debug.h>
#include <APRSControler.h>

extern Registry reg;
extern APRSControler controler;



void initOneButton() {
  Serial.println("Init OneButton");
  // button.attachClick(oneClick);
  button.attachClick(singleClick_CB);
  // button.attachDoubleClick(doubleClick);
  button.attachDoubleClick(doubleClick_CB);
  button.attachLongPressStart(longPressStart);
  // button.attachLongPressStop(longPressStop);
  button.attachLongPressStop(longClick_CB);
  // button.attachDuringLongPress(longPress);
}

void oneClick() {
  Serial.println("one Click");
  if (!reg.controler.config_mode) {
    if (reg.controler.display_change == false) {
      DDD("set nextDisplayMode()");
      reg.controler.nextDisplayMode();
      reg.controler.next_display_time = 0;
    }
    if (reg.controler.display_change == true) {
      DDD("set display_change = false");
      reg.controler.display_change = false;
    }
  }
}


void doubleClick() {
  Serial.println("double Click");
  if (!reg.controler.config_mode) {
    if (reg.controler.display_change == false) {
      reg.controler.display_change = true;
    }
  }
}
void longPressStart() { Serial.println("longPressStart"); }
void longPressStop() { Serial.println("longPressStop"); }
void longPress() { Serial.println("longPress"); }
