#ifndef LORA_ARPS_CONFIG_H
#define LORA_ARPS_CONFIG_H

#define T_BEAM_V1_0    // use this for older Boards AKA Rev1 (second board release)
// #define T_BEAM_V0_7    // use this for older Boards AKA Rev0 (first board release)


#define GPS_BAUD 9600 //GPS

/* * * * * WX sensors * * * * */
#define WX_DHT_SENSOR


#ifdef WX_DHT_SENSOR
    #define DHTPIN 25            // pin the DHT22 is connected to Pin25
#endif    

#define WX_BMC280_SENSOR

#ifdef WX_BMC280_SENSOR
#define WX_BMC280_SENSOR_ADDR 0x76
#endif

/********** LoRa params **********/

#define LoRaSpreadingFactor 12
#define LoRaSignalBandwidth 125000
#define LoRaCodingRate4 5
#define LoRaenableCrc true


#define LoRaTXFREQ  433775000      // Transmit frequency in MHz
#define LoRaTXdbmW  18           // Transmit power in dBm PA needs 23
#define LoRaTXenablePA  0        // switch internal power amplifier on (1) or off (0)

#define LoRaRXFREQ  433900000     // Transmit frequency in MHz

#define LoRaCsPin       18          // LoRa radio chip select
#define LoRaResetPin    23       // LoRa radio reset
#define LoRaIRQPin      26         // change for your board; must be a hardware interrupt pin

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
   #define TXLED  33      //pin number for LED on TX Tracker
#else
   #define TXLED 14      //pin number for LED on TX Tracker
 #endif

// Button of TTGO T-Beam
#ifdef T_BEAM_V1_0
   #define BUTTON 2  // 38     //pin number for Button on TTGO T-Beam
#else
   #define BUTTON 39  //pin number for Button on TTGO T-Beam
#endif


#define OLED_RESET_PIN 4
#define OLED_H_SIZE 128
#define OLED_V_SIZE 64
#define OLED_I2C_ADDR 0x3D

#endif