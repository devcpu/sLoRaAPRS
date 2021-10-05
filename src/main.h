/*
 * File: main.h
 * Project: src
 * File Created: 2021-03-07 20:08
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-04 23:22
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include "freertos/FreeRTOS.h"
#include <Arduino.h>
// #include "esp_freertos_hooks.h"
#include <APRSWebServer.h>
#include <APRSWiFi.h>
#include <ESPFS.h>
#include <GPSSensor.h>
#include <I2CScanner.h>
#include <LoRaAPRSConfig.h>
#include <LoRaHandler.h>
#include <SPI.h>
#include <Scheduler.h>
#include <TrackerDisplay.h>
#include <xOneButton.h>

void f(char *s);

// static void watchdog(void);
#endif // SRC_MAIN_H_
