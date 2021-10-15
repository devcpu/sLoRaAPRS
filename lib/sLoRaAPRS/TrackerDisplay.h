/*
 * File: TrackerDisplay.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-15 23:49
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_TRACKERDISPLAY_H_
#define LIB_SLORAAPRS_TRACKERDISPLAY_H_

#include <soc/rtc_wdt.h>
#include <APRS_MSG.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_SSD1306.h>
#include <gfxfont.h>
#include <splash.h>
#include <Arduino.h>
#include <Config.h>
// #include <TinyGPS++.h>
#include <GPSSensor.h>

#define OLED_RESET 4

/**
 * @brief differnet modes/screens on OLED
 */
typedef enum {
  displayModeUTC,        // shows date + time in UTC
  displayModeGPS,        // shows GPS data
  displayModeWX,         // shows weather sensor data
  displayModeWiFiStatus, // shows wifi infos
  displayModeEND         // for intern only @TODO to fix? obsolete?
} DisplayMode;

class TrackerDisplay {
 public:
  /**
   * @brief Construct a new Tracker Display object
   *
   */
  TrackerDisplay();

  /**
   * @brief inizialize OLED display
   *
   * @param i2c_address
   */
  bool begin(uint8_t i2c_address);

  /**
   * @brief inizialize OLED display on I2C address 0x3C
   *
   */
  bool begin();

  /**
   * @brief current display mode for OLED
   *
   */
  DisplayMode current_display_mode = displayModeUTC;

  // portMUX_TYPE my_mutex;

  /**
   * @brief writes head + 4 lines to OLED
   *
   * @param head  text size 2 (bigger)
   * @param line1 text size 1 (normal)
   * @param line2 text size 1 (normal)
   * @param line3 text size 1 (normal)
   * @param line4 text size 1 (normal)
   */
  void write2Display(const char *head, const char *line1, const char *line2, const char *line3, const char *line4);

  /**
   * @brief writes head + 3 lines to OLED
   *
   * @param head  text size 2 (bigger)
   * @param line1 text size 1 (normal)
   * @param line2 text size 1 (normal)
   * @param line3 text size 1 (normal)
   */
  void write2Display(String head, String line1, String line2, String line3);

  /**
   * @brief writes head + 4 lines to OLED
   *
   * @param line1 text size 1 (normal)
   * @param line2 text size 1 (normal)
   * @param line3 text size 1 (normal)
   * @param line4 text size 1 (normal)
   */
  void write2Display(String head, String line1, String line2, String line3, String line4);

  /**
   * @brief set current_display_mode to next DisplayMode
   *
   */
  void nextDisplayMode(void);

  /**
   * @brief show current_display_mode on OLED
   *
   */
  void showDisplayMode(void);

  /**
   * @brief writes time in utc to OLED
   *
   */
  void writeUTC();

  /**
   * @brief writes current gps position to OLED
   *
   */
  void writeGPS();

  /**
   * @brief  writes weather info to OLED
   *
   */
  void writeWX();

  /**
   * @brief writes current WIFI status to OLED
   *
   */
  void writeWiFiStatus();

  /**
   * @brief prints head + lines to OLED display, optional to serial too
   *
   * @param head
   * @param line1
   * @param line2
   * @param toSerial if has to print to serial too
   * @param sleep time for wait for return
   */
  void write3Line(const char *head, const char *line1, const char *line2, bool toSerial, u_long sleep);

  /**
   * @brief show that LoRa is sending
   *
   * @param to target call, if given
   */
  void writeTX(String to = "broadcast");

  /**
   * @brief writes head + 2 lines to serial
   *
   * @param head
   * @param line1
   * @param line2
   * @param sleep default 0
   */
  void write3toSerial(const char *head, const char *line1, const char *line2, u_long sleep = 0);

 private:
  /**
   * @brief if gps / time is not valid prints this to OLED display
   *
   */
  void _write_no_vaild_data(void);

  /**
   * @brief Write head string and satellites and hdop at right
   *
   * @param head
   */
  void _writeHead(const char *head);

  /**
   * @brief center string in in rv
   *
   * @param rv
   * @param in
   * @return char*
   */
  char *_center_line(char *rv, const char *in);

  void _center_line(const char *in);
};

extern TrackerDisplay td;

#endif // LIB_SLORAAPRS_TRACKERDISPLAY_H_
