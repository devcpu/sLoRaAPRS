/*
 * File: TrackerDisplay.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:14
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-26 17:53
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include "TrackerDisplay.h"

#include <Arduino.h>

#include "APRSControler.h"
#include "Config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

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

/**
 * @brief Initialize SSD1306 OLED Display on 0x3C.
 * Not FreeRTOS save!
 *
 * @return true if +OK
 * @return false if error
 */
bool DisplayInit(void) {
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {  // Address 0x3D for 128x64
    Serial.println("Dsiplay SSD1306 on 0x3c not ready!");
    return false;
  }
  display.display();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  return true;
}

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
}

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
}

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

/**
 * @brief writes time in utc to OLED display
 *
 */
void writeUTC() {
  if (gps.time.isValid() && gps.date.isValid()) {
    char date[25], time[25];
    snprintf(date, sizeof(date), "%04d-%02d-%02d", cfg.gps_time.year,
             cfg.gps_time.month, cfg.gps_time.day);
    snprintf(time, sizeof(time), "  %02d:%02d", cfg.gps_time.hour,
             cfg.gps_time.minute);

    write3Line(" - UTC -", time, date, false, 0);
  } else {
    _write_no_vaild_data();
  }
}

/**
 * @brief writes current gps position to OLED display
 *
 */
void writeGPS() {
  display.clearDisplay();
  writeHead("   GPS");

  if (gps.location.isValid()) {
    char lat[22], lng[22], speed_course[22], alt_hdop[22];
    snprintf(lat, sizeof(lat), "lat: %8.4f", cfg.gps_location.latitude);
    snprintf(lng, sizeof(lng), "lng: %9.4f", cfg.gps_location.longitude);
    snprintf(speed_course, sizeof(speed_course), "kmh: %4.1f  dir: %3.1f*",
             cfg.gps_move.speed, cfg.gps_move.course);
    snprintf(alt_hdop, sizeof(alt_hdop), "alt: %5.1fm sat: %d",
             cfg.gps_location.altitude, cfg.gps_meta.sat);

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
    _write_no_vaild_data();
  }
}

/**
 * @brief writes wether info to OLED display
 *
 */
void writeWX() {
  char temp_buf[24] = {0};
  char hum_buf[24] = {0};
  char press_buf[24] = {0};
  snprintf(temp_buf, sizeof(temp_buf), "Temp:      %3.0f *C",
           round(cfg.WXdata.temp));
  snprintf(hum_buf, sizeof(hum_buf), "Humidity:  %3.0f %%",
           round(cfg.WXdata.humidity));
  snprintf(press_buf, sizeof(press_buf), "Pressure: %4.0f hPa",
           round(cfg.WXdata.pressure));
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
}

/**
 * @brief writes current WIFI status to OLED display
 *
 */
void writeWiFiStatus() {
  display.clearDisplay();
  if (cfg.lan_status.mode == wifi_ap) {
    writeHead("Wifi AP");
    display.setTextSize(1);
    display.setCursor(0, 22);
    display.print(cfg.lan_status.SSID.c_str());
    display.setCursor(0, 36);
    display.print(cfg.lan_status.status.c_str());
    display.setCursor(0, 50);
    display.print(cfg.lan_status.IP.c_str());
  }
  if (cfg.lan_status.mode == wifi_client) {
    writeHead("Wifi Clnt");
    display.setTextSize(1);
    display.setCursor(0, 22);
    display.print(cfg.lan_status.SSID.c_str());
    display.setCursor(0, 36);
    display.print(cfg.lan_status.status.c_str());
    display.setCursor(0, 50);
    display.print(cfg.lan_status.IP.c_str());
  }

  if (cfg.lan_status.mode == wifi_off) {
    writeHead("  Wifi");
    display.setTextSize(2);
    display.setCursor(0, 32);
    display.print("OFF");
  }

  display.display();
}

/**
 * @brief fuer Erweiterungen in der Hauptzeile z.B. ttl next tx
 *
 * @param head
 */
void writeHead(const char *head) {
  char sat[3];
  char hdop[3];

  if (gps.satellites.isValid() && gps.hdop.isValid()) {  // @FIXME
    snprintf(sat, sizeof(sat), "%d", cfg.gps_meta.sat);
    snprintf(hdop, sizeof(hdop), "%d",
             static_cast<int>(round(cfg.gps_meta.hdop)));
  } else {
    strncpy(sat, "", sizeof(sat) - 1);
    strncpy(hdop, "", sizeof(hdop) - 1);
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

/**
 * @brief if gps / time is not valid prints this to OLED display
 *
 */
void _write_no_vaild_data() {
  display.clearDisplay();
  display.setCursor(0, 22);
  display.print("no valid");
  display.setCursor(0, 44);
  display.print("data yet");
  display.display();
}

/**
 * @brief prints head + lines to OLED display.
 * Attention! Not FreeRTOS save!
 *
 * @param const char *head head line
 * @param const char *line1
 * @param const char *line2
 * @param bool toSerial
 * @param u_long sleep
 */
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
    if (sleep) {
      vTaskDelay(sleep / portTICK_PERIOD_MS);
    }
  }
}

void write3toSerial(const char *head, const char *line1, const char *line2,
                    u_long sleep) {
  String s1 = String(head);
  s1.trim();
  String s2 = String(line1);
  s2.trim();
  String s3 = String(line2);
  s3.trim();
  Serial.printf("+++ %s %s %s +++\n", s1.c_str(), s2.c_str(), s3.c_str());
  if (sleep) {
    vTaskDelay(sleep / portTICK_PERIOD_MS);
  }
}

/**
 * @brief show that LoRa is sending
 *
 * @param to
 */
void writeTX(const char *to) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 22);
  display.print(" << TX >> ");
  display.setCursor(0, 44);
  display.print(to);
  display.display();
}
