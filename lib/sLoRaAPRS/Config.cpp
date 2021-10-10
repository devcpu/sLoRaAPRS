/*
 * File: Config.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-10-11 0:05
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <Config.h>
#include <LoRaAPRSConfig.h>
#include <Preferences.h>
#include <apptypes.h>

void ConfigInit(void) {

  Config cfg;
  cfg = taskScheduler.getConfig(cfg);
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);

  cfg.Version = preferences.getString(PREFS_VERSION);
  cfg.Release = preferences.getString(PREFS_RELASE);

  cfg.boot_count = preferences.getULong64(PREFS_BOOT_COUNT, 1ULL);
  // @FIXME
  //  cfg.boot_count++;
  //  Serial.printf("write boot_count: %d\n", cfg.boot_count);
  //  preferences.putULong64(PREFS_BOOT_COUNT, cfg.boot_count);

  cfg.current_wifi_mode =
      static_cast<wifi_mode>(preferences.getUInt(PREFS_CURRENT_WIFI_MODE, static_cast<int>(wifi_ap)));
  cfg.current_run_mode =
      static_cast<run_mode>(preferences.getUInt(PREFS_CURRENT_SYSTEM_MODE, static_cast<int>(mode_tracker)));

  cfg.call = preferences.getString(PREFS_CALL, CHANGE_ME);
  cfg.aprs_call_ext = preferences.getString(PREFS_APRS_CALL_EX, "3");
  cfg.wx_call_ext = preferences.getString(PREFS_WX_CALL_EX, "13");
  cfg.aprs_symbol = preferences.getString(PREFS_APRS_SYMBOL, "/[");

  cfg.WebCredentials.auth_name = preferences.getString(PREFS_WEB_ADMIN, "admin");
  cfg.WebCredentials.auth_tocken = preferences.getString(PREFS_WEB_PASS, DEFAULT_PASSWORD);

  cfg.APCredentials.auth_name = preferences.getString(PREFS_AP_SSID, "sLoRaAPRS");
  cfg.APCredentials.auth_tocken = preferences.getString(PREFS_AP_PASS, DEFAULT_PASSWORD);

  cfg.APRSPassword = preferences.getString(PREFS_APRS_PASSWORD, "12345");
  cfg.APRSServer[0] = preferences.getString(PREFS_APRS_SERVER0, "192.168.168.192");
  cfg.APRSServer[0] = preferences.getString(PREFS_APRS_SERVER0, "192.168.168.192");

  cfg.posfix.latitude = preferences.getDouble(PREFS_POS_LAT_FIX, 0ULL);
  cfg.posfix.longitude = preferences.getDouble(PREFS_POS_LNG_FIX, 0ULL);
  cfg.posfix.altitude = preferences.getDouble(PREFS_POS_ALT_FIX, 0ULL);

  char ssid_name_buf[12] = {0};
  char auth_name_buf[12] = {0};
  uint8_t x = 0;
  for (x = 0; x < 4; x++) {
    snprintf(ssid_name_buf, sizeof(ssid_name_buf), "lan%d_ssid", x);
    snprintf(auth_name_buf, sizeof(auth_name_buf), "lan%d_auth", x);
    // Serial.printf("ssid_name_buf: %s\n", ssid_name_buf);
    // Serial.printf("auth_name_buf: %s\n", auth_name_buf);
    cfg.WifiCrendentials[x].auth_name = preferences.getString(ssid_name_buf, CHANGE_ME);
    cfg.WifiCrendentials[x].auth_tocken = preferences.getString(auth_name_buf, DEFAULT_PASSWORD);
    // Serial.printf("index: %d key: %s value: %s\n", x, ssid_name_buf,
    // cfg.WifiCrendentials[x].auth_name.c_str()); Serial.printf("index: %d key:
    // %s value: %s\n\n", x, auth_name_buf,
    // cfg.WifiCrendentials[x].auth_tocken.c_str());
  }

  preferences.end();

  if (cfg.call == "CHANGEME") {
    cfg.current_wifi_mode = wifi_ap;
    cfg.current_run_mode = mode_tracker;
    ConfigWriteInit();
  }

  taskScheduler.setConfig(cfg);  
#ifdef MODDEBUG
  // ConfigToString();
#endif
  Serial.println("Config init ready\n");

}

void ConfigWriteInit(void) {
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);

  preferences.putString(PREFS_VERSION, "0.0.1 preAlpha");
  preferences.putString(PREFS_RELASE, "Gehasi");

  preferences.putUInt(PREFS_CURRENT_WIFI_MODE, (uint8_t)cfg.current_wifi_mode);
  preferences.putUInt(PREFS_CURRENT_SYSTEM_MODE, cfg.current_run_mode);

  preferences.putString(PREFS_CALL, cfg.call);
  preferences.putString(PREFS_APRS_CALL_EX, cfg.aprs_call_ext);
  preferences.putString(PREFS_WX_CALL_EX, cfg.wx_call_ext);
  preferences.putString(PREFS_APRS_SYMBOL, cfg.aprs_symbol);

  preferences.putString(PREFS_WEB_ADMIN, cfg.WebCredentials.auth_name);
  preferences.putString(PREFS_WEB_PASS, cfg.WebCredentials.auth_tocken);

  preferences.putString(PREFS_AP_SSID, cfg.APCredentials.auth_name);
  preferences.putString(PREFS_AP_PASS, cfg.APCredentials.auth_tocken);

  preferences.putString(PREFS_LAN0_SSID, cfg.WifiCrendentials[0].auth_name);
  preferences.putString(PREFS_LAN0_AUTH, cfg.WifiCrendentials[0].auth_tocken);
  preferences.putString(PREFS_LAN1_SSID, cfg.WifiCrendentials[1].auth_name);
  preferences.putString(PREFS_LAN1_AUTH, cfg.WifiCrendentials[1].auth_tocken);
  preferences.putString(PREFS_LAN2_SSID, cfg.WifiCrendentials[2].auth_name);
  preferences.putString(PREFS_LAN2_AUTH, cfg.WifiCrendentials[2].auth_tocken);
  preferences.putString(PREFS_LAN3_SSID, cfg.WifiCrendentials[3].auth_name);
  preferences.putString(PREFS_LAN3_AUTH, cfg.WifiCrendentials[3].auth_tocken);

  preferences.putDouble(PREFS_POS_LAT_FIX, cfg.posfix.latitude);
  preferences.putDouble(PREFS_POS_LNG_FIX, cfg.posfix.longitude);
  preferences.putDouble(PREFS_POS_ALT_FIX, cfg.posfix.altitude);

  preferences.putString(PREFS_APRS_PASSWORD, cfg.APRSPassword);
  preferences.putString(PREFS_APRS_SERVER0, cfg.APRSServer[0]);
  preferences.putString(PREFS_APRS_SERVER1, cfg.APRSServer[1]);

  preferences.end();
}

/* * * * * * * * * * * * * * * * * * * * * */

String getPrefsString(const char *key) {
  Preferences preferences;
  String retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getString(key, "-- -- --");
  preferences.end();
  return retvar;
}

uint16_t getPrefsInt(const char *key) {
  Preferences preferences;
  uint16_t retvar;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  retvar = preferences.getUInt(key, 0);
  preferences.end();
  return retvar;
}

char getPrefsChar(const char *key) {
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

void setPrefsString(const char *key, String value) {
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  delay(500);
  ESP_LOGD(TAG, "key=%s value=%s", key, value);
  String old_value = getPrefsString(key);
  ESP_LOGD(TAG, "old_value=%s", old_value);
  if (old_value != value) {
    Serial.printf("write %s\n", value.c_str());
    ESP_LOGD(TAG, "write %s", value);
    uint32_t rv = preferences.putString(key, value);
    ESP_LOGD(TAG, "write of key=%s value=%s returns: %lu\n", key, value.c_str(), rv);
    String new_var = preferences.getString(key);
    ESP_LOGD(TAG, "got var from NVS: %s\n", new_var.c_str());
  }
  preferences.end();
}

bool setPrefsUInt(const char *key, uint16_t value) {
  bool rv = false;
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  uint16_t old_value = getPrefsInt(key);
  ESP_LOGD(TAG, "old value = %d; got one = %d", old_value, value);
  if (old_value != value) {
    ESP_LOGD(TAG, "write %s=%d", key, value);
    preferences.putUInt(key, value);
    rv = true;
    ESP_LOGD(TAG, "update");
  }
  preferences.end();
  ESP_LOGD(TAG, "setPrefsUInt END");
  return rv;
}

void setPrefsDouble(const char *key, double value) {
  Preferences preferences;
  preferences.begin(NVS_APP_NAME_SPACE, false);
  double old_value = getPrefsDouble(key);
  ESP_LOGD(TAG, "old value = %l; got %l", old_value, value);
  if (old_value != value) {
    ESP_LOGD(TAG, "write %s=%l", key, value);
    preferences.putDouble(key, value);
  }

  preferences.end();
}

void setPrefsChar(const char *key, char value) {
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

#ifdef MODDEBUG
void ConfigToString(void) {
  Serial.printf("boot_count: %d\n", cfg.boot_count);
  Serial.printf("Version: %s\n", cfg.Version.c_str());
  Serial.printf("Release: %s\n", cfg.Release.c_str());

  Serial.printf("call: %s\n", cfg.call.c_str());
  Serial.printf("aprs_call_ext: %s\n", cfg.aprs_call_ext.c_str());
  Serial.printf("wx_call_ext: %s\n", cfg.wx_call_ext.c_str());

  Serial.printf("run_mode: %d\n", static_cast<int>(cfg.current_run_mode));
  Serial.printf("wifi_mode: %d\n", static_cast<int>(cfg.current_wifi_mode));

  Serial.printf("web admin %s\n", cfg.WebCredentials.auth_name.c_str());
  Serial.printf("web pass %s\n", cfg.WebCredentials.auth_tocken.c_str());

  Serial.printf("AP SSID: %s\n", cfg.APCredentials.auth_name.c_str());
  Serial.printf("AP pass: %s\n", cfg.APCredentials.auth_tocken.c_str());

  Serial.printf("lan0_ssid: %s\n", cfg.WifiCrendentials[0].auth_name.c_str());
  Serial.printf("lan0_auth: %s\n", cfg.WifiCrendentials[0].auth_tocken.c_str());

  Serial.printf("lan1_ssid: %s\n", cfg.WifiCrendentials[1].auth_name.c_str());
  Serial.printf("lan1_auth: %s\n", cfg.WifiCrendentials[1].auth_tocken.c_str());

  Serial.printf("lan2_ssid: %s\n", cfg.WifiCrendentials[2].auth_name.c_str());
  Serial.printf("lan2_auth: %s\n", cfg.WifiCrendentials[2].auth_tocken.c_str());

  Serial.printf("lan3_ssid: %s\n", cfg.WifiCrendentials[3].auth_name.c_str());
  Serial.printf("lan3_auth: %s\n", cfg.WifiCrendentials[3].auth_tocken.c_str());

  Serial.printf("APRS Server 0: %s\n", cfg.APRSServer[0].c_str());
  Serial.printf("APRS Server 1: %s\n", cfg.APRSServer[1].c_str());
  Serial.printf("APRS Pass: %s\n", cfg.APRSPassword.c_str());

  Serial.printf("Lat Fix %3.8f\n", cfg.posfix.latitude);
  Serial.printf("Lng Fix %3.8f\n", cfg.posfix.longitude);
  Serial.printf("Alt Fix %3.8f\n", cfg.posfix.altitude);
}
#endif

String getWifiMode() {
  switch (cfg.current_wifi_mode) {
  case wifi_off:
    return String("WiFi OFF");
    break;

  case wifi_ap:
    return String("WiFi AP");
    break;

  case wifi_client:
    return String("WiFi Client");
  default:
    return String("ERROR no mode");
    break;
  }
}

String getRunMode() {
  switch (cfg.current_run_mode) {
  case mode_tracker:
    return String(" tracker");
    break;

  case mode_wxtracker:
    return String("WX tracker");
    break;

  case mode_wxfix:
    return String("  WX FIX");
    break;

  case mode_digi:
    return String(" repeater");
    break;

  case mode_gateway:
    return String(" Gateway");
    break;

  case mode_digi_gateway:
    return String(" RPT & GW");
    break;

  default:
    return String(" no mode");
    break;
  }
}

String reg_wxCall(void) { return cfg.call + String("-") + cfg.wx_call_ext; }
String reg_aprsCall(void) { return cfg.call + String("-") + cfg.aprs_call_ext; }
