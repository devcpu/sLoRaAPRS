/*
 * File: GPSSensor.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-29 0:28
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <APRSControler.h>
#include <Arduino.h>
#include <Config.h>
#include <GPSSensor.h>
#include <LoRaAPRSConfig.h>
#include <TinyGPS++.h>

extern Config reg;
extern TinyGPSPlus gps;
extern APRSControler maincontroler;

void setGPSData() {
  static double lat = 0;
  static double lng = 0;
  static double course = 0;
  static uint64_t last_update = 0;
  double distance = 0;
  double tmp = 0;

  if (gps.time.isValid()) {
    reg.gps_time.hour = gps.time.hour();
    reg.gps_time.minute = gps.time.minute();
    reg.gps_time.second = gps.time.second();
  }

  if (gps.date.isValid()) {
    reg.gps_time.year = gps.date.year();
    reg.gps_time.month = gps.date.month();
    reg.gps_time.day = gps.date.day();
  }

  if (gps.location.isValid()) {
    reg.gps_location.latitude = gps.location.lat();
    reg.gps_location.longitude = gps.location.lng();
    reg.gps_location.altitude = gps.altitude.meters();

    // dynamic update distace last position
    distance = TinyGPSPlus::distanceBetween(lat, lng, reg.gps_location.latitude,
                                            reg.gps_location.longitude);
  }
  if (gps.course.isValid()) {
    reg.gps_move.course = gps.course.deg();
  }
  if (gps.speed.isValid()) {
    reg.gps_move.speed = gps.speed.kmph();
  }
  // needs ~ 100µs (72 - 204 µs!)
  if (gps.satellites.isValid()) {
    reg.gps_meta.sat = gps.satellites.value();
  }
  if (gps.hdop.isValid()) {
    reg.gps_meta.hdop = gps.hdop.hdop();
    // Serial.printf("Hdop age: %d\n", gps.hdop.age());
    // Serial.printf("Hdop value: %d\n", gps.hdop.value());
    // Serial.printf("Hdop hdop %f\n", gps.hdop.hdop());
  }

  if (distance > APRS_UPDATE_DISTANCE) {
    maincontroler.gps_update = true;
  }

  // @TODO @see https://www.aprs-berlin.de/
  if (course < 180 && reg.gps_move.course > 180) {
    tmp = course + 180;
    if ((course - reg.gps_move.course) > APRS_UPDATE_ANGLE)
      maincontroler.gps_update = true;
  }
  if (reg.gps_move.course < 180 && course > 180) {
    tmp = reg.gps_move.course + 180;
    if ((reg.gps_move.course - course) > APRS_UPDATE_ANGLE)
      maincontroler.gps_update = true;
  }
  if (abs(course - reg.gps_move.course) > APRS_UPDATE_ANGLE) {
    maincontroler.gps_update = true;
  }

  if (reg.gps_move.course > course &&
      reg.gps_move.course - course > APRS_UPDATE_ANGLE) {
    maincontroler.gps_update = true;
  }

  if (last_update < millis()) {
    maincontroler.gps_update = true;
  }

  if (maincontroler.gps_update == true) {
    lat = reg.gps_location.latitude;
    lng = reg.gps_location.longitude;
    course = reg.gps_move.course;
    last_update = millis() + APRS_UPDATE_INTERVAL;
  }
}
