#ifndef APRS_REGISTRY_H
#define APRS_REGISTRY_H

#include <Arduino.h>

#define EEPROM_USER_LENGTH 16
#define EEPROM_PASSWORD_LENGTH 32
#define EEPROM_SIZE 640

#define EEPROM_ADDR_SYSTEM 0
#define EEPROM_ADDR_VERSION 10
#define EEPROM_ADDR_BOOT_COUNT EEPROM_ADDR_VERSION + 25
#define EEPROM_ADDR_CALL EEPROM_ADDR_BOOT_COUNT + 6
#define EEPROM_ADDR_WX_EXT EEPROM_ADDR_CALL + 12
#define EEPROM_ADDR_APRS_EXT EEPROM_ADDR_WX_EXT + 3
#define EEPROM_ADDR_APRS_SYMBOL EEPROM_ADDR_APRS_EXT + 3
#define EEPROM_ADDR_APRS_PASSWORD EEPROM_ADDR_APRS_SYMBOL + 2

#define EEPROM_ADDR_APRS_SERVER EEPROM_ADDR_APRS_PASSWORD + 16  

#define EEPROM_ADDR_LAT_FIX EEPROM_ADDR_APRS_SERVER + 100 // IPv6!
#define EEPROM_ADDR_LNG_FIX EEPROM_ADDR_LAT_FIX + 10

#define EEPROM_ADDR_CURRENT_WIFI_MODE  EEPROM_ADDR_LNG_FIX + 10
#define EEPROM_ADDR_CURRENT_SYSTEM_MODE EEPROM_ADDR_CURRENT_WIFI_MODE + 2

#define EEPROM_ADDR_WEB_CREDENTIAL00 EEPROM_ADDR_CURRENT_SYSTEM_MODE + 2
#define EEPROM_ADDR_WEB_CREDENTIAL01 \
  EEPROM_ADDR_WEB_CREDENTIAL00 + EEPROM_USER_LENGTH

#define EEPROM_ADDR_AP_CREDENTIAL00 \
  EEPROM_ADDR_WEB_CREDENTIAL01 + EEPROM_PASSWORD_LENGTH
#define EEPROM_ADDR_AP_CREDENTIAL01 \
  EEPROM_ADDR_AP_CREDENTIAL00 + EEPROM_USER_LENGTH

#define EEPROM_ADDR_WIFI_CREDENTIAL00 \
  EEPROM_ADDR_AP_CREDENTIAL01 + EEPROM_PASSWORD_LENGTH
#define EEPROM_ADDR_WIFI_CREDENTIAL01 \
  EEPROM_ADDR_WIFI_CREDENTIAL00 + EEPROM_USER_LENGTH

#define EEPROM_ADDR_WIFI_CREDENTIAL10 \
  EEPROM_ADDR_WIFI_CREDENTIAL01 + EEPROM_PASSWORD_LENGTH
#define EEPROM_ADDR_WIFI_CREDENTIAL11 \
  EEPROM_ADDR_WIFI_CREDENTIAL10 + EEPROM_USER_LENGTH

#define EEPROM_ADDR_WIFI_CREDENTIAL20 \
  EEPROM_ADDR_WIFI_CREDENTIAL11 + EEPROM_PASSWORD_LENGTH
#define EEPROM_ADDR_WIFI_CREDENTIAL21 \
  EEPROM_ADDR_WIFI_CREDENTIAL20 + EEPROM_USER_LENGTH

#define EEPROM_ADDR_WIFI_CREDENTIAL30 \
  EEPROM_ADDR_WIFI_CREDENTIAL21 + EEPROM_PASSWORD_LENGTH
#define EEPROM_ADDR_WIFI_CREDENTIAL31 \
  EEPROM_ADDR_WIFI_CREDENTIAL30 + EEPROM_USER_LENGTH

#define SYSTEM_STRING "sLoRaAPRS"


enum wifi_mode { 
  wifi_off, 
  wifi_ap, 
  wifi_client 
};

enum system_mode {
  mode_tracker,
  mode_wxtracker,
  mode_wxfix,
  mode_repeater,
  mode_gateway,
  mode_repeater_gateway
};

struct APRSMessage {
  String msg;
  String gateways;
  String to;
  boolean processed;
  boolean newmessage;
};

struct WXData {
  float temp = 0;
  float pressure = 0;
  float humidity = 0;
};

typedef struct {
  boolean newTxMesg = false;
  boolean newRxMesg = false;
  String Call = "CHANGEME";
  String wx_call_ext = "3";
  String aprs_call_ext = "9";
  String wxCall(void) { return Call + "-" + wx_call_ext; };
  String aprsCall(void) { return Call + "-" + aprs_call_ext; };
  String WifiCrendentials[4][2];
  String APCredentials[2];
  String WebCredentials[2];
  char aprs_symbol = '[';
  String APRSPassword;
  String APRSServer[2];
  wifi_mode current_wifi_mode = wifi_ap;
  system_mode current_system_mode = mode_tracker;
  boolean new_mode = false;
  uint32_t boot_count = 0;
  boolean wx_sensor_bme280 = false;
  boolean wx_sensor_dht = false;
  boolean gps = false;
  boolean oled = false;
  String LngFIX = "01357.56E";
  String LatFIX = "5248.47N";
  String SERVER_IP;
  APRSMessage TxMsg;
  APRSMessage RxMsg;
  WXData WXdata;
} Registry;

void RegistryInit(void);

void EEPROMwriteString(uint16_t addr, String data);
String EEPROMreadString(uint16_t addr);
String EEPROMrestoreString(uint16_t addr, const char* def,
                               const char* name);
void EEPROMClear(void);
void dumpEEPROM(void);

#endif