/*
 * File: main.cpp
 * Project: src
 * File Created: 2021-03-07 20:08
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-09 23:25
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <main.h>


// TTGO has HW serial to GPS // 1 = first UART
HardwareSerial ss(1);

void setup() {
  delay(500);

  RegistryInit();

  

  /*****   Init misc device BEGIN   *****/
  Serial.begin(115200);

  Serial.println("\n--------------------------");
  Serial.printf("Version: %s\n", reg.Version.c_str());
  Serial.printf("Release: %s\n", reg.Release.c_str());
  Serial.printf("Boot Count: %d\n", reg.boot_count);
  Serial.println("--------------------------\n");

  if (ESPFSInit()) {
    Serial.println("ESP file system online");
  } else {
    Serial.println("ERROR: Problem init ESP file system");
  }

  // Init UART1 for GPS device
  ss.begin(GPS_BAUD, SERIAL_8N1, TXPin, RXPin);


  /*****   Init misc device END   *****/

  /*****   Init I2C Begin   *****/
  Serial.print("\nInitialize I2C ... ");
  if (Wire.begin(SDA, SCL)) {
    Serial.println("+OK");
  } else {
    Serial.println("-ERR");
  }
  Serial.print("Starte I2C Scanner");
  delay(100);
  I2CScanner i2csanner;
  i2csanner.scan();
  i2csanner.initialize();
  Serial.println();
  /*****   Init I2C End   *****/



  // keine Ahnung, aber muss als Letztes gemacht werden.
  if (LoRa_init()) {
    Serial.println("LoRa TRX online");
  } else {
    Serial.println("ERROR init LoRa TRX");
  }

  pinMode(TXLED, OUTPUT);


}

void loop() {}