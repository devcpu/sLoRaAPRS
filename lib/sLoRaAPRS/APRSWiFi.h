/*
 * File: APRSWiFi.h
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-17 18:15
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_SLORAAPRS_APRSWIFI_H_
#define LIB_SLORAAPRS_APRSWIFI_H_

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

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);

#endif // LIB_SLORAAPRS_APRSWIFI_H_
