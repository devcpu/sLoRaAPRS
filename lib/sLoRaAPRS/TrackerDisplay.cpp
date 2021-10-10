/*
 * File: TrackerDisplay.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:14
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-10 23:44
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <TrackerDisplay.h>

extern Config cfg;

// interrupt lock?
// portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; // DHTesp.h

Adafruit_SSD1306 display(128, 64, &Wire, OLED_RESET);

/**
 * @brief if true text on display will changed continous
 *
 */
bool displayChange = true;

TrackerDisplay::TrackerDisplay() {
  // vPortCPUInitializeMutex(&my_mutex);
}

bool TrackerDisplay::begin() {
  TrackerDisplay::begin(60); // 60  == 0x3C"
}

bool TrackerDisplay::begin(uint8_t i2c_address) {
  if (!display.begin(SSD1306_SWITCHCAPVCC,
                     i2c_address)) { // Address 0x3D for 128x64
    ESP_LOGE(TAG, "Dsiplay SSD1306 on %s not ready!", i2c_address);
    return false;
  }
  display.display();
  return true;
}

void TrackerDisplay::write2Display(String head, String line1, String line2, String line3, String line4) {
  // TextSize(1) ^= 21 Zeichen
  // TextSize(2) ^= 10 Zeichen
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  _center_line(head.c_str());
  display.setCursor(0, 20);
  _center_line(line1.c_str());
  display.setCursor(0, 32);
  _center_line(line2.c_str());
  display.setCursor(0, 44);
  _center_line(line3.c_str());
  display.setCursor(0, 56); // TextSize 1 needs 8 pixel minimum
  _center_line(line4.c_str());
  display.display();
}

void TrackerDisplay::write2Display(String head, String line1, String line2, String line3) {
  write2Display(head, line1, line2, line3, "");
}

void TrackerDisplay::write2Display(const char *head, const char *line1, const char *line2, const char *line3,
                                   const char *line4) {
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
  display.setCursor(0, 56); // TextSize 1 needs 8 pixel minimum
  display.print(line4);
  display.display();
}

void TrackerDisplay::nextDisplayMode(void) {
  uint8_t max = displayModeEND - 1;
  if (current_display_mode < max) {
    uint8_t x = static_cast<int>(current_display_mode) + 1;
    current_display_mode = (DisplayMode)x;
  } else {
    current_display_mode = (DisplayMode)0;
  }
}

void TrackerDisplay::showDisplayMode(void) {
  switch (current_display_mode) {
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

void TrackerDisplay::writeUTC() {
  display.clearDisplay();
  _writeHead("- UTC -");

  if (gps.time.isValid() && gps.date.isValid()) {
    char date[25], time[25];
    snprintf(date, sizeof(date), "%04d-%02d-%02d", cfg.gps_time.year, cfg.gps_time.month, cfg.gps_time.day);
    snprintf(time, sizeof(time), "  %02d:%02d", cfg.gps_time.hour, cfg.gps_time.minute);

    write3Line("- UTC -", time, date, false, 0);
  } else {
    _write_no_vaild_data();
  }
}

void TrackerDisplay::writeGPS() {
  display.clearDisplay();
  _writeHead("GPS");

  if (gps.location.isValid()) {
    char lat[22], lng[22], speed_course[22], alt_hdop[22];
    snprintf(lat, sizeof(lat), "lat: %8.4f", cfg.gps_location.latitude);
    snprintf(lng, sizeof(lng), "lng: %9.4f", cfg.gps_location.longitude);
    snprintf(speed_course, sizeof(speed_course), "kmh: %4.1f  dir: %3.1f*", cfg.gps_move.speed, cfg.gps_move.course);
    snprintf(alt_hdop, sizeof(alt_hdop), "alt: %5.1fm sat: %d", cfg.gps_location.altitude, cfg.gps_meta.sat);

    // portENTER_CRITICAL(&my_mutex);
    // rtc_wdt_feed();
    // rtc_wdt_feed();
    // portEXIT_CRITICAL(&my_mutex);
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print(lat);
    display.setCursor(0, 32);
    display.print(lng);
    display.setCursor(0, 44);
    display.print(speed_course);
    display.setCursor(0, 56); // TextSize 1 needs 8 pixel minimum
    display.print(alt_hdop);
    display.display();

  } else {

    _write_no_vaild_data();
  }
}

void TrackerDisplay::writeWX() {
  char temp_buf[24] = {0};
  char hum_buf[24] = {0};
  char press_buf[24] = {0};
  snprintf(temp_buf, sizeof(temp_buf), "Temp:      %3.0f *C", round(cfg.WXdata.temp));
  snprintf(hum_buf, sizeof(hum_buf), "Humidity:  %3.0f %%", round(cfg.WXdata.humidity));
  snprintf(press_buf, sizeof(press_buf), "Pressure: %4.0f hPa", round(cfg.WXdata.pressure));
  display.clearDisplay();
  _writeHead("WX");
  display.setTextSize(1);
  display.setCursor(0, 22);
  display.print(temp_buf);
  display.setCursor(0, 36);
  display.print(hum_buf);
  display.setCursor(0, 50);
  display.print(press_buf);
  display.display();
}

void TrackerDisplay::writeWiFiStatus() {
  display.clearDisplay();

  if (cfg.lan_status.mode == wifi_ap) {
    _writeHead("Wifi AP");
    display.setTextSize(1);
    display.setCursor(0, 22);
    _center_line(cfg.lan_status.SSID.c_str());
    display.setCursor(0, 38);
    _center_line(cfg.lan_status.status.c_str());
    display.setCursor(0, 54);
    _center_line(cfg.lan_status.IP.c_str());
  }
  if (cfg.lan_status.mode == wifi_client) {
    _writeHead("Wifi Clnt");
    display.setTextSize(1);
    display.setCursor(0, 22);
    _center_line(cfg.lan_status.SSID.c_str());
    display.setCursor(0, 38);
    _center_line(cfg.lan_status.status.c_str());
    display.setCursor(0, 54);
    _center_line(cfg.lan_status.IP.c_str());
  }

  if (cfg.lan_status.mode == wifi_off) {
    _writeHead("Wifi");
    display.setTextSize(2);
    display.setCursor(0, 32);
    _center_line("OFF");
  }
  display.display();
}

void TrackerDisplay::_writeHead(const char *head) {
  char buf[25] = {0};
  char sat[3];
  char hdop[3];

  // if (gps.satellites.isValid() && gps.hdop.isValid()) {
  //   snprintf(sat, sizeof(sat), "%d", cfg.gps_meta.sat);
  //   snprintf(hdop, sizeof(hdop), "%d", static_cast<int>(round(cfg.gps_meta.hdop)));
  // } else {
  //   strncpy(sat, "", sizeof(sat) - 1);
  //   strncpy(hdop, "", sizeof(hdop) - 1);
  // }

  if (gps.satellites.isValid()) {
    snprintf(sat, sizeof(sat), "%d", cfg.gps_meta.sat);
  } else {
    strncpy(sat, "", sizeof(sat) - 1);
  }
  if (gps.hdop.isValid()) {
    snprintf(hdop, sizeof(hdop), "%d", static_cast<int>(round(cfg.gps_meta.hdop)));
  } else {
    strncpy(hdop, "", sizeof(hdop) - 1);
  }
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.print(_center_line(buf, head));
  display.setTextSize(1);
  display.setCursor(116, 0);
  display.print(sat);
  display.setCursor(116, 10);
  display.print(hdop);
}

void TrackerDisplay::_write_no_vaild_data() {
  display.setCursor(0, 22);
  _center_line("no valid");
  display.setCursor(0, 44);
  _center_line("data yet");
  display.display();
}

void TrackerDisplay::write3Line(const char *head, const char *line1, const char *line2, bool toSerial, u_long sleep) {
  display.clearDisplay();
  _writeHead(head);

  display.setCursor(0, 22);
  _center_line(line1);

  display.setCursor(0, 44);
  _center_line(line2);

  display.display();

  if (toSerial) {
    write3toSerial(head, line1, line2, sleep);
  } else {
    if (sleep) {
      vTaskDelay(sleep / portTICK_PERIOD_MS);
    }
  }
}

void TrackerDisplay::write3toSerial(const char *head, const char *line1, const char *line2, u_long sleep) {
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

void TrackerDisplay::writeTX(String to) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 22);
  _center_line(" << TX >> ");
  display.setCursor(0, 44);
  _center_line(to.c_str());
  display.display();
}

void TrackerDisplay::_center_line(const char *in) {
  char buf[25] = {0};
  if (strlen(in) > 10) {
    display.setTextSize(1);
  } else {
    display.setTextSize(2);
  }
  display.print(_center_line(buf, in));
}

char *TrackerDisplay::_center_line(char *rv, const char *s) {
  uint8_t x = 5;
  if (strlen(s) > 21) {
    snprintf(rv, sizeof(rv), "to long");
    ESP_LOGE(TAG, "ERROR %s to long!", s);
    return rv;
  }
  if (strlen(s) > 10) {
    x = 10;
  }
  snprintf(rv, 24, "%*s%*s", x + strlen(s) / 2, s, x - strlen(s) / 2, ""); // NOLINT(runtime/printf)
  // ESP_LOGD(TAG, "---%s---", rv);
  return rv;
}
