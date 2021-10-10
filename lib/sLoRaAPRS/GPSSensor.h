/*
 * File: GPSSensor.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-10 23:49
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_GPSSENSOR_H_
#define LIB_SLORAAPRS_GPSSENSOR_H_

#include <TinyGPS++.h>

extern TinyGPSPlus gps;
void setGPSData();

#endif // LIB_SLORAAPRS_GPSSENSOR_H_
