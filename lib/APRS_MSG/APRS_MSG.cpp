
#include <APRS_MSG.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/***************************************************************************/
/*************************   WX gettter / setter   *************************/
/***************************************************************************/

// c... = wind direction (in degrees).
void APRS_MSG::setWindDirection(uint16_t winddirection){
    snprintf(_wx_winddirection, 4, "%03u", winddirection);
};



// s... = sustained one-minute wind speed (in mph).
void APRS_MSG::setWindSpeedMph(uint16_t windspeedmph){
    snprintf(_wx_windspeed, 4, "%03u", windspeedmph);
};

void APRS_MSG::setWindSpeedKmh(uint16_t windspeedkmh){
    setWindSpeedMph(kmh2mph(windspeedkmh));
};

// char* APRS_MSG::getWindSpeedMph(void) {
//     return _wx_windspeed;
// };

// char* APRS_MSG::getWindSpeedKmh(char* retvar) {
//     snprintf(retvar, 4, "%03d", mph2kmh(atof(_wx_windspeed)));
//     return retvar;
// };


// g... = gust (peak wind speed in mph in the last 5 minutes).
void APRS_MSG::setGustMph(uint16_t gustmph){
    snprintf(_wx_gust, 4, "%03u", gustmph);
};


void APRS_MSG::setGustKmh(uint16_t gustmph){
    snprintf(_wx_gust, 4, "%03u", kmh2mph(gustmph));
};


// r... = rainfall (in hundredths of an inch) in the last hour.
void APRS_MSG::setRainFallLastHourHInch(uint8_t rainfallhinch){
    snprintf(_wx_rainfalllasthourhinch, 4, "%03u", rainfallhinch);
};


void APRS_MSG::setRainFallLastHourMM(uint8_t rainfallmm){
    snprintf(_wx_rainfalllasthourhinch, 4, "%03u", mm2hinch(rainfallmm));
};



// p... = rainfall (in hundredths of an inch) in the last 24 hours.
void APRS_MSG::setRainFallLast24HInch(uint8_t rainfallhinch){
    snprintf(_wx_rainfalllast24h, 4, "%03u", rainfallhinch);
};


void APRS_MSG::setRainFallLast24MM(uint8_t rainfallmm){
    snprintf(_wx_rainfalllast24h, 4, "%03u", mm2hinch(rainfallmm));
};



// P... = rainfall (in hundredths of an inch) since midnight.
void APRS_MSG::setRainFallSinceMidNightHInch(uint8_t rainfallhinch){
    snprintf(_wx_rainfallsinceMidnight, 4, "%03u", rainfallhinch);
};


void APRS_MSG::setRainFallSinceMidNightMM(uint8_t rainfallmm){
    snprintf(_wx_rainfallsinceMidnight, 4, "%03u", mm2hinch(rainfallmm));
};


// t... = temperature (in degrees Fahrenheit). 
void APRS_MSG::setTemperatureCelsius(int8_t tempc) {
   snprintf( _wx_temp, 4, "%03d", c2f(tempc));
};

void APRS_MSG::setTemperatureFahrenheit(int8_t tempf) {
    snprintf(_wx_temp, 4, "%03d", tempf);
};


// h.. = humidity (in %. 00 = 100%).
void APRS_MSG::setHumidity(uint8_t humidity){
    if (humidity == 100) {
        snprintf(_wx_humidity, 3, "%s", "00");
    } else {
       snprintf(_wx_humidity, 3, "%02u", humidity);
    }
};


// b.... = barometric pressure (in tenths of millibars/tenths of hPascal).
void APRS_MSG::setPressure(uint16_t pressure){
    snprintf(_wx_pressure, 5, "%04u", pressure);
};


void APRS_MSG::setWXDevice(const char* wx_device){
    snprintf(_wx_wx_device, 13, "%s", wx_device);
};


/***************************************************************************/
/*************************   GPS getter / setter   *************************/
/***************************************************************************/


void APRS_MSG::setAPRSSymbolTable(aprs_symbol_table_e table) {
    _aprs_symbol_table = table;
};

void APRS_MSG::setAPRSSymbol(char symbol) {
    _aprs_symbol = symbol;
};

//gps.location.lng();
void APRS_MSG::setLng(double lng) {
    dc2gms(_aprs_lng, lng, true);
};

//gps.location.lat();
void APRS_MSG::setLat(double lat) {
    dc2gms(_aprs_lat, lat, false);
    //Serial.printf("APRS_MSG::Lat: %s\n", _aprs_lat);
};

void APRS_MSG::setSpeedmph(double speed){
    snprintf(_aprs_speed, 4, "%03d", int(speed));
};

void APRS_MSG::setSpeedkmh(double speed){
    setSpeedmph(kmh2mph(speed));
};



void APRS_MSG::setCourse(double course){
    snprintf(_aprs_course, 4, "%03d", int(course));
};


void APRS_MSG::setAltitudeMeter(double altitudemeter){
    setAltitudeFeed(meter2feed(altitudemeter));
};

void APRS_MSG::setAltitudeFeed(double altitudefeed){
    snprintf(_aprs_altitude, 7, "%06d", (uint32_t)(altitudefeed));
};

// char* APRS_MSG::getAltitudeFeed(char* retvar) {
//     snprintf(retvar, 7, "%d", (uint32_t) atof(_aprs_altitude));
//     return retvar;
// };

// char* APRS_MSG::getAltitudeMeter(char* retvar) {
//     snprintf(retvar, 7, "%d", feed2meter((uint32_t) atof(_aprs_altitude)));
//     return retvar;
// };

/***************************************************************************/
/*************************    GPS date / time     **************************/
/***************************************************************************/


void APRS_MSG::setYear(uint16_t year){
    snprintf(_gps_year, 5, "%d", year);
};

void APRS_MSG::setMonth(uint8_t month){
    snprintf(_gps_month, 3, "%02d", month);
};
void APRS_MSG::setDay(uint8_t day){
    snprintf(_gps_day, 3, "%02d", day);
};
void APRS_MSG::setHour(uint8_t hour){
    snprintf(_gps_hour, 3, "%02d", hour);
};
void APRS_MSG::setMinute(uint8_t minute){
    snprintf(_gps_minute, 3, "%02d", minute);
};
void APRS_MSG::setSecond(uint8_t second){
    snprintf(_gps_second, 3, "%02d", second);
};




/***************************************************************************/
/*************************  compute APRS fields   **************************/
/***************************************************************************/

void APRS_MSG::reset() {
    //Serial.printf("call APRS_MSG::reset\n");
    _aprs_symbol_table = primary;
    //@FIXME has to come from registy
    _aprs_symbol = '[';
    strncpy(_call, "DL7UXA-3", CALL_LENGHT + 1);
    // @FIXME has to come from registry
    strncpy(_wxcall, "DL7UXA-5", CALL_LENGHT + 1);
    strcpy(_wx_winddirection, "...");
    strcpy(_wx_windspeed ,"...");
    strcpy(_wx_gust ,"...");
    strcpy(_wx_rainfalllasthourhinch ,"...");
    strcpy(_wx_rainfalllast24h ,"...");
    strcpy(_wx_rainfallsinceMidnight ,"...");
    strcpy(_wx_temp ,"...");
    strcpy(_wx_humidity ,"..");
    strcpy(_wx_pressure ,"....");
    strcpy(_aprs_lat ,"........");
    strcpy(_aprs_lng ,"........");
    strcpy(_aprs_speed ,"...");
    strcpy(_aprs_course ,"...");
    
}

void APRS_MSG::computeWXField(void) {
    snprintf(_aprs_wx_string, 54, "%s/%sg%st%sr%sp%sP%sh%sb%s%s",
    _wx_winddirection,
    _wx_windspeed,
    _wx_gust,
    _wx_temp,
    _wx_rainfalllasthourhinch,
    _wx_rainfalllast24h,
    _wx_rainfallsinceMidnight,
    _wx_humidity,
    _wx_pressure,
    _wx_wx_device
    );
    setAPRSSymbolTable(primary);
    setAPRSSymbol('_'); // wx-station
}

char* APRS_MSG::getWXField() {
    computeWXField();
    return _aprs_wx_string;
};

void APRS_MSG::computeAPRSPos(void) {
    //Serial.printf("call APRS_MSG::computeAPRSPos\n");
    //Serial.printf("APRS_MSG::computeAPRSPos lat %s\n", _aprs_lat);
    snprintf(_aprs_pos, 25, "%s%c%s%c",
        _aprs_lat,
        _aprs_symbol_table,
        _aprs_lng,
        _aprs_symbol
        );
    //Serial.printf("plain: %s%c%s%c\n", _aprs_lat, _aprs_symbol_table, _aprs_lng, _aprs_symbol);
    //Serial.printf("computed: %s\n", _aprs_pos);
};


char* APRS_MSG::getPos(void) {
    computeAPRSPos();
    return _aprs_pos;
};

void APRS_MSG::computeTrackInfo(void) {
    computeAPRSPos();
    snprintf(_aprs_trackinfo, 41, "%s%s/%s/A=%s",
        _aprs_pos,
        _aprs_speed,
        _aprs_course,
        _aprs_altitude
        );
}

char* APRS_MSG::getAPRSTxTrackInfo(char* retvar) {
    computeTrackInfo();
    snprintf(retvar, 60, "%s>APRS:!%s", _call, _aprs_trackinfo);
    return retvar;
};

char* APRS_MSG::getAPRSTxPos(char* retvar) {
    computeAPRSPos();
    snprintf(retvar, 254, "%s>APRS:!%s", _call, _aprs_pos);
    //Serial.printf("APRS_MSG::getAPRSTxPos retvar '%s'\n", retvar);
    return retvar;

//DL7UXA-1>APRS:!
};

void APRS_MSG::setCall(const char *call) {
    snprintf(_call, 13, call);
}

/***************************************************************************/
/**************************   static converters   **************************/
/***************************************************************************/
uint32_t APRS_MSG::feed2meter(uint32_t feed) {
    return round(feed / CONVERT_FEED_METER);
};

uint32_t APRS_MSG::meter2feed(uint32_t meter) {
    return round(meter * CONVERT_FEED_METER);
};

uint16_t APRS_MSG::mph2kmh(uint16_t mph) {
    return round(mph / 0.62137);
};

uint16_t APRS_MSG::kmh2mph(uint16_t kmh){
    return round(kmh * 0.62137);
};

uint16_t APRS_MSG::hinch2mm(uint16_t hinch){
    return round(hinch/3.93701);
};

uint16_t APRS_MSG::mm2hinch(uint16_t mm){
    return round(mm*3.93701);
};

int16_t APRS_MSG::f2c(int16_t temp){
    return round((temp - 32)*5/9);
};

int16_t APRS_MSG::c2f(int16_t temp){
    return round(temp*9/5+32);
};

void APRS_MSG::dc2gms(char* retvar, float gpsdata, bool lng) {
    char fstr[16] = "";
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
    snprintf(retvar, 90, fstr,
        grad,
        minute,
        secunde,
        ew
    );
};