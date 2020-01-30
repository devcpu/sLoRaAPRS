

#include <OneButtonHandler.h>
#include <Registry.h>
#include <uxa_debug.h>
#include <APRSControler.h>


extern Registry reg;
extern APRSControler maincontroler;



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
  if (!maincontroler.config_mode) {
    if (maincontroler.display_change == false) {
      DDD("set nextDisplayMode()");
      maincontroler.nextDisplayMode();
      maincontroler.next_display_time = 0;
    }
    if (maincontroler.display_change == true) {
      DDD("set display_change = false");
      maincontroler.display_change = false;
    }
  }
}


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
