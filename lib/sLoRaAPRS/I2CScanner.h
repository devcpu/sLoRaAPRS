/*
 * File: I2CScanner.h
 * Project: sLoRaAPRS
 * File Created: 2021-03-07 19:53
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-29 0:59
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_I2CSCANNER_H_
#define LIB_SLORAAPRS_I2CSCANNER_H_

#include <Arduino.h>
#include <LoRaAPRSConfig.h>
#include <TrackerDisplay.h>
#include <Wire.h>

#include <vector>

// class I2CScanner
// {
//   public:
//     vector<byte> devices;
//     byte device_count = 0;
//     bool scannend = false;
//     void scan();
//     //bool initialize();

// };

byte* ic2scan(byte* devices);
void ic2init_devices();

#endif  // LIB_SLORAAPRS_I2CSCANNER_H_
