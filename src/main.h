#ifndef MAIN_H
#define MAIN_H

#include <Registry.h>
#include <Arduino.h>
#include <APRSWiFi.h>
#include <ESPFS.h>

#include <APRS_MSG.h>
#include <TinyGPS++.h>
#include <LoRaAPRSConfig.h>



#ifdef T_BEAM_V1_0
  #include <axp20x.h>
  AXP20X_Class axp;
#endif

#ifdef WX_DHT_SENSOR
   #include <DHTesp.h>
  DHTesp dht;
#endif

#define WX_BME280_SENSOR

#ifdef WX_BME280_SENSOR
   #define TINY_BME280_I2C
   #include <TinyBME280.h>
   tiny::BME280 wxSensor;
#endif


extern void WifiDisconnect(void);

// @TODO ifdef GPS
HardwareSerial ss(1);        // TTGO has HW serial to GPS // 1 = first UART

uint8_t setAllGPSData();
void initAXP();
static void smartDelay(uint32_t ms);


void restart(void);
static void watchdog(void);




#endif