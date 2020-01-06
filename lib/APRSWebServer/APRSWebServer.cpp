#define MODDEBUG
#ifdef MODDEBUG
#define DDD(x) Serial.printf("DDD>: %s::%d %s\n", __FILE__, __LINE__, String(x).c_str());
#else
#define DDD(x) ;
#endif

#include <APRSWebServer.h>
#include <APRSWiFi.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <AsyncJson.h>
#include <EEPROM.h>
#include <Registry.h>
#include <TinyGPS++.h>

#include <SPIFFSEditor.h>

#include <Ticker.h>
//#include "CallBackList.h"


// @TODO remove together with restart()

extern Registry reg;

AsyncWebServer *WebServer;
AsyncWebSocket *ws;
AsyncWebSocketClient *globalClient = NULL;

String mainmenue(
    "<form action='.' method='get'><button>Main Menue</button></form><br />");



struct HTML_Error {
  String ErrorMsg = "";
  boolean isSended = true;
};

HTML_Error html_error;


struct SendMsgForm {
  String to = "";
  String gateways = "";
  String msg = "";
  boolean via = false;
};

SendMsgForm send_msg_form_tmp;

// void onRequest(AsyncWebServerRequest *request) {
//   Serial.println("onRequest");
//   request->send(404);
// }

// void onBody(AsyncWebServerRequest *request, uint8_t *data, size_t len,
//             size_t index, size_t total) {
//   Serial.println("onBody");
// }

// void onUpload(AsyncWebServerRequest *request, String filename, size_t index,
//               uint8_t *data, size_t len, bool final) {
//   Serial.println("onUpload");
// }

// void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
//              AwsEventType type, void *arg, uint8_t *data, size_t len) {
//   Serial.println("onEvent");
// }

void WebserverStart(void) {
  Serial.println("starting Webserver");
  WebServer = new AsyncWebServer(80);
  ws = new AsyncWebSocket("/ws");
  void handleRequestChangeMode(AsyncWebServerRequest * request);
  String ProcessorChangeMode(const String &var);

  ws->onEvent(onWsEvent);
  WebServer->addHandler(ws);

  WebServer->onNotFound([](AsyncWebServerRequest *request) {
    showRequest(request);
    request->send(404);
  });

  WebServer->serveStatic("/sloraaprs.css", SPIFFS, "/sloraaprs.css");
  WebServer->serveStatic("/rebootinfo", SPIFFS, "/reboot.html");

  WebServer->on("/sloraaprs.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/sloraaprs.js");
    request->send(SPIFFS, "/sloraaprs.js", "application/javascript", false,
                  ProcessorJS);
  });

  WebServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/");
    request->send(SPIFFS, "/main.html", "text/html", false, ProcessorDefault);
  });

  // System Info
  WebServer->on("/si", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/si");
    request->send(SPIFFS, "/main.html", "text/html", false,
                  systemInfoProcessor);
  });

  // Configure Call
  WebServer->on("/cc", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/cc");
    showRequest(request);
    if (request->params() > 0) {
      DDD("/cc");
      handleRequestConfigCall(request);
      request->redirect("/");
    } else {
      request->send(SPIFFS, "/main.html", "text/html", false,
                    ProcessorConfigCall);
    }
  });

  // Send Message
  WebServer->on("/sm", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/sm");
    if (html_error.isSended) {
      resetHTMLError();
    }
    showRequest(request);
    if (request->params() > 0) {
      handleRequestSendMessage(request);
      request->redirect("/");
    }

    request->send(SPIFFS, "/main.html", "text/html", false,
                  ProcessorSendMessage);
  });

  // Change Mode
  WebServer->on("/cm", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/cm");
    if (html_error.isSended) {
      resetHTMLError();
    }
    showRequest(request);
    DDD("Change Mode");
    if (request->params() == 2) {
      DDD(request->params());
      handleRequestChangeMode(request);
      request->redirect("/");
    }

    request->send(SPIFFS, "/main.html", "text/html", false,
                  ProcessorChangeMode);
  });

  // GPS Info
  WebServer->on("/gi", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/gi");
    showRequest(request);
    request->send(SPIFFS, "/main.html", "text/html", false, ProcessorGPSInfo);
  });

  // WX Info
  WebServer->on("/wx", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/wx");
    showRequest(request);
    request->send(SPIFFS, "/main.html", "text/html", false, ProcessorWXInfo);
  });

  // Wifi AP
  WebServer->on("/ca", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/ca");
    showRequest(request);
    if (request->params()) {
      if (request->params() == 2) {
        handleRequestConfigAP(request);
        request->redirect("/");
      } else {
        Serial.println("ERR: wrong request");
      }
    }

    request->send(SPIFFS, "/main.html", "text/html", false,
                  ProcessorConfigWifiAP);
  });

  // Config Gateway
  WebServer->on("/cg", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/cg");
    if (html_error.isSended) {
      resetHTMLError();
    }
    showRequest(request);
    if (request->params()) {
      handleRequestConfigGateway(request);
      request->redirect("/");
    }
    request->send(SPIFFS, "/main.html", "text/html", false,
                  ProcessorConfigGateway);
  });

  // Config Gateway
  WebServer->on("/reboot", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/reboot");
    reboot(request);
  });

  // ConfigWLAN
  WebServer->on("/cl", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/cl");
    showRequest(request);
    if (request->params()) {
      if (request->params() == 2) {
        handleRequestConfigWLAN(request);
        request->redirect("/");
      } else {
        Serial.println("ERR: wrong request");
      }
    }
    request->send(SPIFFS, "/main.html", "text/html", false,
                  ProcessorConfigWLAN);
  });

  // Config Web Admin
  WebServer->on("/cw", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/cw");
    showRequest(request);
    // handle Request in /ca
  });

  WebServer->on("/bb", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/bb");
    showRequest(request);
    // no Processor !
  });

  WebServer->begin();
  Serial.println("HTTP WebServer started");
};

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Websocket client connection received");
    globalClient = client;

  } else if (type == WS_EVT_DISCONNECT) {
    Serial.println("Websocket client connection finished");
    globalClient = NULL;
  }
}

String ProcessorJS(const String &var) {
  if (var == "SERVER_IP") {
    DDD(reg.SERVER_IP);
    return reg.SERVER_IP;
  }

  return String("wrong placeholder " + var);
}

String ProcessorDefault(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("Main Menue");
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/mainbutton.html");
  }
  return String("wrong placeholder " + var);
}

String systemInfoProcessor(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("System Info");
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }


  if (var == "BODY") {
    return getSystemInfoTable();
  }

  return String("wrong placeholder " + var);
}

String getSystemInfoTable(void) {

// #ifdef ESP32
//   uint64_t chipid = chipid=ESP.getEfuseMac();//The chip ID is essentially its MAC address(length: 6 bytes).
// #endif


//  (ideSize != realSize) ? ""
  FlashMode_t ideMode = ESP.getFlashChipMode();

  String systemdata[][2] = {
      {"Build DateTime:", GetBuildDateAndTime()},
      {"SDKVersion", String(ESP.getSdkVersion())},
      {"BootCount:", String(reg.boot_count)},
      {"Uptime", String(millis() / 1000, DEC)}, 
#ifdef ESP32
      {"Chip Revision:", String(ESP.getChipRevision())},
      // {"ESP32 Chip ID:", String((uint16_t)chipid>>32, HEX) + String((uint32_t)chipid, HEX)},
      {"Reset Reason CPU0", getResetReason(rtc_get_reset_reason(0))},
      {"Reset Reason CPU1", getResetReason(rtc_get_reset_reason(1))},
      {"CpuFreqMHz", String(ESP.getCpuFreqMHz())},
      {"CycleCount", String(ESP.getCycleCount())},
      {"FlashChipMode", String(ESP.getFlashChipMode())},
      {"FlashChipSize", String(ESP.getFlashChipSize())},
      {"FlashChipSpeed", String(ESP.getFlashChipSpeed())},
      {"SketchSize", String(ESP.getSketchSize())},
      {"FreeSketchSpace", String(ESP.getFreeSketchSpace())},
      {"SketchMD5", String(ESP.getSketchMD5())},
      {"HeapSize", String(ESP.getHeapSize())},
      {"FreeHeap", String(ESP.getFreeHeap())},
      {"MaxAllocHeap", String(ESP.getMaxAllocHeap())},
      {"MinFreeHeap", String(ESP.getMinFreeHeap())},
      {"PsramSize", String(ESP.getPsramSize())},
      {"FreePsram", String(ESP.getFreePsram())},
      {"MaxAllocPsram", String(ESP.getMaxAllocPsram())},
      {"MinFreePsram", String(ESP.getMinFreePsram())},
#elif defined(ESP8266)
      {"Flash real id:", String(ESP.getFlashChipId(), HEX)},
      {"Flash real size:", String(ESP.getFlashChipRealSize() / 1024) + "kB"},
#endif
      // size = 5
      {"Flash ide  size:", String(ESP.getFlashChipSize() / 1024) + "kB"},
      {"Flash ide speed:",
       String(ESP.getFlashChipSpeed() / 1000 / 1000) + "MHz"},
      {"Flash ide mode:",
       String((ideMode == FM_QIO
                   ? "QIO"
                   : ideMode == FM_QOUT
                         ? "QOUT"
                         : ideMode == FM_DIO
                               ? "DIO"
                               : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"))},
      {"Sketch size:", String(ESP.getSketchSize() / 1024) + "kB"},
      // size = 10
      {"Free sketch size:", String(ESP.getFreeSketchSpace() / 1024) + "kB"},
      {"Free heap:", String(ESP.getFreeHeap() / 1024) + "kB"},
#ifdef ESP32
#elif defined(ESP8266)
      {"ResetReason", String(ESP.getResetReason())},
//      {"Chip Config Status:", String()},
#endif
      // 
      // {"", String()},
  };

  String hwdata[][2] = {
      {"GPS:", String(reg.gps)},
      {"OLED:", String(reg.oled)},
      {"DHT:", String(reg.wx_sensor_dht)},
      {"BME280", String(reg.wx_sensor_bme280)},
  };

  return table2DGenerator(systemdata, 30, true) +
         table2DGenerator(hwdata, 4, true) + "<br /><br />" + mainmenue;

  //   //     if (ideSize != realSize)
  //   //     {
  //   //         snprintf(flash_config_ok, 64, "<tr><td colspan=\"2\"><b>Flash
  //   Chip
  //   //         configuration wrong!</b></td></tr>");
  //   //     }
  //   //     else
  //   //     {
  //   //         snprintf(flash_config_ok, 64, "<tr><td colspan=\"2\">Flash
  //   Chip
  //   //         configuration ok. </td></tr>");
  //   //     }
}

#ifdef ESP32
String getResetReason(RESET_REASON reason)
{
  String retvar;
  switch ( reason)
  {
    case 1 :  retvar =  "POWERON_RESET";break;          /**<1, Vbat power on reset*/
    case 3 :  retvar =  "SW_RESET";break;               /**<3, Software reset digital core*/
    case 4 :  retvar =  "OWDT_RESET";break;             /**<4, Legacy watch dog reset digital core*/
    case 5 :  retvar =  "DEEPSLEEP_RESET";break;        /**<5, Deep Sleep reset digital core*/
    case 6 :  retvar =  "SDIO_RESET";break;             /**<6, Reset by SLC module, reset digital core*/
    case 7 :  retvar =  "TG0WDT_SYS_RESET";break;       /**<7, Timer Group0 Watch dog reset digital core*/
    case 8 :  retvar =  "TG1WDT_SYS_RESET";break;       /**<8, Timer Group1 Watch dog reset digital core*/
    case 9 :  retvar =  "RTCWDT_SYS_RESET";break;       /**<9, RTC Watch dog Reset digital core*/
    case 10 : retvar =  "INTRUSION_RESET";break;       /**<10, Instrusion tested to reset CPU*/
    case 11 : retvar =  "TGWDT_CPU_RESET";break;       /**<11, Time Group reset CPU*/
    case 12 : retvar =  "SW_CPU_RESET";break;          /**<12, Software reset CPU*/
    case 13 : retvar =  "RTCWDT_CPU_RESET";break;      /**<13, RTC Watch dog Reset CPU*/
    case 14 : retvar =  "EXT_CPU_RESET";break;         /**<14, for APP CPU, reseted by PRO CPU*/
    case 15 : retvar =  "RTCWDT_BROWN_OUT_RESET";break;/**<15, Reset when the vdd voltage is not stable*/
    case 16 : retvar =  "RTCWDT_RTC_RESET";break;      /**<16, RTC Watch dog reset digital core and rtc module*/
    default : retvar =  "NO_MEAN";
  }
  return retvar;
}
#endif



String ProcessorConfigCall(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("Config Call");
  }

  if (var == "CALL") {
    return reg.Call;
  }

  if (var == "LAT") {
    return reg.LatFIX;
  }

  if (var == "LNG") {
    return reg.LngFIX;
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "APRS_SYMBOL") {
    DDD(reg.aprs_symbol);
    return String(reg.aprs_symbol);
  }

  if (var == "APRS_SELECT") {
    return reg.aprs_call_ext;
  }

  if (var == "WX_SELECT") {
    DDD(reg.wx_call_ext);
    return reg.wx_call_ext;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigCall.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
};

void handleRequestConfigCall(AsyncWebServerRequest *request) {
  DDD("handleRequestConfigCall");
  if (request->hasParam("call")) {
    String new_call(request->getParam("call")->value());
    if (new_call.length() > 0 && new_call.length() < 16) {
      new_call.toUpperCase();
      Serial.println("write param 'call' " + new_call);
      EEPROMwriteString(EEPROM_ADDR_CALL, new_call);
      reg.Call = new_call;
    } else {
      Serial.println("ERR: call not valide! " + new_call);
    }
  }
  DDD("call +OK");
  if (request->hasParam("aprs_call_ext")) {
    String aprs_ext_x = request->getParam("aprs_call_ext")->value();
    if (validateNumber(aprs_ext_x)) {
      Serial.println("write param 'aprs_call_ext' " + aprs_ext_x);
      EEPROMwriteString(EEPROM_ADDR_APRS_EXT, aprs_ext_x);
      reg.aprs_call_ext = aprs_ext_x;
      Serial.println("aprs_call_ext fertig");
    } else {
      Serial.println("ERR: APRS ext not valide! " + aprs_ext_x);
    }
  }
  DDD("aprs_ext +OK");
  if (request->hasParam("wx_call_ext")) {
    String wx_call_ext_x = request->getParam("wx_call_ext")->value();
    if (validateNumber(wx_call_ext_x)) {
      Serial.println("write param 'wx_call_ext' " + wx_call_ext_x);
      EEPROMwriteString(EEPROM_ADDR_WX_EXT, wx_call_ext_x);
      reg.wx_call_ext = wx_call_ext_x;
    } else {
      Serial.println("ERR: WX ext not valide! " + wx_call_ext_x);
    }
  }
  DDD("wx_ext +OK");
  if (request->hasParam("aprs_symbol")) {
    String aprs_symbol_tmp = request->getParam("aprs_symbol")->value();
    if (aprs_symbol_tmp.length() == 1) {
      char aprs_symbol_x = aprs_symbol_tmp.charAt(0);
      Serial.println("write param 'aprs_symbol' " + aprs_symbol_tmp);
      EEPROMwriteString(EEPROM_ADDR_APRS_SYMBOL, aprs_symbol_tmp);
      reg.aprs_symbol = aprs_symbol_x;
    } else {
      Serial.println("ERR: APRS Symbol not valide!" + aprs_symbol_tmp);
    }
  }

////////////////////////////////////////////////////////////////////////////////

  if (request->hasParam("lat")) {
    DDD("param lat");
    String new_lat = request->getParam("lat")->value();
    if (new_lat.length() > 6) {
      Serial.println("write param 'lat' " + new_lat);
      EEPROMwriteString(EEPROM_ADDR_LAT_FIX, new_lat);
      reg.LatFIX = new_lat;
    } else {
      Serial.println("ERR: Lat not valide! " + new_lat);
    }
  }

  if (request->hasParam("lng")) {
    DDD("param lng");
    String new_lng = request->getParam("lng")->value();
    if (new_lng.length() > 6) {
      Serial.println("write param 'lng' " + new_lng);
      EEPROMwriteString(EEPROM_ADDR_LNG_FIX, new_lng);
      reg.LatFIX = new_lng;
    } else {
      Serial.println("ERR: Lat not valide! " + new_lng);
    }
  }


}

String ProcessorGPSInfo(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("GPS Info");
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/GPSInfo.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
}

String ProcessorWXInfo(const String& var){
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("GPS Info");
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/WXInfo.html") + mainmenue;
  }

  return String("wrong placeholder " + var);

};

String ProcessorConfigWifiAP(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("Config Wifi AP");
  }

  if (var == "APCredentials0") {
    return reg.APCredentials[0];
  }

  if (var == "APCredentials1") {
    return reg.APCredentials[1];
  }

  if (var == "WebCredentials0") {
    return reg.WebCredentials[0];
  }

  if (var == "WebCredentials1") {
    return reg.WebCredentials[1];
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigWifiAP.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
}

void handleRequestConfigAP(AsyncWebServerRequest *request) {
  if (request->hasParam("APCredentials0")) {
    String APCredentials0(request->getParam("APCredentials0")->value());
    String APCredentials1;
    if (request->hasParam("APCredentials1")) {
      APCredentials1 = request->getParam("APCredentials1")->value();
    }
    if (APCredentials0.length() > 0 &&
        APCredentials0.length() < EEPROM_USER_LENGTH &&
        APCredentials1.length() > 0 &&
        APCredentials1.length() < EEPROM_PASSWORD_LENGTH) {
      EEPROMwriteString(EEPROM_ADDR_AP_CREDENTIAL00, APCredentials0);
      EEPROMwriteString(EEPROM_ADDR_AP_CREDENTIAL01, APCredentials1);
      reg.APCredentials[0] = APCredentials0;
      reg.APCredentials[1] = APCredentials1;
    } else {
      Serial.println("ERR: SSID or Password valide! " + APCredentials0 + "/" +
                     APCredentials1);
    }
  }

  if (request->hasParam("WebCredentials0")) {
    String WebCredentials0(request->getParam("WebCredentials0")->value());
    String WebCredentials1;
    if (request->hasParam("WebCredentials1")) {
      WebCredentials1 = request->getParam("WebCredentials1")->value();
    }
    if (WebCredentials0.length() > 0 &&
        WebCredentials0.length() < EEPROM_USER_LENGTH &&
        WebCredentials1.length() > 0 &&
        WebCredentials1.length() < EEPROM_PASSWORD_LENGTH) {
      EEPROMwriteString(EEPROM_ADDR_WEB_CREDENTIAL00, WebCredentials0);
      EEPROMwriteString(EEPROM_ADDR_WEB_CREDENTIAL01, WebCredentials1);
      reg.WebCredentials[0] = WebCredentials0;
      reg.WebCredentials[1] = WebCredentials1;
    } else {
      Serial.println("ERR: SSID or Password valide! " + WebCredentials0 + "/" +
                     WebCredentials1);
    }
  }
}

String ProcessorConfigGateway(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("Config GateWay");
  }

  if (var == "APRSServer0") {
    return reg.APRSServer[0];
  }

  if (var == "APRSServer1") {
    return reg.APRSServer[1];
  }

  if (var == "APRSPassword") {
    return reg.APRSPassword;
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigGateway.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
}

void handleRequestConfigGateway(AsyncWebServerRequest *request) {
  DDD("start");
  String APRSServer0 = "";
  String APRSServer1 = "";
  String APRSPassword = "";
  if (request->hasParam("APRSServer0")) {
    APRSServer0 = request->getParam("APRSServer0")->value();
  }
  DDD("has APRSServer0");
  if (request->hasParam("APRSServer1")) {
    APRSServer1 = request->getParam("APRSServer1")->value();
  }
  DDD("has APRSServer1");
  if (request->hasParam("APRSPassword")) {
    APRSPassword = request->getParam("APRSPassword")->value();
  }
  DDD("has APRSPassword");
  EEPROMwriteString(EEPROM_ADDR_APRS_SERVER, APRSServer0);
  EEPROMwriteString(EEPROM_ADDR_APRS_SERVER + 50, APRSServer1);
  EEPROMwriteString(EEPROM_ADDR_APRS_PASSWORD, APRSPassword);
  reg.APRSServer[0] = APRSServer0;
  reg.APRSServer[1] = APRSServer1;
  reg.APRSPassword = APRSPassword;
}

String ProcessorSendMessage(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("Send APRS Message");
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "TO") {
    return send_msg_form_tmp.to;
  }

  if (var == "GATEWAY") {
    return send_msg_form_tmp.gateways;
  }

  if (var == "MSG") {
    return send_msg_form_tmp.msg;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormSendMessage.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
}

void handleRequestSendMessage(AsyncWebServerRequest *request) {
  String msg = "";
  String to = "";
  String gateways = "";
  if (request->hasParam("msg")) {
    msg = request->getParam("msg")->value();
  }
  if (request->hasParam("to")) {
    to = request->getParam("to")->value();
  }
  if (request->hasParam("gateways")) {
    gateways = request->getParam("gateways")->value();
  }

  if (msg.length() > 200) {
    send_msg_form_tmp.msg = msg;
    if (gateways && gateways.length() > 3) {
      send_msg_form_tmp.gateways = gateways;
    }
    send_msg_form_tmp.to = to;
    return HTMLSendError("Message to long, please try again", request);
  }
  reg.TxMsg.msg = msg;
  reg.TxMsg.newmessage = true;
  if (gateways.length() > 4) {
    reg.TxMsg.gateways = gateways;
  }
  reg.TxMsg.to = to;
};

String ProcessorConfigWLAN(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("Configure WLAN");
  }

  if (var == "WifiCrendentials00") {
    Serial.println(reg.WifiCrendentials[0][0]);
    return reg.WifiCrendentials[0][0];
  }

  if (var == "WifiCrendentials01") {
    Serial.println(reg.WifiCrendentials[0][1]);
    return reg.WifiCrendentials[0][1];
  }

  if (var == "WifiCrendentials10") {
    Serial.println(reg.WifiCrendentials[1][0]);
    return reg.WifiCrendentials[1][0];
  }

  if (var == "WifiCrendentials11") {
    Serial.println(reg.WifiCrendentials[1][1]);
    return reg.WifiCrendentials[1][1];
  }

  if (var == "WifiCrendentials20") {
    Serial.println(reg.WifiCrendentials[2][0]);
    return reg.WifiCrendentials[2][0];
  }

  if (var == "WifiCrendentials21") {
    Serial.println(reg.WifiCrendentials[2][1]);
    return reg.WifiCrendentials[2][1];
  }

  if (var == "WifiCrendentials30") {
    Serial.println(reg.WifiCrendentials[3][0]);
    return reg.WifiCrendentials[3][0];
  }

  if (var == "WifiCrendentials31") {
    Serial.println(reg.WifiCrendentials[3][1]);
    return reg.WifiCrendentials[3][1];
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigWLAN.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
};

void handleRequestConfigWLAN(AsyncWebServerRequest *request) {
  if (request->hasParam("WifiCrendentials00")) {
    String WifiCrendentials00(request->getParam("WifiCrendentials00")->value());
    String WifiCrendentials01;
    if (request->hasParam("WifiCrendentials01")) {
      WifiCrendentials01 = request->getParam("WifiCrendentials01")->value();
    }
    if (WifiCrendentials00.length() > 0 &&
        WifiCrendentials00.length() < EEPROM_USER_LENGTH &&
        WifiCrendentials01.length() > 0 &&
        WifiCrendentials01.length() < EEPROM_PASSWORD_LENGTH) {
      EEPROMwriteString(EEPROM_ADDR_WIFI_CREDENTIAL00, WifiCrendentials00);
      EEPROMwriteString(EEPROM_ADDR_WIFI_CREDENTIAL01, WifiCrendentials01);
      reg.WifiCrendentials[0][0] = WifiCrendentials00;
      reg.WifiCrendentials[0][1] = WifiCrendentials01;
    } else {
      Serial.println("ERR: SSID or Password valide! " + WifiCrendentials00 +
                     "/" + WifiCrendentials01);
    }
  }

  if (request->hasParam("WifiCrendentials10")) {
    String WifiCrendentials10(request->getParam("WifiCrendentials10")->value());
    String WifiCrendentials11;
    if (request->hasParam("WifiCrendentials11")) {
      WifiCrendentials11 = request->getParam("WifiCrendentials11")->value();
    }
    if (WifiCrendentials10.length() > 0 &&
        WifiCrendentials10.length() < EEPROM_USER_LENGTH &&
        WifiCrendentials11.length() > 0 &&
        WifiCrendentials11.length() < EEPROM_PASSWORD_LENGTH) {
      EEPROMwriteString(EEPROM_ADDR_WIFI_CREDENTIAL10, WifiCrendentials10);
      EEPROMwriteString(EEPROM_ADDR_WIFI_CREDENTIAL11, WifiCrendentials11);
      reg.WifiCrendentials[1][0] = WifiCrendentials10;
      reg.WifiCrendentials[1][1] = WifiCrendentials11;
    } else {
      Serial.println("ERR: SSID or Password valide! " + WifiCrendentials10 +
                     "/" + WifiCrendentials11);
    }
  }

  if (request->hasParam("WifiCrendentials20")) {
    String WifiCrendentials20(request->getParam("WifiCrendentials20")->value());
    String WifiCrendentials21;
    if (request->hasParam("WifiCrendentials21")) {
      WifiCrendentials21 = request->getParam("WifiCrendentials21")->value();
    }
    if (WifiCrendentials20.length() > 0 &&
        WifiCrendentials20.length() < EEPROM_USER_LENGTH &&
        WifiCrendentials21.length() > 0 &&
        WifiCrendentials21.length() < EEPROM_PASSWORD_LENGTH) {
      EEPROMwriteString(EEPROM_ADDR_WIFI_CREDENTIAL20, WifiCrendentials20);
      EEPROMwriteString(EEPROM_ADDR_WIFI_CREDENTIAL21, WifiCrendentials21);
      reg.WifiCrendentials[2][0] = WifiCrendentials20;
      reg.WifiCrendentials[2][1] = WifiCrendentials21;
    } else {
      Serial.println("ERR: SSID or Password valide! " + WifiCrendentials20 +
                     "/" + WifiCrendentials21);
    }
  }

  if (request->hasParam("WifiCrendentials30")) {
    String WifiCrendentials30(request->getParam("WifiCrendentials30")->value());
    String WifiCrendentials31;
    if (request->hasParam("WifiCrendentials31")) {
      WifiCrendentials31 = request->getParam("WifiCrendentials31")->value();
    }
    if (WifiCrendentials30.length() > 0 &&
        WifiCrendentials30.length() < EEPROM_USER_LENGTH &&
        WifiCrendentials31.length() > 0 &&
        WifiCrendentials31.length() < EEPROM_PASSWORD_LENGTH) {
      EEPROMwriteString(EEPROM_ADDR_WIFI_CREDENTIAL30, WifiCrendentials30);
      EEPROMwriteString(EEPROM_ADDR_WIFI_CREDENTIAL31, WifiCrendentials31);
      reg.WifiCrendentials[3][0] = WifiCrendentials30;
      reg.WifiCrendentials[3][1] = WifiCrendentials31;
    } else {
      Serial.println("ERR: SSID or Password valide! " + WifiCrendentials30 +
                     "/" + WifiCrendentials31);
    }
  }
};

String ProcessorChangeMode(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.Call;
  }

  if (var == "H2TITLE") {
    return String("Configure Run Mode & Wifi Mode");
  }

  if (var == "ERRORMSG") {
    html_error.isSended = true;
    return html_error.ErrorMsg;
  }

  if (var == "SELECT_RUNMODE") {
    // enum system_mode {mode_tracker, mode_wxtracker, mode_wxfix,
    // mode_repeater, mode_gateway, mode_repeater_gateway};
    String options[] = {"GPS Tracker",
                        "WX Tracker",
                        "WX Fix Position (no GPS)",
                        "APRS LoRa Repeater",
                        "APRS Gateway",
                        "APRS LoRa Repeater & APRS Gateway"};

    return optionsFeldGenerator(reg.current_system_mode, "new_system_mode",
                                options, 6);
  }

  // enum wifi_mode {wifi_off, wifi_ap, wifi_client};
  if (var == "SELECT_WIFIMODE") {
    String options[] = {
        "Wifi OFF",
        "Wifi AP",
        "WLAN Connect",
    };

    return optionsFeldGenerator(reg.current_wifi_mode, "new_wifi_mode", options,
                                3);
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigMode.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
};

void handleRequestChangeMode(AsyncWebServerRequest *request) {
  DDD("handleRequestChangeMode");
  String new_system_mode = "";
  String new_wifi_mode = "";
  if (request->hasParam("new_system_mode")) {
    new_system_mode = request->getParam("new_system_mode")->value();
  }
  if (request->hasParam("new_wifi_mode")) {
    new_wifi_mode = request->getParam("new_wifi_mode")->value();
  }



  if (new_system_mode.length() == 1 && new_wifi_mode.length() == 1) {
    EEPROM.write(EEPROM_ADDR_CURRENT_WIFI_MODE, atoi(new_wifi_mode.c_str()));
    EEPROM.commit();
    EEPROM.write(EEPROM_ADDR_CURRENT_SYSTEM_MODE, atoi(new_system_mode.c_str()));
    EEPROM.commit();
    if (reg.current_system_mode != (system_mode)atoi(new_system_mode.c_str()) ||
        reg.current_wifi_mode != (wifi_mode)atoi(new_wifi_mode.c_str())) {
      reg.current_system_mode = (system_mode)atoi(new_system_mode.c_str());
      reg.current_wifi_mode = (wifi_mode)atoi(new_wifi_mode.c_str());
      reg.new_mode = true;
    }
  }
};

String GetBuildDateAndTime(void) {
  // "2017-03-07T11:08:02" - ISO8601:2004
  char bdt[21];
  char *p;
  char mdate[] = __DATE__;  // "Mar  7 2017"
  char *smonth = mdate;
  int day = 0;
  int year = 0;

  // sscanf(mdate, "%s %d %d", bdt, &day, &year);  // Not implemented in 2.3.0
  // and probably too much code
  uint8_t i = 0;
  for (char *str = strtok_r(mdate, " ", &p); str && i < 3;
       str = strtok_r(nullptr, " ", &p)) {
    switch (i++) {
      case 0:  // Month
        smonth = str;
        break;
      case 1:  // Day
        day = atoi(str);
        break;
      case 2:  // Year
        year = atoi(str);
    }
  }

  int month = (strstr(kMonthNamesEnglish, smonth) - kMonthNamesEnglish) / 3 + 1;
  snprintf_P(bdt, 21, "%04d-%02d-%02d %s", year, month, day, __TIME__);
  return String(bdt);  // 2017-03-07T11:08:02
}

// @FIXME remove to main
void reboot(AsyncWebServerRequest *request) {
  request->redirect("/rebootinfo");
  delay(3000);
  WifiDisconnect();
  delay(500);
  ESP.restart();
}

String getContentType(
    String filename) {  // convert the file extension to the MIME type
  if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".jpeg"))
    return "image/jpeg";
  else if (filename.endsWith(".jpg"))
    return "image/jpg";
  return "text/plain";
}

bool handleFileRead(
    String path) {  // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/"))
    path += "index.html";  // If a folder is requested, send the index file
  String contentType = getContentType(path);  // Get the MIME type
  if (SPIFFS.exists(path)) {                  // If the file exists
    File file = SPIFFS.open(
        path, "r");  // Open it
                     //        size_t sent = WebServer->streamFile(file,
                     //        contentType); // And send it to the client
    file.close();    // Then close the file again
    return true;
  }
  Serial.println("\tFile Not Found");
  return false;  // If the file doesn't exist, return false
}

String table2DGenerator(String data[][2], uint8_t size, boolean bold) {
  String tdstart("<tr><td>");
  String tdmittle("</td><td>");
  if (bold) {
    tdstart = "<tr><td><b>";
    tdmittle = "</b></td><td>";
  }
  String tdend("</td></tr>");
  String retvar("<table>");

  for (uint8_t z = 0; z < size; z++) {
    retvar += tdstart + data[z][0] + tdmittle + data[z][1] + tdend;
  }
  retvar += "</table>";
  return retvar;
};

String readSPIFFS2String(const char *path) {
  char buf[64] = {0};
  if (!SPIFFS.exists(path)) {
    snprintf(buf, 64, "ERROR, %s do not exists.", path);
    Serial.println(buf);
    return String(buf);
  }
  File f = SPIFFS.open(path, "r");
  String retvar;
  while (f.available()) {
    retvar += char(f.read());
  }
  return retvar;
};

String optionsFeldGenerator(uint8_t selected, const char *name, String data[],
                            uint8_t size) {
  DDD(name);
  DDD(selected);
  char buf[1000] = {0};
  char zbuf[128] = {0};
  char selectxt[32] = {0};
  snprintf(zbuf, 128, "\n\n<select name='%s'>\n", name);
  strncat(buf, zbuf, 1000);
  for (uint8_t i = 0; i < size; i++) {
    if (i == selected) {
      strncpy(selectxt, "selected", 32);
    } else {
      strncpy(selectxt, "", 32);
    }
    snprintf(zbuf, 64, "<option value='%d' %s> %s</option>\n", i, selectxt,
             data[i].c_str());
    strncat(buf, zbuf, 128);
  }  // END for

  strncat(buf, "</select>\n\n", 1000);

  DDD(name);

  return String(buf);
}

void showRequest(AsyncWebServerRequest *request) {
  if (request->method() == HTTP_GET)
    Serial.printf("GET");
  else if (request->method() == HTTP_POST)
    Serial.printf("POST");
  else if (request->method() == HTTP_DELETE)
    Serial.printf("DELETE");
  else if (request->method() == HTTP_PUT)
    Serial.printf("PUT");
  else if (request->method() == HTTP_PATCH)
    Serial.printf("PATCH");
  else if (request->method() == HTTP_HEAD)
    Serial.printf("HEAD");
  else if (request->method() == HTTP_OPTIONS)
    Serial.printf("OPTIONS");
  else
    Serial.printf("UNKNOWN");
  Serial.printf(" http://%s%s\n", request->host().c_str(),
                request->url().c_str());

  if (request->contentLength()) {
    Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
    Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
  }

  int i;
  // int headers = request->headers();
  // for (i = 0; i < headers; i++) {
  //   AsyncWebHeader *h = request->getHeader(i);
  //   Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(),
  //   h->value().c_str());
  //  }

  int params = request->params();
  for (i = 0; i < params; i++) {
    AsyncWebParameter *p = request->getParam(i);
    if (p->isFile()) {
      Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(),
                    p->value().c_str(), p->size());
    } else if (p->isPost()) {
      Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else {
      Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }
}

boolean validateNumber(String test) {
  if (!isNumeric(test)) {
    return false;
  }
  if (test.toInt() < 0 || test.toInt() > 15) {
    return false;
  }
  return true;
}

boolean isNumeric(String str) {
  unsigned int stringLength = str.length();

  if (stringLength == 0) {
    return false;
  }
  for (unsigned int i = 0; i < stringLength; ++i) {
    if (isDigit(str.charAt(i))) {
      continue;
    } else {
      return false;
    }
  }
  return true;
}

void APRSWebServerTick(void) {
  if (globalClient != NULL && globalClient->status() == WS_CONNECTED) {
    //      String randomNumber = String(random(0,20));
    //      globalClient->text(randomNumber);
    static uint32_t last = 0;
    uint32_t i = millis();
    if (i > last + 5000) {
      last = i;
      // globalClient->text("Uptime: " + String(last / 1000) + " Sekunden");
      sendGPSDataJson();
    }
  }
};

void resetHTMLError(void) {
  html_error.ErrorMsg = "";
  html_error.isSended = false;
}

void HTMLSendError(String msg, AsyncWebServerRequest *request) {
  Serial.println("send HTML error: " + msg);
  html_error.ErrorMsg = msg;
  html_error.isSended = false;
  Serial.print("request->url(): ");
  Serial.println(request->url());
  request->redirect(request->url());
}

void sendGPSDataJson(void) {
  // StaticJsonDocument<10000> doc;

  // AsyncJsonResponse * response = new AsyncJsonResponse();
  // JsonVariant& root = response->getRoot();
  globalClient->server()->cleanupClients();

  StaticJsonDocument<1024> root;
  root["isValidTime"] = 1;
  root["isValidGPS"] = 1;
  root["time"] = millis() / 1000;
  root["date"] = "2020-01-04";
  root["lat"] = "52.4711";
  root["lng"] = "13.2142";
  root["speed"] = 0;
  root["direction"] = "";
  root["temp"] = 21.3;
  root["humidity"] = 51;
  root["pressure"] = 1024;
  root["sensor"] = "HDT11";
  root["sat"] = 7;
  root["hdop"] = 1;
  uint16_t len = measureJson(root);
  // Serial.println(len);
  // serializeJson(root, Serial);

  // ws.cleanupClients(); !!!

  AsyncWebSocketMessageBuffer *buffer = globalClient->server()->makeBuffer(
      len);  //  creates a buffer (len + 1) for you.
  if (buffer) {
    serializeJson(root, (char *)buffer->get(), len + 1);
    globalClient->server()->textAll(buffer);
  }

  // serializeJsonPretty(root, Serial);
};
