#include <uxa_debug.h>
#include <APRSWebServer.h>
#include <APRSWiFi.h>
#include <APRS_MSG.h>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <AsyncJson.h>
#include <Registry.h>
#include <TinyGPS++.h>
#include <Preferences.h>
#include <SPIFFSEditor.h>


#include <Ticker.h>

TinyGPSPlus gps;

extern Preferences preferences;
extern QueueHandle_t LoRaTXQueue;

//#include <CallBackList.h>

// @TODO remove together with restart()

extern Registry reg;

AsyncWebServer *WebServer;
AsyncWebSocket *ws;
AsyncWebSocketClient *globalClient = NULL;

String mainmenue(
    "<form action='.' method='get'><button>Main Menue</button></form><br />");

struct HTML_Error {
  String ErrorMsg;
  boolean isSended;

 public:
  void setErrorMsg(String msg) {
    if (ErrorMsg == "") {
      ErrorMsg = msg;
    } else {
      ErrorMsg += msg;
    }
    isSended = false;
  }
  String getErrorMsg(void) {
    isSended = true;
    return ErrorMsg;
  }
};

HTML_Error html_error;

struct SendMsgForm {
  String to = "";
  String wide = "0";
  String msg = "";
  String path = "";
  String gateway = "";
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
    // first run wizard
    if (reg.call == "CHANGEME") { // first run wizard
      request->redirect("/cc");
    }
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
      if (reg.APCredentials.auth_tocken == "letmein42") { // first run wizard
        request->redirect("/ca");
      }
      request->redirect("/");
    } else {
      request->send(SPIFFS, "/main.html", "text/html", false,
                    ProcessorConfigCall);
    }
  });

  // Send Message
  WebServer->on("/sm", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/sm");
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
    showRequest(request);
    DDD("Change Mode");
    if (request->params() == 2) {
      DDD(request->params());
      handleRequestChangeMode(request);
      Serial.printf("new run_mode: %d / new_wifi_mode %d\n", (int)reg.current_system_mode, (int)reg.current_wifi_mode);
      Serial.printf("new run_mode: %d / new_wifi_mode %d\n", (int)getPrefsDouble(PREFS_CURRENT_SYSTEM_MODE), (int)getPrefsDouble(PREFS_CURRENT_WIFI_MODE));
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
    showRequest(request);
    if (request->params()) {
      handleRequestConfigGateway(request);
      request->redirect("/");
    }
    request->send(SPIFFS, "/main.html", "text/html", false,
                  ProcessorConfigGateway);
  });

  // reboot
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


// Config Web Admin
  WebServer->on("/APRSSymbol", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("/APRSSymbol");
    showRequest(request);
    request->send(SPIFFS, "/APRS_Symbol_Chart.pdf", "application/pdf", false);
    
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
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("Main Menue");
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
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
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("System Info");
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
  }

  if (var == "BODY") {
    return getSystemInfoTable();
  }

  return String("wrong placeholder " + var);
}

String getSystemInfoTable(void) {
#ifdef ESP32
  uint64_t chipid = chipid =
      ESP.getEfuseMac();  // The chip ID is essentially its MAC address(length:
                          // 6 bytes).
#endif

  FlashMode_t ideMode = ESP.getFlashChipMode();

  String systemdata[][2] = {
      {"Build DateTime: ", GetBuildDateAndTime()},
      {"SDKVersion: ", String(ESP.getSdkVersion())},
      {"BootCount: ", String(reg.boot_count)},
      {"Uptime: ", String(millis() / 1000 / 60, DEC) + "min"},
#ifdef ESP32
      {"Chip Revision:", String(ESP.getChipRevision())},
      {"ESP32 Chip ID:",
       String((uint16_t)chipid >> 32, HEX) + String((uint32_t)chipid, HEX)},
      {"Reset Reason CPU0: ", getResetReason(rtc_get_reset_reason(0))},
      {"Reset Reason CPU1: ", getResetReason(rtc_get_reset_reason(1))},
      {"CpuFreqMHz: ", String(ESP.getCpuFreqMHz()) + "MHz"},
      {"CycleCount: ", String(ESP.getCycleCount())},
      {"FlashChipMode: ", String(ESP.getFlashChipMode())},
      {"FlashChipSize: ", String(ESP.getFlashChipSize() / 1024 / 1024) + "MB"},
      {"FlashChipSpeed: ", String(ESP.getFlashChipSpeed()/1024/1024) + "MHz"},
      {"SketchSize: ", String(ESP.getSketchSize() / 1024) + "kB"},
      {"FreeSketchSpace: ", String(ESP.getFreeSketchSpace() / 1024) + "kB"},
      {"SketchMD5: ", String(ESP.getSketchMD5())},
      {"HeapSize: ", String(ESP.getHeapSize() / 1024) + "kB"},
      {"FreeHeap: ", String(ESP.getFreeHeap() / 1024) + "kB"},
      {"MaxAllocHeap: ", String(ESP.getMaxAllocHeap() / 1024) + "kB"},
      {"MinFreeHeap: ", String(ESP.getMinFreeHeap() / 1024) + "kB"},

      // {"PsramSize: ", String(ESP.getPsramSize() / 1024) + "kB"},
      // {"FreePsram", String(ESP.getFreePsram() / 1024) + "kB"},
      // {"MaxAllocPsram: ", String(ESP.getMaxAllocPsram() / 1024) + "kB"},
      // {"MinFreePsram", String(ESP.getMinFreePsram() / 1024) + "kB"},

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
      {"GPS:", String(reg.hardware.GPS)},
      {"OLED:", String(reg.hardware.OLED)},
      {"DHT:", String(reg.hardware.DHT22)},
      {"BME280", String(reg.hardware.BME280)},
  };

  return table2DGenerator(systemdata, 32, true) +
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
String getResetReason(RESET_REASON reason) {
  String retvar;
  switch (reason) {
    case 1:
      retvar = "POWERON_RESET";
      break; /**<1, Vbat power on reset*/
    case 3:
      retvar = "SW_RESET";
      break; /**<3, Software reset digital core*/
    case 4:
      retvar = "OWDT_RESET";
      break; /**<4, Legacy watch dog reset digital core*/
    case 5:
      retvar = "DEEPSLEEP_RESET";
      break; /**<5, Deep Sleep reset digital core*/
    case 6:
      retvar = "SDIO_RESET";
      break; /**<6, Reset by SLC module, reset digital core*/
    case 7:
      retvar = "TG0WDT_SYS_RESET";
      break; /**<7, Timer Group0 Watch dog reset digital core*/
    case 8:
      retvar = "TG1WDT_SYS_RESET";
      break; /**<8, Timer Group1 Watch dog reset digital core*/
    case 9:
      retvar = "RTCWDT_SYS_RESET";
      break; /**<9, RTC Watch dog Reset digital core*/
    case 10:
      retvar = "INTRUSION_RESET";
      break; /**<10, Instrusion tested to reset CPU*/
    case 11:
      retvar = "TGWDT_CPU_RESET";
      break; /**<11, Time Group reset CPU*/
    case 12:
      retvar = "SW_CPU_RESET";
      break; /**<12, Software reset CPU*/
    case 13:
      retvar = "RTCWDT_CPU_RESET";
      break; /**<13, RTC Watch dog Reset CPU*/
    case 14:
      retvar = "EXT_CPU_RESET";
      break; /**<14, for APP CPU, reseted by PRO CPU*/
    case 15:
      retvar = "RTCWDT_BROWN_OUT_RESET";
      break; /**<15, Reset when the vdd voltage is not stable*/
    case 16:
      retvar = "RTCWDT_RTC_RESET";
      break; /**<16, RTC Watch dog reset digital core and rtc module*/
    default:
      retvar = "NO_MEAN";
  }
  return retvar;
}
#endif

String ProcessorConfigCall(const String &var) {

      String options[][2] = {
        {" -0: Primärstation", "0"},
        {" -1: weiter allgemeine Std", "1"},
        {" -2: weiter allgemeine Std", "2"},
        {" -3: weiter allgemeine Std", "3"},
        {" -4: weiter allgemeine Std", "4"},
        {" -5: anderses Netzwerk (D-Star / 3G)", "5"},
        {" -6: Satellit", "6"},
        {" -7: Handfunkgerät", "7"},
        {" -8: Boot / Schiff", "8"},
        {" -9: Mobilstadtion", "9"},
        {"-10: APRS-IS ohne Funkmodul", "10"},
        {"-11: Ballon / Fluggerät / Raumschiff", "11"},
        {"-12: APRStt / DTML (Einweg)", "12"},
        {"-13: Wetterstation", "13"},
        {"-14: Lastkraftwagen (permanent)", "14"},
        {"-15: weiter allgemeine Std", "15"},
    };



  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("Config Call");
  }

  if (var == PREFS_CALL) {
    return reg.call;
  }

  if (var == PREFS_POS_LAT_FIX) {
    return String(reg.posfix.latitude, 6);
  }

  if (var == PREFS_POS_LNG_FIX) {
    return String(reg.posfix.longitude, 6);
  }

  if (var == PREFS_POS_ALT_FIX) {
    return String(reg.posfix.altitude, 1);
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
  }

  if (var == PREFS_APRS_SYMBOL) {
    return String(reg.aprs_symbol.symbol);
  }

  if (var == "aprs_ext_options") {
    return optionsFeldGenerator(reg.aprs_call_ext.toInt(), PREFS_APRS_CALL_EX, options, 16);
  }

  if (var == PREFS_APRS_CALL_EX) {
    return reg.aprs_call_ext;
  }


  if (var == "wx_ext_options") {
    DDD(reg.wx_call_ext);
    return optionsFeldGenerator(reg.wx_call_ext.toInt(), PREFS_WX_CALL_EX, options, 16);
  }

  if (var == PREFS_WX_CALL_EX) {
    return reg.wx_call_ext;
  }


  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigCall.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
};

void handleRequestConfigCall(AsyncWebServerRequest *request) {
  // call
  if (request->hasParam(PREFS_CALL)) {
    String new_call = "";
    new_call = getWebParam(request, PREFS_CALL);
    if (new_call.length() > 2 && new_call.length() < 16) {
      new_call.toUpperCase();
      reg.call = new_call;
      setPrefsString(PREFS_CALL, new_call);
    } else {
      html_error.ErrorMsg = String("call to short or to long");
      Serial.println("ERR: call not valide! " + new_call);
    }
  }

  // aprs_call_ext
  getWebParam(request, PREFS_APRS_CALL_EX, &reg.aprs_call_ext);
  getWebParam(request, PREFS_WX_CALL_EX, &reg.wx_call_ext);

  DDD("wx_ext +OK");

  String new_aprs_symbol = "";
  if (request->hasParam(PREFS_APRS_SYMBOL)) {
    new_aprs_symbol = getWebParam(request, PREFS_APRS_SYMBOL);
    reg.aprs_symbol.symbol = (char)new_aprs_symbol.charAt(0);
    setPrefsChar(PREFS_APRS_SYMBOL, new_aprs_symbol.charAt(0));
  } else {
    Serial.println("ERR: APRS Symbol not valide!" + new_aprs_symbol);
  }

  ////////////////////////////////////////////////////////////////////////////////

  if (request->hasParam(PREFS_POS_LAT_FIX)) {
    getWebParam(request, PREFS_POS_LAT_FIX, &reg.posfix.latitude);
  }

  if (request->hasParam(PREFS_POS_LNG_FIX)) {
    getWebParam(request, PREFS_POS_LNG_FIX, &reg.posfix.longitude);
  }

  if (request->hasParam(PREFS_POS_ALT_FIX)) {
    getWebParam(request, PREFS_POS_ALT_FIX, &reg.posfix.altitude);

  }
}

String ProcessorGPSInfo(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("GPS Info");
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
  }

  if (var == "BODY") {
    return readSPIFFS2String("/GPSInfo.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
}

String ProcessorWXInfo(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("GPS Info");
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
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
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("Config Wifi AP");
  }

  if (var == PREFS_AP_SSID) {
    return reg.APCredentials.auth_name;
  }

  if (var == PREFS_AP_PASS) {
    return reg.APCredentials.auth_tocken;
  }

  if (var == PREFS_WEB_ADMIN) {
    return reg.WebCredentials.auth_name;
  }

  if (var == PREFS_WEB_PASS) {
    return reg.WebCredentials.auth_tocken;
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigWifiAP.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
}

void handleRequestConfigAP(AsyncWebServerRequest *request) {
  if (request->hasParam(PREFS_AP_SSID)) {
    getWebParam(request, PREFS_AP_SSID, &reg.APCredentials.auth_name);
  }

  if (request->hasParam(PREFS_AP_PASS)) {
    getWebParam(request, PREFS_AP_PASS, &reg.APCredentials.auth_tocken);
  }

  if (request->hasParam(PREFS_WEB_ADMIN)) {
    getWebParam(request, PREFS_WEB_ADMIN, &reg.WebCredentials.auth_name);
  }

  if (request->hasParam(PREFS_WEB_PASS)) {
    getWebParam(request, PREFS_WEB_PASS, &reg.WebCredentials.auth_tocken);
  }

}

String ProcessorConfigGateway(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("Config GateWay");
  }

  if (var == PREFS_APRS_SERVER0) {
    return reg.APRSServer[0];
  }

  if (var == PREFS_APRS_SERVER1) {
    return reg.APRSServer[1];
  }

  if (var == PREFS_APRS_PASSWORD) {
    return reg.APRSPassword;
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigGateway.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
}

void handleRequestConfigGateway(AsyncWebServerRequest *request) {
  if (request->hasParam(PREFS_APRS_PASSWORD)) {
    getWebParam(request, PREFS_APRS_PASSWORD, &reg.APRSPassword);
  }

  if (request->hasParam(PREFS_APRS_SERVER0)) {
    getWebParam(request, PREFS_APRS_SERVER0, &reg.APRSServer[0]);
  }

  if (request->hasParam(PREFS_APRS_SERVER1)) {
    getWebParam(request, PREFS_APRS_SERVER1, &reg.APRSServer[1]);
  }

}

String ProcessorSendMessage(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("Send APRS Message");
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
  }

    if (var == MSG_FORM_TO) {
    return send_msg_form_tmp.to;
  }

  if (var == MSG_FORM_WIDE) {
    String options[][2] = {
        { "local", "0"},
        {"WIDE1-1", "1" },
        {"WIDE2-2", "2" },
        {"WIDE3-3", "3" }
    };

    return optionsFeldGenerator(send_msg_form_tmp.wide.toInt(), MSG_FORM_WIDE, options,
                                4);
    
  }

  if (var == MSG_FORM_MSG) {
    return send_msg_form_tmp.msg;
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormSendMessage.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
}

void handleRequestSendMessage(AsyncWebServerRequest *request) {
  String msg = getWebParam(request, MSG_FORM_MSG);
  String to = getWebParam(request, MSG_FORM_TO);
  String wide = getWebParam(request, MSG_FORM_WIDE);

  SendMsgForm sform;

  if (msg.length() > 0 && msg.length() < 256 && to.length() > 4) {
    sform.msg = msg;
    sform.to = to;
  } else {
    html_error.setErrorMsg("Error wrong parameter");
  }
  if (wide.length() > 0) {
    sform.wide = wide;    
  }

  if (xQueueSend(LoRaTXQueue, (SendMsgForm*)&sform, (TickType_t)100) != pdPASS) {
    Serial.printf("ERROR: Can't put APRS msg to LoRaTXQueue\n to:%s msg:%s", sform.to.c_str(), sform.msg.c_str());
  }



};



String ProcessorConfigWLAN(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("Configure WLAN");
  }

  if (var == PREFS_LAN0_SSID) {
    return reg.WifiCrendentials[0].auth_name;
  }

  if (var == PREFS_LAN0_AUTH) {
    return reg.WifiCrendentials[0].auth_tocken;
  }

  if (var == PREFS_LAN1_SSID) {
    return reg.WifiCrendentials[1].auth_name;
  }

  if (var == PREFS_LAN1_AUTH) {
    return reg.WifiCrendentials[1].auth_tocken;
  }

  if (var == PREFS_LAN2_SSID) {
    return reg.WifiCrendentials[2].auth_name;
  }

  if (var == PREFS_LAN2_AUTH) {
    return reg.WifiCrendentials[2].auth_tocken;
  }

  if (var == PREFS_LAN3_SSID) {
    return reg.WifiCrendentials[3].auth_name;
  }

  if (var == PREFS_LAN3_AUTH) {
    return reg.WifiCrendentials[3].auth_tocken;
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigWLAN.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
};

void handleRequestConfigWLAN(AsyncWebServerRequest *request) {
  if (request->hasParam(PREFS_LAN0_SSID)) {
    getWebParam(request, PREFS_LAN0_SSID, &reg.WifiCrendentials[0].auth_name);
    getWebParam(request, PREFS_LAN0_AUTH, &reg.WifiCrendentials[0].auth_tocken);
  }

  if (request->hasParam(PREFS_LAN1_SSID)) {
    getWebParam(request, PREFS_LAN1_SSID, &reg.WifiCrendentials[1].auth_name);
    getWebParam(request, PREFS_LAN1_AUTH, &reg.WifiCrendentials[1].auth_tocken);
  }

  if (request->hasParam(PREFS_LAN2_SSID)) {
    getWebParam(request, PREFS_LAN2_SSID, &reg.WifiCrendentials[2].auth_name);
    getWebParam(request, PREFS_LAN2_AUTH, &reg.WifiCrendentials[2].auth_tocken);
  }

  if (request->hasParam(PREFS_LAN3_SSID)) {
    getWebParam(request, PREFS_LAN3_SSID, &reg.WifiCrendentials[3].auth_name);
    getWebParam(request, PREFS_LAN3_AUTH, &reg.WifiCrendentials[3].auth_tocken);
  }


}

String ProcessorChangeMode(const String &var) {
  if (var == "HTMLTILE") {
    return String("simple LoRaAPRS system by DL7UXA");
  }

  if (var == "H3TITLE") {
    return String("sLoRaAPRS for ") + reg.call;
  }

  if (var == "H2TITLE") {
    return String("Configure Run Mode & Wifi Mode");
  }

  if (var == "ERRORMSG") {
    return html_error.getErrorMsg();
  }

  if (var == PREFS_CURRENT_SYSTEM_MODE) {
    // enum system_mode {mode_tracker, mode_wxtracker, mode_wxfix,
    // mode_repeater, mode_gateway, mode_repeater_gateway};
    String options[][2] = {
        {"GPS Tracker", "0"},
        {"WX Tracker", "1"},
        {"WX Fix Position (no GPS)", "2"},
        {"APRS LoRa Repeater", "3"},
        {"APRS Gateway", "4"},
        {"APRS LoRa Repeater & APRS Gateway", String(mode_repeater_gateway)},
    };

    return optionsFeldGenerator(reg.current_system_mode,
                                PREFS_CURRENT_SYSTEM_MODE, options, 6);
  }

  // enum wifi_mode {wifi_off, wifi_ap, wifi_client};
  if (var == PREFS_CURRENT_WIFI_MODE) {
    String options[][2] = {
        { "Wifi OFF", "0"},
        {"Wifi AP", "1" },
        {"WLAN Connect", "2" }
    };

    return optionsFeldGenerator(reg.current_wifi_mode, PREFS_CURRENT_WIFI_MODE, options,
                                3);
  }

  if (var == "BODY") {
    return readSPIFFS2String("/FormConfigMode.html") + mainmenue;
  }

  return String("wrong placeholder " + var);
};

void handleRequestChangeMode(AsyncWebServerRequest *request) {
  DDD("handleRequestChangeMode");
  if (request->hasParam(PREFS_CURRENT_SYSTEM_MODE)) {
    String new_system_mode = getWebParam(request, PREFS_CURRENT_SYSTEM_MODE);
    setPrefsUInt(PREFS_CURRENT_SYSTEM_MODE, (int)new_system_mode.toInt());
    reg.current_system_mode = (system_mode) (int)new_system_mode.toInt();
  }

  if (request->hasParam(PREFS_CURRENT_WIFI_MODE)) {
    String new_wifi_mode = getWebParam(request, PREFS_CURRENT_WIFI_MODE);
    setPrefsUInt(PREFS_CURRENT_WIFI_MODE, new_wifi_mode.toInt());
    reg.current_wifi_mode = (wifi_mode) new_wifi_mode.toInt();
  }

}

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

/**
 * reboot device
 *
 * @author	Unknown
 * @since	v0.0.1
 * @version	v1.0.0	Wednesday, January 15th, 2020.
 * @global
 * @param	asyncwebserverrequest	*request	
 * @return	void
 */
void reboot(AsyncWebServerRequest *request) {
  request->redirect("/rebootinfo");
  delay(3000);
  WifiDisconnect();
  delay(500);
  ESP.restart();
}

/**
 * table2DGenerator.
 *
 * @author	Unknown
 * @since	v0.0.1
 * @version	v1.0.0	Thursday, January 16th, 2020.
 * @global
 * @param	string 	data[][2]	
 * @param	uint8_t	size     	
 * @param	boolean	bold     	
 * @return	mixed
 */
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

/**
 * @brief generates HTML options field from given parameter
 * 
 * @param selected pre selected field
 * @param name field name (HTML)
 * @param data array of HTML display Strings (keys) -> values
 * @param size count elements
 * @return String 
 */
String optionsFeldGenerator(uint8_t selected, const char *name, String data[][2],
                            uint8_t size) {
  DDD(name);
  DDD(selected);
  char buf[1200] = {0};
  char zbuf[1200] = {0};
  char selectxt[32] = {0};
  snprintf(zbuf, 1200, "\n\n<select name='%s'>\n", name);
  strncat(buf, zbuf, 1200);
  for (uint8_t i = 0; i < size; i++) {
    if (i == selected) {
      strncpy(selectxt, " selected ", 32);
    } else {
      strncpy(selectxt, "", 32);
    }
    snprintf(zbuf, 1200, "<option value=\"%s\"%s>%s</option>\n", data[i][1].c_str(), selectxt,
             data[i][0].c_str());
    strncat(buf, zbuf, 1200);
  }  // END for

  strncat(buf, "</select>\n\n", 1200);

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


void APRSWebServerTick(void) {
  if (globalClient != NULL && globalClient->status() == WS_CONNECTED) {
    //      String randomNumber = String(random(0,20));
    //      globalClient->text(randomNumber);
    static uint32_t last = 0;
    uint32_t i = millis();
    if (i > last + 1000) {
      last = i;
      // globalClient->text("Uptime: " + String(last / 1000) + " Sekunden");
      sendGPSDataJson();
    }
  }
};


void sendGPSDataJson(void) {
  
  
  // StaticJsonDocument<10000> doc;

  // AsyncJsonResponse * response = new AsyncJsonResponse();
  // JsonVariant& root = response->getRoot();
  globalClient->server()->cleanupClients();
  char tmpbuf[32] = {0};
  StaticJsonDocument<1024> root;
  root["isValidTime"] = gps.time.isValid();
  root["isValidGPS"] = gps.date.isValid();

  snprintf(tmpbuf, 12, "%02d:%02d:%02d", reg.gps_time.hour, reg.gps_time.minute, reg.gps_time.second);
  root["time"] = tmpbuf;
  
  snprintf(tmpbuf, 12, "%4d-%02d-%02d", reg.gps_time.year, reg.gps_time.month, reg.gps_time.day);  
  root["date"] = tmpbuf;
 
  root["lat"] = reg.gps_location.latitude;
  root["lng"] = reg.gps_location.longitude;
  root["alt"] = reg.gps_location.altitude;
  root["course"] = reg.gps_move.course;
  root["speed"] = reg.gps_move.speed;
  root["temp"] = reg.WXdata.temp;
  root["humidity"] = reg.WXdata.humidity;
  root["pressure"] = reg.WXdata.pressure;
  root["sensor"] = "BME280";
  root["sat"] = reg.gps_meta.sat;
  root["hdop"] = reg.gps_meta.hdop;
  uint16_t len = measureJson(root);
  // Serial.println(len);
  // serializeJson(root, Serial);

  AsyncWebSocketMessageBuffer *buffer = globalClient->server()->makeBuffer(
      len);  //  creates a buffer (len + 1) for you.
  if (buffer) {
    serializeJson(root, (char *)buffer->get(), len + 1);
    globalClient->server()->textAll(buffer);
  }

  // serializeJsonPretty(root, Serial);
};

String getWebParam(AsyncWebServerRequest *request, const char *key,
                   String *prefsvar) {
  String new_var = "";
  if (request->hasParam(key)) {
    new_var = request->getParam(key)->value();
    if (new_var.length() > 0 && new_var.length() < 32) {
      *prefsvar = new_var;
      Serial.printf("set new var to reg key=%s value=%s\n", key, new_var.c_str());
      setPrefsString(key, new_var);
    }
    return new_var;
  } else {
    char buf[32] = {0};
    snprintf(buf, 32, "ERR> key %s not found in request,  no value written", key);
    DDD(buf);
    return String("");
  }
  return new_var;
}

String getWebParam(AsyncWebServerRequest *request, const char *key,
                   double *prefsvar) {
  String new_var = "";
  if (request->hasParam(key)) {
    new_var = request->getParam(key)->value();
    new_var.replace(',', '.');
    *prefsvar = new_var.toDouble();
    setPrefsDouble(key, new_var.toDouble());
    return new_var;
  } else {
    char buf[32] = {0};
    snprintf(buf, 32, "key %s not found in request, no value written", key);
    DDD(buf);
    return String("");
  }
  return new_var;
}

String getWebParam(AsyncWebServerRequest *request, const char *key) {
  String new_var = "";
  if (request->hasParam(key)) {
    new_var = request->getParam(key)->value();
    if (new_var.length() > 0 && new_var.length() < 32) {
      return new_var;
    }
  } else {
    char buf[32] = {0};
    snprintf(buf, 32, "key %s not found in request, no value written", key);
    DDD(buf);
    return String("");
  }
  return new_var;
}
