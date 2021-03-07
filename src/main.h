/**
* main.h
* @author bulbasaur
* @description 
* @created 2020-02-05T11:21:21.897Z+01:00
* @copyright None 
* None
* @last-modified 2021-03-07T16:42:06.978Z+01:00
*/

#ifndef MAIN_H
#define MAIN_H

#include <apptypes.h>
#include <Registry.h>
#include <Arduino.h>
#include <APRSWiFi.h>
#include <ESPFS.h>
#include <APRS_MSG.h>
#include <TinyGPS++.h>
#include <LoRaAPRSConfig.h>

#define DISPLA_DELAY_SHORT 100
#define DISPLA_DELAY_MEDIUM 300
#define DISPLA_DELAY_LONG 500

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
extern void WifiDisconnect(void);

bool initAXP();
static void smartDelay(uint32_t ms);

void button_tick(TimerHandle_t xExpiredTimer);
void button_tick(void);

void restart(void);
static void watchdog(void);



void Scanner(void);
#endif