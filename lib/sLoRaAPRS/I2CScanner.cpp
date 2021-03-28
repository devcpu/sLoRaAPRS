/*
 * File: I2CScanner.cpp
 * Project: sLoRaAPRS
 * File Created: 2021-03-07 19:53
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-29 0:52
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include "I2CScanner.h"
#include "AXP20xDevice.h"

// void I2CScanner::scan() {
byte* ic2scan(byte* devices) {
  byte device_count = 0;
  Serial.println();
  Serial.println("I2C scanner. Scanning ...");

  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);        // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0) {  // Receive 0 = success (ACK response)
      Serial.print("Found device on address ");
      Serial.print("DEC: ");
      Serial.print(i, DEC);
      Serial.print(" HEX: ");
      Serial.print("(0x");
      Serial.print(i, HEX);
      Serial.println(")");

      // devices.push_back(i);
      devices[device_count] = i;
      device_count++;
    }
  }
  Serial.print("Found ");
  Serial.print(device_count, DEC);  // numbers of devices
  Serial.println(" device(s).");
  return devices;
}

void ic2init_devices(byte* devices) {
  for (int i = 0; i < 10; i++) {
    switch (devices[i]) {
      // AXP192
      case 52:
        if (initAXP()) {
          write3Line("Init AXP", " AXP 192", "   +OK", true,
                     DISPLA_DELAY_SHORT);
          // reg.hardware.AXP192 = true;
        } else {
          write3Line("Init AXP", "   -ERR", "check wire", true,
                     DISPLA_DELAY_LONG);
          // reg.hardware.AXP192 = false;
        }

      // INA219 High-Side DC Current/Voltage sensor (0x40 - 0x4F)
      case 64:
      case 65:
      case 66:
      case 67:
      case 68:
      case 69:
      case 70:
      case 71:
      case 72:
      case 73:
      case 74:
      case 75:
      case 76:
      case 77:
      case 78:
      case 79:
        Serial.println("Found INA219");

      case 118:
      case 119:
        Serial.println("Found BME280");

        break;

      default:
        break;
    }
  }
}
