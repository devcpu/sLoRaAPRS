/*
 * File: Config.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-15 12:47
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_CONFIG_H_
#define LIB_SLORAAPRS_CONFIG_H_

#include <Arduino.h>

#include <freertos/FreeRTOS.h>
#include "APRSControler.h"
#include "freertos/timers.h"

void ConfigInit(void);

void ConfigWriteInit(void);

String reg_wxCall(void);

String reg_aprsCall(void);

String getPrefsString(const char *key);
void setPrefsString(const char *key, String value);

uint16_t getPrefsInt(String key);
bool setPrefsUInt(const char *key, uint16_t value);

double getPrefsDouble(const char *key);
void setPrefsDouble(const char *key, double value);

char getPrefsChar(const char *key);
void setPrefsChar(const char *key, char value);

void ConfigToString(void);

String getRunMode();
String getWifiMode();

#endif  // LIB_SLORAAPRS_CONFIG_H_
