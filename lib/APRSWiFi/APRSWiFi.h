#ifndef APRS_WIFI_H
#define APRS_WIFI_H


#include <Arduino.h>
#ifdef ESP32
#include <AsyncTCP.h>
#include <ESPmDNS.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <WiFiAP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#endif


void WifiAPInit(void);
void WIFIAP_start(void);
void WIFIAP_stop();
void WifiConnect(void);
void WifiDisconnect(void);





#endif
