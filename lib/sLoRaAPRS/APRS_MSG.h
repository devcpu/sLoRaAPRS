/*
 * File: APRS_MSG.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-29 0:21
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_APRS_MSG_H_
#define LIB_SLORAAPRS_APRS_MSG_H_

#include <Arduino.h>
#include <Config.h>
#include <LoRaAPRSConfig.h>
#include <TinyGPS++.h>
#include <apptypes.h>

extern Config cfg;

#define CONVERT_FEED_METER 3.28084
#define WX_DEVICE_STR_LENGHT 12
#define CALL_LENGHT 12  // @TODO needs check if it is in APRS OK

class APRS_MSG {
 public:
  // enum aprs_symbol_table_e { primary = '/', alternate = '\\' };

  static char *computeAPRSPos(char *aprs_buf);
  static char *computeWXField(char *rv);
  static char *computeTrackInfo(char *rv);
  static char *calcHumidity(char *rv, float humidity);
  static char *computeTimestamp(char *rv);
  /**
   * @brief converts given miles/hour to km/h (imperial2metric)
   *
   * @param mph miles/hour
   * @return double_t km/h
   */
  static double_t mph2kmh(double_t mph);

  /**
   * @brief converts given km/h to miles/hour (metric2imperial)
   *
   * @param kmh km/h
   * @return double_t mph
   */
  static double_t kmh2mph(double_t kmh);

  /**
   * @brief converts hunderts of inch to mm (imperial2metric)
   *
   * @param hinch inch/100
   * @return double_t inch/100
   */
  static double_t hinch2mm(double_t hinch);

  /**
   * @brief converts mm to hunderts of inch (metric2imperial)
   *
   * @param mm mm
   * @return double_t inch/100
   */
  static double_t mm2hinch(double_t mm);

  /**
   * @brief converts temperature fahrendheit to Celsius (imperial2metric)
   *
   * @param temp °F
   * @return double_t °C
   */
  static double_t f2c(double_t temp);

  /**
   * @brief converts temperature Celsius to fahrendheit (metric2imperial)
   *
   * @param temp °C
   * @return double_t  °F
   */
  static double_t c2f(double_t temp);

  /**
   * @brief converts wgs data from dc (decimal) to APRS - gms
   * (grad,minutes,secundes), given from GPS Lib to APRS format
   *
   * keep in mind the format for APRS differs from normal.
   * 52.2904N in APRS will be 5229.04N!
   * there no negative for S or W, it will be show as W or S
   *
   * @param retvar _aprs_lng or _aprs_lat (char[10])
   * @param gpsdata lat or lng from GPS
   * @param lng must be true from lng, it sets fromat (xxxxx.xx for lng xxxx.xx
   * for lat and also E/W or N/S)
   *
   * @see https://www.gpskoordinaten.de/gps-koordinaten-konverter
   * @see
   * https://www.giga.de/extra/gps/tipps/gps-koordinaten-umrechnen-online-oder-mit-formel/
   */

  // static String dc2gms(double gpsdata, boolean lng);
  static char *dc2gms(char *rv, double gpsdata, boolean lng);

  static double_t feed2meter(double_t feed);
  static double_t meter2feed(double_t meter);

 private:
  void computeWXField(void);

  void computeTrackInfo(void);
};

#endif  // LIB_SLORAAPRS_APRS_MSG_H_
