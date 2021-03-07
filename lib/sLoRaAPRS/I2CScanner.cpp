/**
* I2CScanner.cpp
* @author Johannes Arlt
* @description Implements a simple I2C-Scanner to find all deviceses on I2C bus
* @created 2021-03-07
* @copyright © 2021 Johannes Arlt
* @last-modified 2021-03-07
*/
#include "I2CScanner.h"

void I2CScanner::scan() {
  Serial.println();
  Serial.println("I2C scanner. Scanning ...");

  for (byte i = 8; i < 120; i++) {
    Wire.beginTransmission(i);        // Begin I2C transmission Address (i)
    if (Wire.endTransmission() == 0)  // Receive 0 = success (ACK response)
    {
      Serial.print("Found device on address ");
      Serial.print(" (0x");
      Serial.print(i, HEX);  // PCF8574 7 bit address
      Serial.println(")");
      device_count++;
      devices.push_back(i);
    }
  }
  Serial.print("Found ");
  Serial.print(device_count, DEC);  // numbers of devices
  Serial.println(" device(s).");
  scannend = true;
}

bool I2CScanner::initialize() {
    for (std::vector<byte>::size_type i = 0; i != devices.size(); ++i) {
        using namespace std;
        switch (devices[i])
        {
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
