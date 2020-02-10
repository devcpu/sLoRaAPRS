#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include <APRSControler.h>
#include <Registry.h>
#include <TrackerDisplay.h>
#include <uxa_debug.h>

extern APRSControler maincontroler;
extern TimerHandle_t button_timer;
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

/**
 * @brief if true text on display will changed continous
 *
 */
bool displayChange = true;

/**
 * @brief time until nex display info
 * @TODO move to LoRaAPRSConfig.h
 */
uint16_t tracker_display_time = 5000;

/**
 * @brief for getting gps data
 *
 * @var	extern	TinyGPSPlus
 */
extern TinyGPSPlus gps;

bool DisplayInit(void) {
  Serial.println("Init Display SSD1306!");
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println("Dsiplay SSD1306 on 0x3c not ready!");
    return false;
  }
  display.display();
  delay(1000);
  return true;
};

// TextSize(1) ^= 21 Zeichen
// TextSize(2) ^= 10 Zeichen
void write2Display(String head = "", String line1 = "", String line2 = "",
                   String line3 = "", String line4 = "") {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(head);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print(line1);
  display.setCursor(0, 32);
  display.print(line2);
  display.setCursor(0, 44);
  display.print(line3);
  display.setCursor(0, 56);  // TextSize 1 needs 8 pixel minimum
  display.print(line4);
  display.display();
};

void write2Display(String head = "", String line1 = "", String line2 = "",
                   String line3 = "") {
  write2Display(head, line1, line2, line3, "");
}

void write2Display(const char *head, const char *line1, const char *line2,
                   const char *line3, const char *line4) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(head);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print(line1);
  display.setCursor(0, 32);
  display.print(line2);
  display.setCursor(0, 44);
  display.print(line3);
  display.setCursor(0, 56);  // TextSize 1 needs 8 pixel minimum
  display.print(line4);
  display.display();
};

void tracker_display_tick(void) {
  DisplayMode dm = displayModeUTC;
  if (!maincontroler.display_update) {
    return;
  }
  if (maincontroler.next_display_time > millis()) {
    return;
  }

  maincontroler.next_display_time = millis() + tracker_display_time;
  if (maincontroler.display_change) {
    dm = maincontroler.getNextDisplayMode();
  } else {
    dm = maincontroler.getCurrentDisplayMode();
  }
  switch (dm) {
    case displayModeGPS:
      writeGPS();
      break;

    case displayModeUTC:
      writeUTC();
      break;

    case displayModeWX:
      writeWX();
      break;

    case displayModeWiFiStatus:
      writeWiFiStatus();
      break;

    default:
      writeUTC();
      break;
  }
}

// uint8_t tracker_display_switch(void) {
//   static uint8_t current = 0;
//   if (!displayChange) {
//     return current;
//   }
//   uint8_t max = displayModeEND - 1;
//   if (current < max) {
//     current++;
//   } else {
//     current = 0;
//   }
//   return current;
// }

void writeUTC() {
  // display.clearDisplay();
  // writeHead("   UTC");

  if (gps.time.isValid() && gps.date.isValid()) {
    char date[25], time[25];
    snprintf(date, 25, "%04d-%02d-%02d", reg.gps_time.year, reg.gps_time.month,
             reg.gps_time.day);
    snprintf(time, 25, "  %02d:%02d", reg.gps_time.hour, reg.gps_time.minute);

    // display.setTextSize(2);
    // display.setCursor(0, 22);
    // display.print(time);
    // display.setCursor(0, 44);
    // display.print(date);
    // display.display();
    write3Line(" - UTC -", time, date, false, 0);
  } else {
    write_no_vaild_data();
  }
};
void writeGPS() {
  display.clearDisplay();
  writeHead("   GPS");

  if (gps.location.isValid()) {
    char lat[22], lng[22], speed_course[22], alt_hdop[22];
    snprintf(lat, 22, "lat: %8.4f", reg.gps_location.latitude);
    snprintf(lng, 22, "lng: %9.4f", reg.gps_location.longitude);
    snprintf(speed_course, 22, "kmh: %4.1f  dir: %3.1f*", reg.gps_move.speed,
             reg.gps_move.course);
    snprintf(alt_hdop, 22, "alt: %5.1fm sat: %d", reg.gps_location.altitude,
             reg.gps_meta.sat);

    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print(lat);
    display.setCursor(0, 32);
    display.print(lng);
    display.setCursor(0, 44);
    display.print(speed_course);
    display.setCursor(0, 56);  // TextSize 1 needs 8 pixel minimum
    display.print(alt_hdop);
    display.display();

  } else {
    write_no_vaild_data();
  }
};
void writeWX() {
  char temp_buf[24] = {0};
  char hum_buf[24] = {0};
  char press_buf[24] = {0};
  snprintf(temp_buf, 24, "Temp:      %3.0f *C", round(reg.WXdata.temp));
  snprintf(hum_buf, 24, "Humidity:  %3.0f %%", round(reg.WXdata.humidity));
  snprintf(press_buf, 24, "Pressure: %4.0f hPa", round(reg.WXdata.pressure));
  display.clearDisplay();
  writeHead("   WX");
  display.setTextSize(1);
  display.setCursor(0, 22);
  display.print(temp_buf);
  display.setCursor(0, 36);
  display.print(hum_buf);
  display.setCursor(0, 50);
  display.print(press_buf);

  display.display();
};

void writeWiFiStatus() {
  display.clearDisplay();
  if (reg.lan_status.mode == wifi_ap) {
    writeHead("Wifi AP");
    display.setTextSize(1);
    display.setCursor(0, 22);
    display.print(reg.lan_status.SSID.c_str());
    display.setCursor(0, 36);
    display.print(reg.lan_status.status.c_str());
    display.setCursor(0, 50);
    display.print(reg.lan_status.IP.c_str());
  }
  if (reg.lan_status.mode == wifi_client) {
    writeHead("Wifi Clnt");
    display.setTextSize(1);
    display.setCursor(0, 22);
    display.print(reg.lan_status.SSID.c_str());
    display.setCursor(0, 36);
    display.print(reg.lan_status.status.c_str());
    display.setCursor(0, 50);
    display.print(reg.lan_status.IP.c_str());
  }

  if (reg.lan_status.mode == wifi_off) {
    writeHead("  Wifi");
    display.setTextSize(2);
    display.setCursor(0, 32);
    display.print("OFF");
  }

  display.display();
}

// fuer Erweiterungen in der Hauptzeile z.B. ttl next tx
void writeHead(const char *head) {
  char sat[3];
  char hdop[3];

  if (gps.satellites.isValid() && gps.hdop.isValid()) {
    // snprintf(sat, 3, "%d", int(gps.satellites.value()));
    // snprintf(hdop, 3, "%d", int(gps.hdop.hdop()));
    snprintf(sat, 3, "%d", reg.gps_meta.sat);
    snprintf(hdop, 3, "%d", int(round(reg.gps_meta.hdop)));
  } else {
    strcpy(sat, "");
    strcpy(hdop, "");
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(head);
  display.setTextSize(1);
  display.setCursor(116, 0);
  display.print(sat);
  display.setCursor(116, 10);
  display.print(hdop);
}

void write_no_vaild_data() {
  display.clearDisplay();
  display.setCursor(0, 22);
  display.print("no valid");
  display.setCursor(0, 44);
  display.print("data yet");
  display.display();
}

void write3Line(const char *head, const char *line1, const char *line2,
                bool toSerial, u_long sleep) {
  
  display.clearDisplay();
  writeHead(head);

  if (strlen(line1) > 10) {
    display.setTextSize(1);
  } else {
    display.setTextSize(2);
  }
  display.setCursor(0, 22);
  display.print(line1);

  if (strlen(line2) > 10) {
    display.setTextSize(1);
  } else {
    display.setTextSize(2);
  }
  display.setCursor(0, 44);
  display.print(line2);
  display.display();

  if (toSerial) {
    write3toSerial(head, line1, line2, sleep);
  } else {
    delay(sleep);
  }
}

void write3toSerial(const char *head, const char *line1, const char *line2,
                    u_long sleep) {
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
  display.setCursor(0, 22);
  display.print(" << TX >> ");
  display.setCursor(0, 44);
  display.print(to);
  display.display();
}
