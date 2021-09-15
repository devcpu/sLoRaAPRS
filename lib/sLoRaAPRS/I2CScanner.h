/*
 * File: I2CScanner.h
 * Project: sLoRaAPRS
 * File Created: 2021-03-07 19:53
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-10 0:13
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

class I2CScanner {
 public:
  std::vector<byte> devices;
  byte device_count = 0;

  bool scannend = false;
  void scan();
  void initialize();
};

#endif  // LIB_SLORAAPRS_I2CSCANNER_H_
