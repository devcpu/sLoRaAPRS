#ifndef MAIN_H
#define MAIN_H

#include <APRSWiFi.h>
#include <APRS_MSG.h>
#include <Arduino.h>
#include <Config.h>
#include <ESPFS.h>
#include <LoRaAPRSConfig.h>
#include <TinyGPS++.h>
#include <apptypes.h>

#define DISPLAY_DELAY_SHORT 100
#define DISPLAY_DELAY_MEDIUM 300
#define DISPLAY_DELAY_LONG 500

/*
#ifdef T_BEAM_V1_0
  #include <axp20x.h>
  AXP20X_Class axp;
#endif
*/

#ifdef WX_DHT_SENSOR
#include <DHTesp.h>
DHTesp dht;
#endif

// @TODO ifdef GPS
HardwareSerial ss(1);  // TTGO has HW serial to GPS // 1 = first UART
extern void WifiDisconnect(void);

bool initAXP();
static void smartDelay(uint32_t ms);

void button_tick(TimerHandle_t xExpiredTimer);
void button_tick(void);

void restart(void);
static void watchdog(void);

void Scanner(void);
#endif
