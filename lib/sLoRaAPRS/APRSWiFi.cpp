/*
 * File: APRSWiFi.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-13 1:26
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt 
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <APRSWiFi.h>
#include <Arduino.h>
#include <LoRaAPRSConfig.h>
#include <Config.h>
#include <TrackerDisplay.h>
#include <apptypes.h>

extern Config cfg;

#ifdef ESP32
#elif defined(ESP8266)
#endif

String APRSWiFI_SSID = "";

void WifiAPInit(void) {
  WifiDisconnect();  // after flash it will not connect without it
  /* You can remove the password parameter if you want the AP to be open. */
  Serial.println("Configuring access point...");
#ifdef ESP32
  // WiFi.softAP(cfg.APCredentials.auth_name.c_str(),
  // cfg.APCredentials.auth_tocken.c_str());
  WiFi.softAP("sLoRaAPRS", "letmein42"); // FIXME from reg!
  Serial.println("WiFi.softAP");
// @TODO sollte hier nicht nötig sein!
#elif defined(ESP8266)
  WiFi.softAP(cfg.APCredentials[0], cfg.APCredentials[1]);
#endif

  IPAddress myIP = WiFi.softAPIP();
  cfg.SERVER_IP = myIP.toString();
  Serial.println("got IP");
  if (cfg.call == "CHANGEME") {
    // //@FIXME print to OLED to!
    Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
    Serial.print("++++++++++   AP IP address: ");
    Serial.print(myIP);
    Serial.println("    ++++++++++");
    Serial.println("++++++++++   SSID: " + cfg.APCredentials.auth_name +
                   "               ++++++++++");
    Serial.println("++++++++++   Password: " + cfg.APCredentials.auth_tocken +
                   "         ++++++++++");
    Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
    write3Line(" WIFI AP", cfg.APCredentials.auth_name.c_str(),
               cfg.APCredentials.auth_tocken.c_str(), false, 3000);
  } else {
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Serial.println("SSID: " + cfg.APCredentials.auth_name);
    write3Line(" WIFI AP", "  SSID:", cfg.APCredentials.auth_name.c_str(), true,
               3000);
  }
  cfg.lan_status.SSID = WiFi.SSID();
  cfg.lan_status.IP = myIP.toString();
  cfg.lan_status.status = "running";
  cfg.lan_status.mode = wifi_ap;
}

void WifiConnect(void) {
  // WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WifiDisconnect();  // after flash it will not connect without it

  // WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.println("Trying Connecting to WiFi ..");
  WiFi.begin(cfg.WifiCrendentials[0].auth_name.c_str(),
             cfg.WifiCrendentials[0].auth_tocken.c_str());

  // @TODO
  uint8_t count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    count++;
    delay(3000);
    Serial.print("Try again connecting to WiFi ");
    Serial.print(count);
    Serial.println(". try");
  }

  Serial.println(WiFi.localIP());
  IPAddress myIP = WiFi.localIP();
  cfg.SERVER_IP = myIP.toString();
  write3Line("WiFiclient", APRSWiFI_SSID.c_str(), myIP.toString().c_str(), true,
             3000);

  cfg.lan_status.SSID = WiFi.SSID();
  cfg.lan_status.IP = myIP.toString();
  cfg.lan_status.status = "connected";
  cfg.lan_status.mode = wifi_client;
}

void WifiDisconnect(void) {
  Serial.println("disconnect wifi");
#ifdef ESP8266
  delay(100);
  ETS_UART_INTR_DISABLE();
  wifi_station_disconnect();
  delay(100);
  ETS_UART_INTR_ENABLE();
#endif
#ifdef ESP8266
  esp_wifi_disconnect();
#endif
  delay(100);
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  Serial.println("Connected to AP!");

  char buf[32] = {0};

  Serial.print("SSID Length: ");
  Serial.println(info.connected.ssid_len);

  Serial.print("SSID: ");
  for (int i = 0; i < info.connected.ssid_len; i++) {
    Serial.print(static_cast<char>(info.connected.ssid[i]));
    buf[i] = static_cast<char>(info.connected.ssid[i]);
  }
  APRSWiFI_SSID = buf;

  Serial.print("\nBSSID: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", info.connected.bssid[i]);

    if (i < 5) {
      Serial.print(":");
    }
  }

  Serial.print("\nChannel: ");
  Serial.println(info.connected.channel);

  Serial.print("Auth mode: ");
  Serial.println(info.connected.authmode);
}
