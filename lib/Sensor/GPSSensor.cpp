#include <Arduino.h>
#include <GPSSensor.h>
#include <Registry.h>
#include <TinyGPS++.h>
#include <LoRaAPRSConfig.h>

extern Registry reg;
extern TinyGPSPlus gps;

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
    distance = TinyGPSPlus::distanceBetween(lat, lng, reg.gps_location.latitude,reg.gps_location.longitude);
  }
  if (gps.course.isValid()) {
    reg.gps_move.course = gps.course.deg();
  }
  if (gps.speed.isValid()) {
    reg.gps_move.speed = gps.speed.kmph();
  }
  // needs ~ 100µs (72 - 204 µs!)


  if (distance > APRS_UPDATE_DISTANCE) {
    reg.contral_data.gps_update = true;
  }

  // @TODO @see https://www.aprs-berlin.de/
  if (course < 180 && reg.gps_move.course > 180) {
    tmp = course + 180;
    if ((course - reg.gps_move.course) > APRS_UPDATE_ANGLE)
    reg.contral_data.gps_update = true;
  }
  if (reg.gps_move.course < 180 && course > 180) {
    tmp = reg.gps_move.course + 180;
    if ((reg.gps_move.course - course) > APRS_UPDATE_ANGLE)
    reg.contral_data.gps_update = true;
  }
  if (abs(course - reg.gps_move.course) > APRS_UPDATE_ANGLE ) {
    reg.contral_data.gps_update = true;
  }

  if (reg.gps_move.course > course && reg.gps_move.course - course > APRS_UPDATE_ANGLE) {
    reg.contral_data.gps_update = true;

  }

  if (last_update < millis() ) {
    reg.contral_data.gps_update = true;
  }

  if (reg.contral_data.gps_update == true) {
    lat = reg.gps_location.latitude;
    lng = reg.gps_location.longitude;
    course = reg.gps_move.course;
    last_update = millis() + APRS_UPDATE_INTERVAL;
  }

  

}