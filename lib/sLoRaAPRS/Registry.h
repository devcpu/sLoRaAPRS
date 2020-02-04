#ifndef APRS_REGISTRY_H
#define APRS_REGISTRY_H

#include <Arduino.h>
#include "APRSControler.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"



void RegistryInit(void);

void registryWriteInit(void);

String reg_wxCall(void);

String reg_aprsCall(void);

String getPrefsString(const char* key);
void setPrefsString(const char* key, String  value);

uint16_t getPrefsInt(String key);
void setPrefsUInt(const char* key, uint16_t value);

double getPrefsDouble(const char *key);
void setPrefsDouble(const char *key, double value);

char getPrefsChar(const char *key);
void setPrefsChar(const char* key, char value);

void RegistryToString(void);

String getRunMode();
String getWifiMode();

#endif