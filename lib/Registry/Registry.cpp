#include <EEPROM.h>
#include <LoRaAPRSConfig.h>
#include <Registry.h>
#include <Preferences.h>

Preferences preferences;
Registry reg;

void RegistryInit(void) {

  preferences.begin(NVS_APP_NAME_SPACE, false);
  
  reg.Version = preferences.getString(PREFS_VERSION, "");
  reg.boot_count = preferences.getULong64(PREFS_BOOT_COUNT, 0);
  reg.boot_count++;
  preferences.putUInt(PREFS_BOOT_COUNT, reg.boot_count);
  reg.current_wifi_mode = (wifi_mode) preferences.getUInt(PREFS_CURRENT_WIFI_MODE, int(wifi_ap));
  reg.current_system_mode = (system_mode) preferences.getUInt(PREFS_CURRENT_SYSTEM_MODE, int(mode_tracker));
  reg.aprs_symbol = preferences.getChar(PREFS_APRS_SYMBOL, '[');
  reg.call = preferences.getString(PREFS_CALL, CHANGE_ME);
  reg.aprs_call_ext = preferences.getString(PREFS_APRS_CALL_EX, "3");
  reg.wx_call_ext = preferences.getString(PREFS_WX_CALL_EX, "12");
  reg.WebCredentials.auth_name = preferences.getString(PREFS_WEB_ADMIN, "admin");
  reg.WebCredentials.auth_tocken = preferences.getString(PREFS_WEB_PASS, DEFAULT_PASSWORD);
  reg.APCredentials.auth_name = preferences.getString(PREFS_AP_SSID, "sLoRaAPRS");
  reg.APCredentials.auth_tocken = preferences.getString(PREFS_AP_PASS, DEFAULT_PASSWORD);
  reg.APRSPassword = preferences.getString(PREFS_APRS_PASSWORD, "12345");
  reg.APRSServer[0] = preferences.getString(PREFS_APRS_SERVER0, "192.168.168.192");
  reg.APRSServer[0] = preferences.getString(PREFS_APRS_SERVER0, "192.168.168.192");

  char ssid_name_buf[12] = {0};
  char auth_name_buf[12] = {0};
  for (uint8_t x = 0; x < 4; x++) {
    snprintf(ssid_name_buf, 12, "lan%d_ssid", x);
    snprintf(auth_name_buf, 12, "lan%d_auth", x);
    reg.WifiCrendentials[x].auth_name = preferences.getString(ssid_name_buf, CHANGE_ME);
    reg.WifiCrendentials[x].auth_tocken = preferences.getString(auth_name_buf, DEFAULT_PASSWORD);
  }
  preferences.end();
  
  if (reg.call == "CHANGEME") {
    reg.current_wifi_mode = wifi_ap;
    reg.current_system_mode = mode_tracker;
    registryWriteInit();
  }

  Serial.println("Registry init ready\n");
}


void registryWriteInit(void){
  setPrefsString(PREFS_VERSION, "0.0.1 prealpha");
  setPrefsString(PREFS_RELASE, "Gehasi");

  setPrefsUInt(PREFS_CURRENT_WIFI_MODE, (uint8_t) reg.current_wifi_mode);
  setPrefsUInt(PREFS_CURRENT_SYSTEM_MODE, reg.current_system_mode);
    
  setPrefsString(PREFS_CALL, reg.call);
  setPrefsString(PREFS_APRS_CALL_EX, reg.aprs_call_ext);
  setPrefsString(PREFS_WX_CALL_EX, reg.wx_call_ext);
  setPrefsChar(PREFS_APRS_SYMBOL, reg.aprs_symbol);

  setPrefsString(PREFS_WEB_ADMIN, reg.WebCredentials.auth_name);
  setPrefsString(PREFS_WEB_PASS, reg.WebCredentials.auth_tocken);
  
  setPrefsString(PREFS_AP_SSID, reg.APCredentials.auth_name);
  setPrefsString(PREFS_AP_PASS, reg.APCredentials.auth_tocken);
  
  setPrefsString(PREFS_LAN0_SSID, reg.WifiCrendentials[0].auth_name);
  setPrefsString(PREFS_LAN0_AUTH, reg.WifiCrendentials[0].auth_tocken);
  setPrefsString(PREFS_LAN1_SSID, reg.WifiCrendentials[0].auth_name);
  setPrefsString(PREFS_LAN1_AUTH, reg.WifiCrendentials[0].auth_tocken);
  setPrefsString(PREFS_LAN2_SSID, reg.WifiCrendentials[0].auth_name);
  setPrefsString(PREFS_LAN2_AUTH, reg.WifiCrendentials[0].auth_tocken);
  setPrefsString(PREFS_LAN3_SSID, reg.WifiCrendentials[0].auth_name);
  setPrefsString(PREFS_LAN3_AUTH, reg.WifiCrendentials[0].auth_tocken);
  
  
  setPrefsDouble(PREFS_POS_LAT_FIX, reg.posfix.latitude);
  setPrefsDouble(PREFS_POS_LNG_FIX, reg.posfix.longitude);
  setPrefsDouble(PREFS_POS_ALT_FIX, reg.posfix.altitude);

  setPrefsString(PREFS_APRS_PASSWORD, reg.APRSPassword);
  setPrefsString(PREFS_APRS_SERVER0, reg.APRSServer[0]);
  setPrefsString(PREFS_APRS_SERVER1, reg.APRSServer[1]);
  
  
};

/* * * * * * * * * * * * * * * * * * * * * */

String getPrefsString(String key) {
  return getPrefsString(key.c_str());
}

uint16_t getPrefsInt(String key) {
  return getPrefsInt(key.c_str());
}

char getPrefsChar(String key) {
  return getPrefsChar(key.c_str());
}

String getPrefsString(const char* key) {
  String retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getString(key, "-- -- --");
  preferences.end();
  return retvar;
}

uint16_t getPrefsInt(const char* key) {
  uint16_t retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getUInt(key, 0);
  preferences.end();
  return retvar;
  
}

char getPrefsChar(const char* key) {
  char retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getChar(key, 'a');
  preferences.end();
  return retvar;
}

double getPrefsDouble(const char *key) {
  double retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getDouble(key, 0);
  preferences.end();
  return retvar;
}

double getPrefsDouble(String key) {
  double retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getDouble(key.c_str(), 0);
  preferences.end();
  return retvar;
}


/* * * * * * * * * * * * * * * * * * * * * */

void setPrefsString(const char* key, char*  value){
  preferences.begin(NVS_APP_NAME_SPACE, false);
  preferences.putString(key, value);
  preferences.end();
}

void setPrefsString(const char* key, String value){
  setPrefsString(key, value.c_str());
}

void setPrefsString(String key, String value) {
  setPrefsString(key.c_str(), value.c_str());
}


void setPrefsUInt(const char* key, uint16_t value) {
  preferences.begin(NVS_APP_NAME_SPACE, false);
  preferences.putUInt(key, value);
  preferences.end();
}

void setPrefsUInt(const char* key, String value) {
  setPrefsUInt(key, value.toInt());
}

void setPrefsUInt(String key, uint16_t value) {
  setPrefsUInt(key.c_str(), value);
}

void setPrefsUInt(String key, String value) {
  setPrefsUInt(key.c_str(), value.toInt());
}

void setPrefsDouble(const char *key, double value) {
  preferences.begin(NVS_APP_NAME_SPACE, false);
  preferences.putDouble(key, value);
  preferences.end();
}

void setPrefsDouble(const char *key, String value) {
  setPrefsDouble(key, value.toDouble());

}

void setPrefsDouble(String key, String value) {
  setPrefsDouble(key.c_str(), value.toDouble());
}



void setPrefsChar(const char* key, char value) {
  preferences.begin(NVS_APP_NAME_SPACE, false);
  preferences.putChar(key, value);
  preferences.end();
}

void setPrefsChar(String key, String value) {
  setPrefsChar(key.c_str(), value.indexOf(0));
}


