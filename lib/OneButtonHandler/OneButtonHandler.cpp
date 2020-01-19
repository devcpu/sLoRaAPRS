

#include <OneButtonHandler.h>


void initOneButton() {
    Serial.println("Init OneButton");
    button.attachClick(oneClick);
    button.attachDoubleClick(doubleClick);
    button.attachLongPressStart(longPressStart);
    button.attachLongPressStop(longPressStop);
    button.attachDuringLongPress(longPress);

}


void oneClick(){
  Serial.println("one Click");
  displayChange = !displayChange;
}
void doubleClick(){
  Serial.println("double Click");
}
void longPressStart(){
  Serial.println("longPressStart");
}
void longPressStop(){
  Serial.println("longPressStop");
}
void longPress(){
  Serial.println("longPress");
}
