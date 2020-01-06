#include <Arduino.h>

#ifdef ESP32
#include <SPIFFS.h>
#elif defined(ESP8266)
#include <FS.h>
#endif

boolean ESPFSInit() {
  bool initok = false;
#ifdef ESP32
  initok = SPIFFS.begin(true);
#elif defined(ESP8266)
  initok = SPIFFS.begin();
  if (!(initok))  // Format SPIFS, of not formatted. - Try 1
  {
    Serial.println("Format SPIFFS");
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (!(initok))  // Format SPIFS, of not formatted. - Try 2
  {
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (initok) {
    Serial.println("SPIFFS ist OK");
  } else {
    Serial.println("SPIFFS ist nicht OK");
  }
#endif
  return initok;
}
