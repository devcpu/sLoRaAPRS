/*
 * File: ESPFS.cpp
 * Project: sLoRaAPRS
 * File Created: 2020-11-11 20:13
 * Author: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de)
 * -----
 * Last Modified: 2021-09-18 22:23
 * Modified By: (DL7UXA) Johannes G.  Arlt (dl7uxa@arltus.de>)
 * -----
 * Copyright © 2019 - 2021 (DL7UXA) Johannes G.  Arlt
 * License: MIT License  http://www.opensource.org/licenses/MIT
 */

#include <Arduino.h>

#ifdef ESP32
#include <SPIFFS.h>
#elif defined(ESP8266)
#include <FS.h>
#endif

boolean ESPFSInit() {
  bool initok = false;
  // #ifdef ESP32
  //   initok = SPIFFS.begin(true);
  // #elif defined(ESP8266)
  initok = SPIFFS.begin();
  if (!(initok)) {  // Format SPIFS, of not formatted. - Try 1
    ESP_LOGI(TAG, "Format SPIFFS (try 1). Please wait ... ");
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (!(initok)) {  // Format SPIFS, of not formatted. - Try 2
    ESP_LOGW(TAG, "Format SPIFFS (try 2). Please wait ... ");
    SPIFFS.format();
    initok = SPIFFS.begin();
  }
  if (!initok) {
    ESP_LOGE(TAG, "SPIFFS ist nicht OK");
  }
  // #endif
  return initok;
}
