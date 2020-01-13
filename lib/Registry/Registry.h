#ifndef APRS_REGISTRY_H
#define APRS_REGISTRY_H

#include <Arduino.h>

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

struct PositionData {
  double latitude;
  double longitude;
  double altitude;
};

struct Credentials {
  String auth_name;
  String auth_tocken;
};

struct Registry {
  String Version = "";
  String Release = "";
  boolean newTxMesg = false;
  boolean newRxMesg = false;
  
  String call = "CHANGEME";
  String wx_call_ext = "3";
  String aprs_call_ext = "9";
  String wxCall(void) { return call + "-" + wx_call_ext; };
  String aprsCall(void) { return call + "-" + aprs_call_ext; };

  Credentials WifiCrendentials[4];
  Credentials APCredentials;
  Credentials WebCredentials;
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
  PositionData pos;
  PositionData posfix;
  String SERVER_IP;
  APRSMessage TxMsg;
  APRSMessage RxMsg;
  WXData WXdata;
};

  // String LngFIX = "01357.56E";
  // String LatFIX = "5248.47N";


void RegistryInit(void);

void registryWriteInit(void);






String getPrefsString(const char* key);
void setPrefsString(const char* key, String  value);

uint16_t getPrefsInt(String key);
void setPrefsUInt(const char* key, uint16_t value);

double getPrefsDouble(const char *key);
void setPrefsDouble(const char *key, double value);

char getPrefsChar(String key);
void setPrefsChar(const char* key, char value);

void RegistryToString(void);

String getRunMode();
String getWifiMode();

#endif