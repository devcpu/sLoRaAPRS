#include <uxa_debug.h>
#include <LoRaAPRSConfig.h>
#include <Registry.h>
#include <Preferences.h>


Registry reg;


void RegistryInit(void) {

  reg.controler.button_state = new ButtonNeutral();

  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  
  reg.Version = preferences.getString(PREFS_VERSION);
  reg.Release = preferences.getString(PREFS_RELASE);

  reg.boot_count = preferences.getULong64(PREFS_BOOT_COUNT, 1ULL);
  reg.boot_count++;
  Serial.printf("write boot_count: %d\n", reg.boot_count);
  preferences.putULong64(PREFS_BOOT_COUNT, reg.boot_count);

  reg.current_wifi_mode = (wifi_mode) preferences.getUInt(PREFS_CURRENT_WIFI_MODE, int(wifi_ap));
  reg.current_system_mode = (system_mode) preferences.getUInt(PREFS_CURRENT_SYSTEM_MODE, int(mode_tracker));
  
  
  reg.call = preferences.getString(PREFS_CALL, CHANGE_ME);
  reg.aprs_call_ext = preferences.getString(PREFS_APRS_CALL_EX, "3");
  reg.wx_call_ext = preferences.getString(PREFS_WX_CALL_EX, "13");
  reg.aprs_symbol.symbol = preferences.getChar(PREFS_APRS_SYMBOL, '[');

  reg.WebCredentials.auth_name = preferences.getString(PREFS_WEB_ADMIN, "admin");
  reg.WebCredentials.auth_tocken = preferences.getString(PREFS_WEB_PASS, DEFAULT_PASSWORD);
  
  reg.APCredentials.auth_name = preferences.getString(PREFS_AP_SSID, "sLoRaAPRS");
  reg.APCredentials.auth_tocken = preferences.getString(PREFS_AP_PASS, DEFAULT_PASSWORD);
  
  reg.APRSPassword = preferences.getString(PREFS_APRS_PASSWORD, "12345");
  reg.APRSServer[0] = preferences.getString(PREFS_APRS_SERVER0, "192.168.168.192");
  reg.APRSServer[0] = preferences.getString(PREFS_APRS_SERVER0, "192.168.168.192");

  reg.posfix.latitude = preferences.getDouble(PREFS_POS_LAT_FIX, 0ULL);
  reg.posfix.longitude = preferences.getDouble(PREFS_POS_LNG_FIX, 0ULL);
  reg.posfix.altitude = preferences.getDouble(PREFS_POS_ALT_FIX, 0ULL);

  char ssid_name_buf[12] = {0};
  char auth_name_buf[12] = {0};
  uint8_t x = 0;
  for (x = 0; x < 4; x++) {
    snprintf(ssid_name_buf, 12, "lan%d_ssid", x);
    snprintf(auth_name_buf, 12, "lan%d_auth", x);
    // Serial.printf("ssid_name_buf: %s\n", ssid_name_buf);
    // Serial.printf("auth_name_buf: %s\n", auth_name_buf);
    reg.WifiCrendentials[x].auth_name = preferences.getString(ssid_name_buf, CHANGE_ME);
    reg.WifiCrendentials[x].auth_tocken = preferences.getString(auth_name_buf, DEFAULT_PASSWORD);
    // Serial.printf("index: %d key: %s value: %s\n", x, ssid_name_buf, reg.WifiCrendentials[x].auth_name.c_str());
    // Serial.printf("index: %d key: %s value: %s\n\n", x, auth_name_buf, reg.WifiCrendentials[x].auth_tocken.c_str());
  }

  preferences.end();
  
  if (reg.call == "CHANGEME") {
    reg.current_wifi_mode = wifi_ap;
    reg.current_system_mode = mode_tracker;
    registryWriteInit();
  }
#ifdef MODDEBUG
  //RegistryToString();
#endif
  Serial.println("Registry init ready\n");
}


void registryWriteInit(void){
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);

  preferences.putString(PREFS_VERSION, "0.0.1 preAlpha");
  preferences.putString(PREFS_RELASE, "Gehasi");

  
  preferences.putUInt(PREFS_CURRENT_WIFI_MODE, (uint8_t) reg.current_wifi_mode);
  preferences.putUInt(PREFS_CURRENT_SYSTEM_MODE, reg.current_system_mode);
   
  preferences.putString(PREFS_CALL, reg.call);
  preferences.putString(PREFS_APRS_CALL_EX, reg.aprs_call_ext);
  preferences.putString(PREFS_WX_CALL_EX, reg.wx_call_ext);
  preferences.putChar(PREFS_APRS_SYMBOL, reg.aprs_symbol.symbol);

  preferences.putString(PREFS_WEB_ADMIN, reg.WebCredentials.auth_name);
  preferences.putString(PREFS_WEB_PASS, reg.WebCredentials.auth_tocken);
  
  preferences.putString(PREFS_AP_SSID, reg.APCredentials.auth_name);
  preferences.putString(PREFS_AP_PASS, reg.APCredentials.auth_tocken);
  
  preferences.putString(PREFS_LAN0_SSID, reg.WifiCrendentials[0].auth_name);
  preferences.putString(PREFS_LAN0_AUTH, reg.WifiCrendentials[0].auth_tocken);
  preferences.putString(PREFS_LAN1_SSID, reg.WifiCrendentials[1].auth_name);
  preferences.putString(PREFS_LAN1_AUTH, reg.WifiCrendentials[1].auth_tocken);
  preferences.putString(PREFS_LAN2_SSID, reg.WifiCrendentials[2].auth_name);
  preferences.putString(PREFS_LAN2_AUTH, reg.WifiCrendentials[2].auth_tocken);
  preferences.putString(PREFS_LAN3_SSID, reg.WifiCrendentials[3].auth_name);
  preferences.putString(PREFS_LAN3_AUTH, reg.WifiCrendentials[3].auth_tocken);
  
  
  preferences.putDouble(PREFS_POS_LAT_FIX, reg.posfix.latitude);
  preferences.putDouble(PREFS_POS_LNG_FIX, reg.posfix.longitude);
  preferences.putDouble(PREFS_POS_ALT_FIX, reg.posfix.altitude);

  preferences.putString(PREFS_APRS_PASSWORD, reg.APRSPassword);
  preferences.putString(PREFS_APRS_SERVER0, reg.APRSServer[0]);
  preferences.putString(PREFS_APRS_SERVER1, reg.APRSServer[1]);
  
  preferences.end();
};

/* * * * * * * * * * * * * * * * * * * * * */

String getPrefsString(const char* key) {
  Preferences preferences;
  String retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getString(key, "-- -- --");
  preferences.end();
  return retvar;
}

uint16_t getPrefsInt(const char* key) {
  Preferences preferences;
  uint16_t retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getUInt(key, 0);
  preferences.end();
  return retvar;
  
}

char getPrefsChar(const char* key) {
  Preferences preferences;
  char retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getChar(key, 'a');
  preferences.end();
  return retvar;
}

double getPrefsDouble(const char *key) {
  Preferences preferences;
  double retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getDouble(key, 0);
  preferences.end();
  return retvar;
}


/* * * * * * * * * * * * * * * * * * * * * */

void setPrefsString(const char* key, String  value){
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  delay(500);
  Serial.println("2");
  String old_value = getPrefsString(key);
  Serial.println(old_value);
  if (old_value != value) {
    Serial.printf("write %s\n", value.c_str());
    long rv =  preferences.putString(key, value);
    Serial.printf("write of key=%s value=%s returns: %lu\n", key, value.c_str(), rv);
    String new_var = preferences.getString(key);
    Serial.printf("got var from NVS: %s\n", new_var.c_str());

  }
  preferences.end();
}


void setPrefsUInt(const char* key, uint16_t value) {
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  uint16_t old_value = getPrefsInt(key);
  DDE("new value: %d", value);
  DDE("old value: %d", old_value);
  if (old_value != value) {
    preferences.putUInt(key, value);
    DDD("update");
  }
  preferences.end();
  DDD("setPrefsUInt END");
}


void setPrefsDouble(const char *key, double value) {
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  double old_value = getPrefsDouble(key);
  DDE("old_val", String(old_value));
  if (old_value != value) {
    DDE(String(key), String(value));
    preferences.putDouble(key, value);
  }
  
  preferences.end();
}



void setPrefsChar(const char* key, char value) {
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  char old_value = getPrefsChar(key);
  Serial.printf("CharOLDValue=%c\n", old_value);
  Serial.printf("CharNEWValue=%c\n", value);
  if (old_value != value) {
    Serial.printf("wite new Char\n");
    preferences.putChar(key, value);
  }
  
  preferences.end();
}

void RegistryToString(void) {
  Serial.printf("boot_count: %d\n", reg.boot_count);
  Serial.printf("Version: %s\n", reg.Version.c_str());
  Serial.printf("Release: %s\n", reg.Release.c_str());

  Serial.printf("call: %s\n", reg.call.c_str());
  Serial.printf("aprs_call_ext: %s\n", reg.aprs_call_ext.c_str());
  Serial.printf("wx_call_ext: %s\n", reg.wx_call_ext.c_str());

  Serial.printf("run_mode: %d\n", (int)reg.current_system_mode);
  Serial.printf("wifi_mode: %d\n", (int)reg.current_wifi_mode);

  Serial.printf("web admin %s\n", reg.WebCredentials.auth_name.c_str());
  Serial.printf("web pass %s\n", reg.WebCredentials.auth_tocken.c_str());

  Serial.printf("AP SSID: %s\n", reg.APCredentials.auth_name.c_str());
  Serial.printf("AP pass: %s\n", reg.APCredentials.auth_tocken.c_str());

  Serial.printf("lan0_ssid: %s\n", reg.WifiCrendentials[0].auth_name.c_str());
  Serial.printf("lan0_auth: %s\n", reg.WifiCrendentials[0].auth_tocken.c_str());

  Serial.printf("lan1_ssid: %s\n", reg.WifiCrendentials[1].auth_name.c_str());
  Serial.printf("lan1_auth: %s\n", reg.WifiCrendentials[1].auth_tocken.c_str());

  Serial.printf("lan2_ssid: %s\n", reg.WifiCrendentials[2].auth_name.c_str());
  Serial.printf("lan2_auth: %s\n", reg.WifiCrendentials[2].auth_tocken.c_str());

  Serial.printf("lan3_ssid: %s\n", reg.WifiCrendentials[3].auth_name.c_str());
  Serial.printf("lan3_auth: %s\n", reg.WifiCrendentials[3].auth_tocken.c_str());

  Serial.printf("APRS Server 0: %s\n", reg.APRSServer[0].c_str());
  Serial.printf("APRS Server 1: %s\n", reg.APRSServer[1].c_str());
  Serial.printf("APRS Pass: %s\n", reg.APRSPassword.c_str());

  Serial.printf("Lat Fix %3.8f\n", reg.posfix.latitude);
  Serial.printf("Lng Fix %3.8f\n", reg.posfix.longitude);
  Serial.printf("Alt Fix %3.8f\n", reg.posfix.altitude);

};


String getWifiMode() {
  switch (reg.current_wifi_mode)
  {
  case wifi_off:
    return String("WiFi OFF");
    break;
  
  case wifi_ap:
    return String("WiFi AP");
    break;
  
  case wifi_client:
    return String("WiFi Client");
  default:
    return String(" no mode");
    break;
  }
}

String getRunMode() {
  switch (reg.current_system_mode)
  {
  case mode_tracker:
    return String(" tracker");
    break;

  case mode_wxtracker:
    return String("WX tracker");
    break;

  case mode_wxfix:
    return String("  WX FIX");
    break;

  case mode_repeater:
    return String(" repeater");
    break;

  case mode_gateway:
    return String(" Gateway");
    break;

  case mode_repeater_gateway:
    return String(" RPT & GW");
    break;

  default:
    return String(" no mode");
    break;
  }
}

String reg_wxCall(void) { return reg.call + String("-") + reg.wx_call_ext; };
String reg_aprsCall(void) { return reg.call + String("-") + reg.aprs_call_ext; };

