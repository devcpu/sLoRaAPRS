#include <APRSControler.h>
#include <uxa_debug.h>

extern APRSControler maincontroler;

DisplayMode APRSControler::getCurrentDisplayMode(void) {
  return (DisplayMode)display_mode;
};
DisplayMode APRSControler::getNextDisplayMode(void) {
  nextDisplayMode();
  return (DisplayMode)display_mode;
};

void APRSControler::nextDisplayMode(void) {
  uint8_t max = displayModeEND - 1;
  if (display_mode < max) {
    display_mode++;
  } else {
    display_mode = displayModeUTC;
  }
};

// APRSControler::APRSControler() {
// }

void APRSControler::singleClick() {
  button_state->singleClick(*this);
};

void APRSControler::doubleClick(void) {
  button_state->doubleClick(*this);
};

void APRSControler::longClick(void) {
  button_state->longClick(*this);
};



void singleClick_CB(void){
  maincontroler.singleClick();
}

void doubleClick_CB(void){
  
  maincontroler.doubleClick();
}

void longClick_CB(void){
  maincontroler.longClick();
}

void kinoTimer_CB(TimerHandle_t xExpiredTimer){
  Serial.printf("\ncall of kinoTimer_CB\n");
  maincontroler.button_state->kino();
}
