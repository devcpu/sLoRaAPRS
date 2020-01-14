#ifndef TRACKER_DISPLAY_H
#define TRACKER_DISPLAY_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <APRS_MSG.h>
#include <TinyGPS++.h>

#define OLED_RESET 4

bool initDisplay(void);

extern APRS_MSG tx_msg;
extern TinyGPSPlus gps;
extern bool displayCange;
extern uint16_t tracker_display_time;

uint8_t tracker_display_switch(void);

void tracker_display_tick(void);

void writeUTC();
void writeGPS();
void writeWX();
void writeHead(const char* head);
void writeTX(const char *to);

void write3Line(const char *head, const char *line1, const char *line2, bool toSerial, u_long sleep);
void write3toSerial(const char *head, const char *line1, const char *line2, u_long sleep);


void write_no_vaild_data(void);

void write2Display(String head, String line1, String line2, String line3);
//void write2Display(String head, String line1, String line2, String line3, String line4);

#endif