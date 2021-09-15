/*
 * File: I2CScanner.cpp
 * Project: sLoRaAPRS
 * File Created: 2021-03-07 19:53
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-12 19:38
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include "I2CScanner.h"
#include "AXP20xDevice.h"

/**
 * @brief scans  all I2C-addresses (0-127) and print founds
 *
 */
void I2CScanner::scan() {
  // byte* ic2scan(byte* devices) {
  byte device_count = 0;
  Serial.println();
  Serial.println("I2C scanner. Scanning ...");

  // the scanner didn't found AXP-Device

#ifdef T_BEAM_V1_0
  devices.push_back(52);
  device_count++;
#endif

  for (byte i = 58; i < 62; i++) {
    Wire.beginTransmission(i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0) {  // Receive 0 = success (ACK response)
      Serial.print("Found device on address ");
      Serial.print("DEC: ");
      Serial.print(i, DEC);
      Serial.print(" HEX: ");
      Serial.print("(0x");
      Serial.print(i, HEX);
      Serial.println(")");

      devices.push_back(i);
      device_count++;
      Serial.println();
    } else {
      if (Wire.endTransmission() == 4) {
        Serial.print("Found error");
      }
    }
    delay(500);
  }
  Serial.print("Found ");
  Serial.print(device_count, DEC);  // numbers of devices
  Serial.println(" device(s).");
  scannend = true;
}

/**
 * @brief Initialize founded devices, print errors on console if errors
 *
 */
void I2CScanner::initialize() {
  if (!devices.empty()) {
    Serial.println("Mist");
    for (std::vector<byte>::iterator it = devices.begin(); it != devices.end();
         ++it) {
      switch (*it) {
        // AXP192
        case 52:
          Serial.println("Init for address 0x34");
          Serial.print("\tTry to initialize AXP Device ..");
          if (initAXP()) {
            Serial.println("+OK");
            cfg.hardware.AXP192 = true;
          } else {
            Serial.println("ERROR :no AXP Device found!");
            cfg.hardware.AXP192 = false;
          }
          // break;
        case 60:
          Serial.println("Init for address 0x3C");
          Serial.print("\tTry to initialize OLED Display ...");
          if (DisplayInit()) {
            Serial.println("+OK");
            cfg.hardware.OLED = true;
          } else {
            Serial.println("ERROR! Can't initialize OLED Display");
            cfg.hardware.OLED = false;
          }
          break;

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
          break;
        case 79:
          Serial.println("Init for 0x40 - 0x4F");
          Serial.println("\tTODO: init for INA219 not implemented yet");
          break;
        case 118:
        case 119:
          Serial.println("Init for 0x77");
          Serial.println("\tTODO: Init for BME280 not implemented yet");
          break;

        default:
          break;
      }
    }
  }
}
