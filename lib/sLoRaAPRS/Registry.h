/*
 * File: Registry.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-03-29 1:29
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt 
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_REGISTRY_H_
#define LIB_SLORAAPRS_REGISTRY_H_

#include <Arduino.h>

#include "APRSControler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

void RegistryInit(void);

void registryWriteInit(void);

String reg_wxCall(void);

String reg_aprsCall(void);

String getPrefsString(const char* key);
void setPrefsString(const char* key, String value);

uint16_t getPrefsInt(String key);
void setPrefsUInt(const char* key, uint16_t value);

double getPrefsDouble(const char* key);
void setPrefsDouble(const char* key, double value);

char getPrefsChar(const char* key);
void setPrefsChar(const char* key, char value);

void RegistryToString(void);

String getRunMode();
String getWifiMode();

#endif  // LIB_SLORAAPRS_REGISTRY_H_
