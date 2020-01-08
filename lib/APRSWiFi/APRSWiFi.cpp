#include <APRSWiFi.h>
#include <LoRaAPRSConfig.h>
#include <Registry.h>


extern Registry reg; 

#ifdef ESP32
#elif defined(ESP8266)
#endif

void WifiAPInit(void) {
  WifiDisconnect();  // after flash it will not connect without
  /* You can remove the password parameter if you want the AP to be open. */
  Serial.println("Configuring access point...");
#ifdef ESP32
WiFi.softAP(reg.APCredentials[0].c_str(), reg.APCredentials[1].c_str());
Serial.println("WiFi.softAP");
// @TODO sollte hier nicht nötig sein!
#elif defined(ESP8266)
WiFi.softAP(reg.APCredentials[0], reg.APCredentials[1]);
#endif
  

  IPAddress myIP = WiFi.softAPIP();
  reg.SERVER_IP = myIP.toString();
  Serial.println("got IP");
  if (reg.Call == "CHANGEME") {
    // //@FIXME print to OLED to!
    Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
    Serial.print("++++++++++   AP IP address: ");
    Serial.print(myIP);
    Serial.println("    ++++++++++");
    Serial.println("++++++++++   SSID: " + reg.APCredentials[0] +
                   "               ++++++++++");
    Serial.println("++++++++++   Password: " + reg.APCredentials[1] +
                   "         ++++++++++");
    Serial.println("+++++++++++++++++++++++++++++++++++++++++++++++++++++");
  } else {
    Serial.print("AP IP address: ");
    Serial.println(myIP);
    Serial.println("SSID: " + reg.APCredentials[0]);
  }
}

void WifiConnect(void) {
  WifiDisconnect();  // after flash it will not connect without it

  // WiFi.mode(WIFI_STA);
  delay(1000);
  Serial.println("Trying Connecting to WiFi ..");
  WiFi.begin(reg.WifiCrendentials[0][0].c_str(), reg.WifiCrendentials[0][1].c_str());


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
  reg.SERVER_IP = myIP.toString();

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

