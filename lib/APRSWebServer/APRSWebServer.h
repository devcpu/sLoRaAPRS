#ifndef APRS_WEBSERVER_H
#define APRS_WEBSERVER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#ifdef ESP32
#include <rom/rtc.h>
#endif




void WebserverStart(void);

static const char kMonthNamesEnglish[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

String getContentType(String filename);

String ProcessorDefault(const String& var);

String systemInfoProcessor(const String& var);
String ProcessorConfigCall(const String& var);
void handleRequestConfigCall(AsyncWebServerRequest *request);

String ProcessorSendMessage(const String& var);
void handleRequestSendMessage(AsyncWebServerRequest *request);


void handleRequestChangeMode(AsyncWebServerRequest *request);
String ProcessorChangeMode(const String& var);

String ProcessorGPSInfo(const String& var);
String ProcessorConfigWifiAP(const String& var);
void handleRequestConfigAP(AsyncWebServerRequest *request);

String ProcessorConfigGateway(const String& var);
void handleRequestConfigGateway(AsyncWebServerRequest *request);

String ProcessorConfigWebAdmin(const String& var);


String ProcessorConfigWLAN(const String& var);
void handleRequestConfigWLAN(AsyncWebServerRequest *request);

String ProcessorJS(const String& var);

String getSystemInfoTable(void);
String table2DGenerator(String data[][2], uint8_t size, boolean bold);
String GetBuildDateAndTime(void);
String readSPIFFS2String(const char* path);
void reboot(AsyncWebServerRequest *request);

String optionsFeldGenerator(uint8_t selected, const char *name, String data[][2], uint8_t size);

void showRequest(AsyncWebServerRequest *request);
boolean validateNumber(String test);
boolean isNumeric(String str);

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void APRSWebServerTick(void);

void resetHTMLError(void);
void sendGPSDataJson(void);
void HTMLSendError(String msg, AsyncWebServerRequest *request);

String ProcessorWXInfo(const String& var);

#ifdef ESP32
String getResetReason(RESET_REASON reason);
#endif

// String getWebParam(AsyncWebServerRequest *request, const char* key, String prefsvar);
// String getWebParam(AsyncWebServerRequest *request, const char *key, double prefsvar);
String getWebParam(AsyncWebServerRequest *request, const char* key);

#endif