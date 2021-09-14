/*
 * File: APRS_MSG.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:14
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-29 1:48
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <APRS_MSG.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uxa_debug.h>

extern TinyGPSPlus gps;

// ".../...g...t...r...p...P...h..b...."

char* APRS_MSG::computeWXField(char* rv) {
  char hum_buf[16] = {0};
  snprintf(rv, sizeof(rv), "%s/%sg%st%03dr%sp%sP%sh%sb%05d %s",
           "...",  // winddirection 0-360
           "...",  // windspeed in mph
           "...",  // gust
           static_cast<int8_t>(
               round(APRS_MSG::c2f(cfg.WXdata.temp))),  // temperature
           "...",                                       // rainfalllasthourhinch
           "...",                                       // rainfalllast24hinch
           "...",  // rainfallsinceMidnightinch
           APRS_MSG::calcHumidity(hum_buf, cfg.WXdata.humidity),  // humidity
           static_cast<uint8_t>(round(
               cfg.WXdata.pressure * 10)),  // pressure @FIXME / 100.00L * 10 ??
           "Sensor: BME280");  // wx sensor @FIXME sensor from i2c-scanner, not
                               // staticly
  return rv;
}

/**
 * @brief round and convert 100% humidity to APRS syle 00
 *
 * @param char* rv: return value
 * @param float humidity: humidity
 * @return char*
 */
char* APRS_MSG::calcHumidity(char* rv, float humidity) {
  if (humidity == 100) {
    snprintf(rv, sizeof(rv), "%s", "00");
  } else {
    snprintf(rv, sizeof(rv), "%02u", static_cast<uint8_t>(round(humidity)));
  }
  return rv;
}

char* APRS_MSG::computeAPRSPos(char* rv) {
  char buf_lat[16] = {0};
  char buf_lng[16] = {0};
  snprintf(
      rv, 32, "%s%c%s%c", dc2gms(buf_lat, cfg.gps_location.latitude, false),
      cfg.aprs_symbol.table, dc2gms(buf_lng, cfg.gps_location.longitude, true),
      cfg.aprs_symbol.symbol);
  return rv;
}

char* APRS_MSG::computeTrackInfo(char* rv) {
  snprintf(rv, sizeof(rv), "%03.0f/%03.0f/A=%06.0f", cfg.gps_move.speed,
           cfg.gps_move.course, cfg.gps_location.altitude);
  return rv;
}

char* APRS_MSG::computeTimestamp(char* rv) {
  // day/hour/minute
  snprintf(rv, sizeof(rv), "@%02d%02d%02dz", cfg.gps_time.day,
           cfg.gps_time.hour, cfg.gps_time.minute);
  return rv;
}

// char* APRS_MSG::getAPRSTxPos(char* retvar) {
//     computeAPRSPos();
//     snprintf(retvar, 254, "%s>APRS:!%s", _call, _aprs_pos);
//     //Serial.printf("APRS_MSG::getAPRSTxPos retvar '%s'\n", retvar);
//     return retvar;

// DL7UXA-1>APRS:!
//};

/***************************************************************************/
/**************************   static converters   **************************/
/***************************************************************************/
double_t APRS_MSG::feed2meter(double_t feed) {
  return round(feed / CONVERT_FEED_METER);
}

double_t APRS_MSG::meter2feed(double_t meter) {
  return round(meter * CONVERT_FEED_METER);
}

double_t APRS_MSG::mph2kmh(double_t mph) { return round(mph / 0.62137); }

double_t APRS_MSG::kmh2mph(double_t kmh) { return round(kmh * 0.62137); }

double_t APRS_MSG::hinch2mm(double_t hinch) { return round(hinch / 3.93701); }

double_t APRS_MSG::mm2hinch(double_t mm) { return round(mm * 3.93701); }

double_t APRS_MSG::f2c(double_t temp) { return round((temp - 32) * 5 / 9); }

double_t APRS_MSG::c2f(double_t temp) { return round(temp * 9 / 5 + 32); }

char* APRS_MSG::dc2gms(char* rv, double gpsdata, boolean lng) {
  char fstr[16] = {0};
  // char retvar[16] = {0};
  char ew;

  if (gpsdata < 0 && lng) {
    ew = 'W';
  }
  if (gpsdata >= 0 && lng) {
    ew = 'E';
  }
  if (gpsdata < 0 && !lng) {
    ew = 'S';
  }
  if (gpsdata >= 0 && !lng) {
    ew = 'N';
  }

  if (gpsdata < 0) {
    gpsdata = -gpsdata;
  }

  uint8_t grad = gpsdata;
  float tmp = gpsdata - grad;
  tmp = tmp * 60;
  uint8_t minute = tmp;
  tmp = tmp - minute;
  uint8_t secunde = tmp * 60;
  if (lng) {
    strncpy(fstr, "%03d%02d.%02d%c", sizeof(fstr));
  } else {
    strncpy(fstr, "%02d%02d.%02d%c", sizeof(fstr));
  }
  snprintf(rv, sizeof(rv), // Flawfinder: ignore
          fstr, grad, minute, secunde, ew);  
  return rv;
}
