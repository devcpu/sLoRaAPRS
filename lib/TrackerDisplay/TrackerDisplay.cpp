#include <TrackerDisplay.h>
#include <uxa_debug.h>

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

enum  {displayModeUTC, displayModeGPS, displayModeEND};

/**
 * @brief if true text on display will changed continous
 * 
 */
bool displayCange = true;

/**
 * @brief time until nex display info
 * 
 */
uint16_t tracker_display_time = 3000;


bool initDisplay(void){
  Serial.println("Init Display SSD1306!");
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
     Serial.println("Dsiplay SSD1306 on 0x3c not ready!");
     return false;
  }
  display.display();
  //write2Display(String("SystemInit"), String("Display +OK"), String(""), String(""));

  delay(1000);
  return true;
};

// TextSize(1) ^= 21 Zeichen
// TextSize(2) ^= 10 Zeichen
void write2Display(String head = "", String line1 = "", String line2 = "", String line3 = "", String line4 = ""){
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2); 
  display.setCursor(0,0);
  display.println(head);
  display.setTextSize(1);
  display.setCursor(0,20);
  display.println(line1);
  display.setCursor(0,32);
  display.println(line2);
  display.setCursor(0,44);
  display.println(line3);
  display.setCursor(0,56); // TextSize 1 needs 8 pixel minimum
  display.println(line4);
  display.display();
};


void write2Display(String head = "", String line1 = "", String line2 = "", String line3 = ""){
  write2Display(head, line1, line2, line3, "");
}


void tracker_display_tick(void){
  //Serial.print(".");
  static uint64_t ts = 0;
  if (ts + tracker_display_time > millis()) {
    return;
  }
  ts = millis();
  if (!displayCange) {
    return;
  }
  switch (tracker_display_switch())
  {
  case displayModeGPS:
    writeGPS();
    break;

  case displayModeUTC:
    writeUTC();
    break;

  
  default:
    writeUTC();
    break;
  }

}

uint8_t tracker_display_switch(void) {
  static uint8_t current = 0;
  uint8_t max = displayModeEND - 1;
  if (current < max) {
    current++;
  } else {
    current = 0;
  }
  Serial.println(current);
  return current;
}


void writeUTC(){
  display.clearDisplay();
  writeHead("   UTC");

  if (gps.time.isValid() && gps.date.isValid()) {
    char date[25], time[25];
    snprintf(date, 25, "%04d-%02d-%02d", gps.date.year(), gps.date.month(), gps.date.day());
    snprintf(time, 25, "  %02d:%02d", gps.time.hour(), gps.time.minute());

    display.setTextSize(2);
    display.setCursor(0,22);
    display.println(time);
    display.setCursor(0,44);
    display.println(date);
    display.display();
  } else {
    write_no_vaild_data();
  }
};
void writeGPS(){

  display.clearDisplay();
  writeHead("   GPS");

  if (gps.location.isValid()) {
    char lat[22], lng[22],  speed_course[22], alt_hdop[22];    
    snprintf(lat ,22, "lat: %8.6f", gps.location.lat());
    snprintf(lng ,22, "lng: %9.6f", gps.location.lng());
    snprintf(speed_course ,22, "kmh: %05.1f  %03d", gps.speed.kmph(), int(gps.course.value()));
    snprintf(alt_hdop ,22, "alt: %05dm sat: %d", int(gps.altitude.meters()), int(gps.satellites.value()));


    display.setTextSize(1);
    display.setCursor(0,20);
    display.println(lat);
    display.setCursor(0,32);
    display.println(lng);
    display.setCursor(0,44);
    display.println(speed_course);
    display.setCursor(0,56); // TextSize 1 needs 8 pixel minimum
    display.println(alt_hdop);
    display.display();

  } else {
    write_no_vaild_data();
  }

};
void writeWX(){
  display.clearDisplay();
  writeHead("   WX");

};


// fuer Erweiterungen in der Hauptzeile z.B. ttl next tx
void writeHead(const char* head){
  char headx[11];
  char sat[3];
  char hdop[3];
  
  if (gps.satellites.isValid() && gps.hdop.isValid()) {
    snprintf(sat, 3, "%d", int(gps.satellites.value()));
    snprintf(hdop, 3, "%d", int(gps.hdop.hdop()));
  } else {
    strcpy(sat, "--");
    strcpy(hdop, "--");
  }

  display.setTextColor(WHITE);
  display.setTextSize(2); 
  display.setCursor(0,0);
  display.print(head);
  display.setTextSize(1);
  display.setCursor(120,0);
  display.print(sat);
  display.setCursor(120,10);
  display.print(hdop);

}

void write_no_vaild_data() {
  display.setCursor(0,22);
  display.println("no valid");
  display.setCursor(0,44);
  display.println("data yet");
  display.display();
}

void write3Line(const char *head, const char *line1, const char *line2, bool toSerial, u_long sleep){
  display.clearDisplay();
  writeHead(head);

  if (strlen(line1)> 10) {
    display.setTextSize(1);   
  } else {
    display.setTextSize(2); 
  }
  display.setCursor(0,22);
  display.print(line1);

  if (strlen(line2)> 10) {
    display.setTextSize(1);   
  } else {
    display.setTextSize(2); 
  }
  display.setCursor(0,44);
  display.print(line2);
  display.display();
  if (toSerial) {
    write3toSerial(head, line1, line2, sleep);
  } else {
    delay(sleep);
  }
  

}

void write3toSerial(const char *head, const char *line1, const char *line2, u_long sleep){
    Serial.println("\n+----------------------");
    Serial.printf("| %s\n", head);
    Serial.println("+----------------------");
    Serial.printf("| %s\n", line1);
    Serial.printf("| %s\n", line2);
    Serial.println("+----------------------\n\n");
    delay(sleep);
}

void writeTX(const char *to) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,22);
  display.print(" << TX >> ");
  display.setCursor(0,44);
  display.print(to);
  display.display();
}
