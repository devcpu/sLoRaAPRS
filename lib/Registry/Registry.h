#ifndef APRS_REGISTRY_H
#define APRS_REGISTRY_H

#include <Arduino.h>
#include <APRSControler.h>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>



#define NVS_APP_NAME_SPACE "sla"
#define SYSTEM_STRING "sLoRaAPRS"
#define DEFAULT_PASSWORD "letmein42"
#define CHANGE_ME "CHANGEME"


#define PREFS_VERSION "Version"
#define PREFS_RELASE "Release"
#define PREFS_BOOT_COUNT "boot_count"
#define PREFS_CURRENT_WIFI_MODE "c_wifi_mode"
#define PREFS_CURRENT_SYSTEM_MODE "c_system_mode"
#define PREFS_CALL "call"
#define PREFS_APRS_SYMBOL "aprs_symbol"
#define PREFS_APRS_CALL_EX "aprs_ext"
#define PREFS_WX_CALL_EX "wx_ext"
#define PREFS_WEB_ADMIN "web_admin"
#define PREFS_WEB_PASS "web_pass"
#define PREFS_AP_SSID "ap_ssid"
#define PREFS_AP_PASS "ap_pass"
#define PREFS_APRS_PASSWORD "aprs_pass"
#define PREFS_APRS_SERVER0 "aprs_server0"
#define PREFS_APRS_SERVER1 "aprs_server1"
#define PREFS_LAN0_SSID "lan0_ssid"
#define PREFS_LAN0_AUTH "lan0_auth"
#define PREFS_LAN1_SSID "lan1_ssid"
#define PREFS_LAN1_AUTH "lan1_auth"
#define PREFS_LAN2_SSID "lan2_ssid"
#define PREFS_LAN2_AUTH "lan2_auth"
#define PREFS_LAN3_SSID "lan3_ssid"
#define PREFS_LAN3_AUTH "lan3_auth"
#define PREFS_POS_LAT_FIX "lat_fix"
#define PREFS_POS_LNG_FIX "lng_fix"
#define PREFS_POS_ALT_FIX "alt_fix"

#define MSG_FORM_TO "to"
#define MSG_FORM_WIDE "wide"
#define MSG_FORM_MSG "msg"


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
  String wide;
  String to;
  boolean processed;
  boolean newmessage;
};

struct WXData {
  float temp = 0;
  float pressure = 0;
  float humidity = 0;
};

struct Location {
  double latitude;
  double longitude;
  double altitude;
};

struct Move {
  double speed;
  double course;
};

struct DateTime {
  uint16_t year = 0;
  uint8_t month = 0;
  uint8_t day = 0;
  uint8_t hour = 0;
  uint8_t minute = 0;
  uint8_t second = 0;
 
};

struct GPSMeta {
  uint32_t sat = 0;
  double hdop = 0;
};

struct Credentials {
  String auth_name;
  String auth_tocken;
};

struct APRSSymbol {
  char table = '/';
  char symbol = '_';
};

struct HardWare {
  boolean OLED = false;
  boolean BME280 = false;
  boolean DHT22 = false;
  boolean AXP192 = false;
  boolean GPS = false;
};


struct LanStatus {
  wifi_mode mode = wifi_off;
  String SSID = "";
  String status = "";
  String IP = "";
};

struct OLEDMessage {
  boolean active = false;
  String head = "";
  String line1 = "";
  String line2 = "";
  String line3 = "";
  String line4 = "";
};

struct Registry {
  String Version = "";
  String Release = "";
  OLEDMessage oled_message;
  String call = "CHANGEME";
  String wx_call_ext = "3";
  String aprs_call_ext = "9";
  Credentials WifiCrendentials[4];
  Credentials APCredentials;
  Credentials WebCredentials;
  APRSSymbol aprs_symbol;
  String APRSPassword;
  String APRSServer[2];
  uint32_t boot_count = 0;

  wifi_mode current_wifi_mode = wifi_ap;
  system_mode current_system_mode = mode_tracker;
  
  LanStatus lan_status;
  
  HardWare hardware;

  Location gps_location;
  Move gps_move;
  DateTime gps_time;
  GPSMeta gps_meta;
  Location posfix;

  String SERVER_IP;
  APRSMessage TxMsg;
  APRSMessage RxMsg;
  WXData WXdata;
  APRSControler controler;
};


void singleClick_CB(void);
void doubleClick_CB(void);
void longClick_CB(void);
void kinoTimer_CB(TimerHandle_t xExpiredTimer);

void RegistryInit(void);

void registryWriteInit(void);

String reg_wxCall(void);

String reg_aprsCall(void);

String getPrefsString(const char* key);
void setPrefsString(const char* key, String  value);

uint16_t getPrefsInt(String key);
void setPrefsUInt(const char* key, uint16_t value);

double getPrefsDouble(const char *key);
void setPrefsDouble(const char *key, double value);

char getPrefsChar(const char *key);
void setPrefsChar(const char* key, char value);

void RegistryToString(void);

String getRunMode();
String getWifiMode();

#endif