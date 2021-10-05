/*
 * File: BMEHandler.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-15 3:03
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_BMEHANDLER_H_
#define LIB_SLORAAPRS_BMEHANDLER_H_

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

// @FIXME should be configureable via web interface or by GPS!
// 1013.25 ^= -50.96m
// 1100.00 ^= 634.93m
// 1033.25 ^= 116.89m
// 1029.57 ^= 83.80 m
#define SEALEVELPRESSURE_HPA (1029.57)

bool BMEHandlerInit(void);
void setWXData(void);
void Sensor_tick();

extern Adafruit_BME280 bme;

#endif // LIB_SLORAAPRS_BMEHANDLER_H_
