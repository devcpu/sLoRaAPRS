/*
 * File: LoRaAPRSConfig.h
 * Project: config
 * File Created: 2020-09-26 21:08
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-29 1:21
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#ifndef LIB_CONFIG_LORAAPRSCONFIG_H_
#define LIB_CONFIG_LORAAPRSCONFIG_H_

#include <Arduino.h>

// @FIXME set to 1000
#define DISPLAY_DELAY_SHORT 30
// @FIXME set to 3000
#define DISPLAY_DELAY_MEDIUM 30
// @FIXME set to 5000
#define DISPLAY_DELAY_LONG 30

/**
 * @brief how long ervery info is shown in display change mode
 *
 */
#define DISPLAY_KINO_TIME 5000

// use this for newer Boards AKA Rev1 (second board release)
#define T_BEAM_V1_0
// or
// #define T_BEAM_V0_7
// use this for older Boards AKA Rev0 (first board release)

#define GPS_BAUD 9600  // GPS

/* * * * * WX sensors * * * * */
#define WX_DHT_SENSOR

#ifdef WX_DHT_SENSOR
// pin the DHT22 is connected to Pin25
#define DHTPIN 25
#endif

#define WX_BMC280_SENSOR

#ifdef WX_BMC280_SENSOR
#define WX_BMC280_SENSOR_ADDR 0x76
#endif

#define SENSOR_UPDATE_INTERVAL 1000 * 60 * 20

// update every xx in meter recommended is 1.300 km
#define APRS_UPDATE_DISTANCE 500
#define APRS_UPDATE_ANGLE 10
#define APRS_UPDATE_INTERVAL 1000 * 60 * 10

/********** LoRa params **********/

#define LoRaSpreadingFactor 12
#define LoRaSignalBandwidth 125000
#define LoRaCodingRate4 5
#define LoRaenableCrc true

#define LoRaTXFREQ 433775000  // Transmit frequency in MHz
#define LoRaTXdbmW 12         // Transmit power in dBm PA needs 23 /18 normal
#define LoRaTXenablePA 0  // switch internal power amplifier on (1) or off (0)

#define LoRaRXFREQ 433775000  // Transmit frequency in MHz

#define LoRaCsPin 18     // LoRa radio chip select
#define LoRaResetPin 23  // LoRa radio reset
#define LoRaIRQPin \
  26  // change for your board; must be a hardware interrupt pin

/********** LoRa params end **********/

/********** AXP192 power controler params end **********/
/*
AXP192_SLAVE_ADDRESS 0x34 comes from <axp20x.h>
*/

#ifdef ESP32
#define SDA 21
#define SCL 22
#elif defined(ESP8266)
#define SDA 21
#define SCL 22
#endif

static const uint32_t GPSBaud = 9600;

// GPS UART
#ifdef T_BEAM_V1_0
#define RXPin 12
#define TXPin 34
#else
#define RXPin 15
#define TXPin 12
#endif

// LED for signalling
#ifdef T_BEAM_V1_0
#define TXLED 33  // pin number for LED on TX Tracker
#else
#define TXLED 14  // pin number for LED on TX Tracker
#endif

// Button of TTGO T-Beam
// @CHANGEME
// 2 for my test board
#ifdef T_BEAM_V1_0
#define BUTTON 2  // GPIO number for Button on TTGO T-Beam
#else
#define BUTTON 39  // pin number for Button on TTGO T-Beam
#endif

#define OLED_RESET_PIN 4
#define OLED_H_SIZE 128
#define OLED_V_SIZE 64
#define OLED_I2C_ADDR 0x3D

#endif  // LIB_CONFIG_LORAAPRSCONFIG_H_
