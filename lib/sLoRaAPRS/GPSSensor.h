/*
 * File: GPSSensor.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-17 18:53
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_GPSSENSOR_H_
#define LIB_SLORAAPRS_GPSSENSOR_H_

#include <Arduino.h>
#include <TinyGPS++.h>

extern HardwareSerial ss;
extern TinyGPSPlus gps;
void setGPSData();
void GPSReadIdleHookCB();

#endif // LIB_SLORAAPRS_GPSSENSOR_H_
