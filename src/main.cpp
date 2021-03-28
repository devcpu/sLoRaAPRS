/*
 * File: main.cpp
 * Project: src
 * File Created: 2021-03-07 20:08
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-27 23:40
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt 
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */


#include <main.h>

void setup() {
  delay(2000);  
  Serial.begin(115200);
  Wire.begin(21, 22);

  if (Wire.begin(SDA, SCL)) {
    write3toSerial("Init I2C", "  System", "   +OK", DISPLA_DELAY_SHORT);
  } else {
    write3toSerial("Init I2C", "   -ERR", "check wire", DISPLA_DELAY_LONG);
  }

  Serial.println("Starte I2C Scanner");
  delay(1000);
  byte devices[10] = {0};
  ic2scan(devices);
  //ic2init_devices();

  // I2CScanner i2cscanner = I2CScanner();
  // i2cscanner.scan();
  // for (vector<byte>::size_type i = 0; i != i2cscanner.devices.size(); ++i) {
  //   Serial.println(i2cscanner.devices[i]);
  // }



}


void loop() {
}