#ifndef MAIN_H
#define MAIN_H

#include <Registry.h>
#include <Arduino.h>
#include <APRSWiFi.h>
#include <ESPFS.h>
#include <APRS_MSG.h>
#include <TinyGPS++.h>
#include <LoRaAPRSConfig.h>
#include <APRSWebServer.h>
#include <APRSControler.h>
#include <ButtonState.h>
#include <AsyncTCP.h>
#include <BMEHandler.h>
#include <Esp.h>
#include <GPSSensor.h>
#include <LoRaHandler.h>
#include <OneButtonHandler.h>
#include <SPI.h>
#include <TrackerDisplay.h>
#include <Wire.h>
#include <fap.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include <iGate.h>


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


void restart(void);
static void watchdog(void);



void Scanner(void);
#endif