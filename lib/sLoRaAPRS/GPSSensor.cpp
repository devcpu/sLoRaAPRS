/*
 * File: GPSSensor.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-17 18:52
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <Arduino.h>
#include <Config.h>
#include <GPSSensor.h>
#include <LoRaAPRSConfig.h>

// #include "maidenhead.h"

void GPSReadIdleHookCB() {
  static uint8_t c = 0;
  if (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      c++;
      if (c >= 10) {
        setGPSData();
        c = 0;
      }
    }
  }
}

void setGPSData() {
  static double lat = 0;
  static double lng = 0;
  static double course = 0;
  static uint64_t last_update = 0;
  double distance = 0;
  double tmp = 0;

  if (gps.time.isValid()) {
    cfg.gps_time.hour = gps.time.hour();
    cfg.gps_time.minute = gps.time.minute();
    cfg.gps_time.second = gps.time.second();
  }

  if (gps.date.isValid()) {
    cfg.gps_time.year = gps.date.year();
    cfg.gps_time.month = gps.date.month();
    cfg.gps_time.day = gps.date.day();
  }

  if (gps.location.isValid()) {
    cfg.gps_location.latitude = gps.location.lat();
    cfg.gps_location.longitude = gps.location.lng();
    cfg.gps_location.altitude = gps.altitude.meters();

    // dynamic update distace last position
    distance = TinyGPSPlus::distanceBetween(lat, lng, cfg.gps_location.latitude, cfg.gps_location.longitude);
  }
  if (gps.course.isValid()) {
    cfg.gps_move.course = gps.course.deg();
  }
  if (gps.speed.isValid()) {
    cfg.gps_move.speed = gps.speed.kmph();
  }
  // needs ~ 100µs (72 - 204 µs!)
  if (gps.satellites.isValid()) {
    cfg.gps_meta.sat = gps.satellites.value();
  }
  if (gps.hdop.isValid()) {
    cfg.gps_meta.hdop = gps.hdop.hdop();
    // Serial.printf("Hdop age: %d\n", gps.hdop.age());
    // Serial.printf("Hdop value: %d\n", gps.hdop.value());
    // Serial.printf("Hdop hdop %f\n", gps.hdop.hdop());
  }

  // @TODO FUTURE
  // if (distance > APRS_UPDATE_DISTANCE) {
  //   maincontroler.gps_update = true;
  // }
  // @TODO @see https://www.aprs-berlin.de/
  // if (course < 180 && cfg.gps_move.course > 180) {
  //   tmp = course + 180;
  //   if ((course - cfg.gps_move.course) > APRS_UPDATE_ANGLE)
  //     maincontroler.gps_update = true;
  // }
  // if (cfg.gps_move.course < 180 && course > 180) {
  //   tmp = cfg.gps_move.course + 180;
  //   if ((cfg.gps_move.course - course) > APRS_UPDATE_ANGLE)
  //     maincontroler.gps_update = true;
  // }
  // if (abs(course - cfg.gps_move.course) > APRS_UPDATE_ANGLE) {
  //   maincontroler.gps_update = true;
  // }

  // if (cfg.gps_move.course > course &&
  //     cfg.gps_move.course - course > APRS_UPDATE_ANGLE) {
  //   maincontroler.gps_update = true;
  // }

  //   if (last_update < millis()) {
  //     maincontroler.gps_update = true;
  //   }

  //   if (maincontroler.gps_update == true) {
  //     lat = cfg.gps_location.latitude;
  //     lng = cfg.gps_location.longitude;
  //     course = cfg.gps_move.course;
  //     last_update = millis() + APRS_UPDATE_INTERVAL;
  //   }
}
