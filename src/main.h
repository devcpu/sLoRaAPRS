#ifndef MAIN_H
#define MAIN_H

#include <Registry.h>
#include <Arduino.h>
#include <APRSWiFi.h>
#include <ESPFS.h>
#include <APRS_MSG.h>
#include <TinyGPS++.h>
#include <LoRaAPRSConfig.h>

#define DISPLA_DELAY_SHORT 1000
#define DISPLA_DELAY_MEDIUM 3000
#define DISPLA_DELAY_LONG 5000

#ifdef T_BEAM_V1_0
  #include <axp20x.h>
  AXP20X_Class axp;
#endif

#ifdef WX_DHT_SENSOR
   #include <DHTesp.h>
  DHTesp dht;
#endif

// @TODO ifdef GPS
HardwareSerial ss(1);        // TTGO has HW serial to GPS // 1 = first UART
uint8_t setAllGPSData();

extern void WifiDisconnect(void);

bool initAXP();
static void smartDelay(uint32_t ms);


void restart(void);
static void watchdog(void);



void Scanner(void);
#endif