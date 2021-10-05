/*
 * File: I2CScanner.cpp
 * Project: sLoRaAPRS
 * File Created: 2021-03-07 19:53
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-03 22:04
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include "I2CScanner.h"
#include "AXP20xDevice.h"

extern TrackerDisplay td;

/**
 * @brief scans  all I2C-addresses (0-127) and print founds
 *
 */
void I2CScanner::scan() {
  // byte* ic2scan(byte* devices) {

  byte device_count = 0;
  ESP_LOGW(TAG, "scanning I2C bus ...");

  // the scanner didn't found AXP-Device

#ifdef T_BEAM_V1_0
  devices.push_back(52);
  device_count++;
#endif

  for (byte i = 58; i < 62; i++) {
    Wire.beginTransmission(i);         // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0) { // Receive 0 = success (ACK response)
      ESP_LOGW(TAG, "found I2C-dev at DEC: %d HEX: (0x%02X)", i, i);
      devices.push_back(i);
      device_count++;
    } else {
      if (Wire.endTransmission() == 4) {
        ESP_LOGD(TAG, "I2C error on %02X", i);
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  ESP_LOGI(TAG, "found %d I2C devices", device_count);
  scannend = true;
}

/**
 * @brief Initialize founded devices, print errors on console if errors
 *
 */
void I2CScanner::initialize() {
  if (!devices.empty()) {
    for (std::vector<byte>::iterator it = devices.begin(); it != devices.end(); ++it) {
      switch (*it) {
      // AXP192
      case 52:
        ESP_LOGD(TAG, "Init for address 0x34");
        ESP_LOGI(TAG, "\tTry to initialize AXP Device at 0x%02X", 52);
        if (initAXP()) {
          ESP_LOGD(TAG, "\t\t+OK");
          cfg.hardware.AXP192 = true;
        } else {
          ESP_LOGE(TAG, "ERROR :no AXP Device found!");
          cfg.hardware.AXP192 = false;
        }
        break;

      case 60:
        ESP_LOGD(TAG, "Init for address 0x%02X", 60);
        ESP_LOGI(TAG, "\tTry to initialize OLED Display at 0x%02X", 60);
        if (td.begin(60)) {
          ESP_LOGI(TAG, "\t\t+OK");
          cfg.hardware.OLED = true;
        } else {
          ESP_LOGE(TAG, "ERROR! Can't initialize OLED Display");
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
        ESP_LOGD(TAG, "Init for 0x40 - 0x4F");
        ESP_LOGD(TAG, "\tTODO: init for INA219 not implemented yet");
        break;
      case 118:
      case 119:
        ESP_LOGD(TAG, "Init for 0x77");
        ESP_LOGD(TAG, "\tTODO: Init for BME280 not implemented yet");
        break;

      default:
        break;
      }
    }
  }
}
