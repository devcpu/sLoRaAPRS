
#include <APRS_MSG.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <uxa_debug.h>

extern TinyGPSPlus gps;


// ".../...g...t...r...p...P...h..b...."


char*  APRS_MSG::computeWXField(char *rv) {
    char hum_buf[16] = {0};
    snprintf(rv, 54, "%s/%sg%st%03dr%sp%sP%sh%sb%04d %s",
    "...", //winddirection 0-360
    "...", // windspeed in mph
    "...", // gust
    (int)APRS_MSG::c2f(reg.WXdata.temp), // temperature
    "...", // rainfalllasthourhinch
    "...", // rainfalllast24h
    "...", // rainfallsinceMidnight
    APRS_MSG::calcHumidity(hum_buf, reg.WXdata.humidity), // humidity
    (int)reg.WXdata.pressure, // pressure
    "BME280" // wx sensor
    
    );
    return rv;
}

char* APRS_MSG::calcHumidity(char *rv, float humidity){
    if (humidity == 100) {
        snprintf(rv, 3, "%s", "00");
    } else {
       snprintf(rv, 3, "%02u", (int)humidity);
    }
    return rv;
};



// char* APRS_MSG::getWXField() {
//     computeWXField();
//     return _aprs_wx_string;
// };

//String APRS_MSG::computeAPRSPos(double lat, double lng, Registry *reg) {
char* APRS_MSG::computeAPRSPos(char *rv) {
    char buf_lat[16] = {0};
    char buf_lng[16] = {0};
    snprintf(rv, 32, "%s%c%s%c",
        dc2gms(buf_lat, gps.location.lat(), false),
        reg.aprs_symbol.table,
        dc2gms(buf_lng, gps.location.lng(), true),
        reg.aprs_symbol.symbol
        );
        return rv;
};



char* APRS_MSG::computeTrackInfo(char *rv) {
    snprintf(rv, 64, "%03d/%03d/A=%06d",
        (int)gps.speed.mph(),
        (int)gps.course.deg(),
        (int)gps.altitude.feet()
        );
        return rv;
}


// char* APRS_MSG::getAPRSTxPos(char* retvar) {
//     computeAPRSPos();
//     snprintf(retvar, 254, "%s>APRS:!%s", _call, _aprs_pos);
//     //Serial.printf("APRS_MSG::getAPRSTxPos retvar '%s'\n", retvar);
//     return retvar;

//DL7UXA-1>APRS:!
//};

/***************************************************************************/
/**************************   static converters   **************************/
/***************************************************************************/
double_t APRS_MSG::feed2meter(double_t feed) {
    return round(feed / CONVERT_FEED_METER);
};

double_t APRS_MSG::meter2feed(double_t meter) {
    return round(meter * CONVERT_FEED_METER);
};

double_t APRS_MSG::mph2kmh(double_t mph) {
    return round(mph / 0.62137);
};

double_t APRS_MSG::kmh2mph(double_t kmh){
    return round(kmh * 0.62137);
};

double_t APRS_MSG::hinch2mm(double_t hinch){
    return round(hinch/3.93701);
};

double_t APRS_MSG::mm2hinch(double_t mm){
    return round(mm*3.93701);
};

double_t APRS_MSG::f2c(double_t temp){
    return round((temp - 32)*5/9);
};

double_t APRS_MSG::c2f(double_t temp){
    return round(temp*9/5+32);
};

char*  APRS_MSG::dc2gms(char *rv, double gpsdata, boolean lng) {
    char fstr[16] = {0};
    //char retvar[16] = {0};
    char ew;

    if (gpsdata<0 && lng)   { ew = 'W'; }
    if (gpsdata>=0 && lng)  { ew = 'E'; }
    if (gpsdata<0 && !lng)  { ew = 'S'; }
    if (gpsdata>=0 && !lng) { ew = 'N'; }

    if (gpsdata < 0) { gpsdata = -gpsdata; }

    uint8_t grad = gpsdata;
    float tmp = gpsdata - grad;
    tmp = tmp * 60 ;
    uint8_t minute = tmp;
    tmp = tmp - minute;
    uint8_t  secunde = tmp * 60;
    if (lng) {
        strcpy(fstr, "%03d%02d.%02d%c");
    } else {
        strcpy(fstr, "%02d%02d.%02d%c");
    }
    snprintf(rv, 90, fstr,
        grad,
        minute,
        secunde,
        ew
    );
    return rv;
};
