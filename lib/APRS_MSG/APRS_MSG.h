#ifndef APRS_MSG_H
#define APRS_MSG_H

#include <Arduino.h>

// #ifdef ARDUINO
// #include <Arduino.h>
// #endif

// #ifndef ARDUINO
// #include <iostream>
// #include <tgmath.h>
// #endif


#include <LoRaAPRSConfig.h>


#define CONVERT_FEED_METER 3.28084
#define WX_DEVICE_STR_LENGHT 12
#define CALL_LENGHT 12 // @TODO needs check if it is in APRS OK



class APRS_MSG {
  public:

    enum aprs_symbol_table_e { primary = '/', alternate = '\\'};
  
    // c = wind direction (in degrees).
    void setWindDirection(uint16_t winddirection);


    // s = sustained one-minute wind speed (in mph).
    void setWindSpeedMph(uint16_t windspeedmph);
    void setWindSpeedKmh(uint16_t windspeedkmh);
    // char* getWindSpeedMph(void);
    // char* getWindSpeedKmh(char* retvar);

    // g = gust (peak wind speed in mph in the last 5 minutes).
    void setGustMph(uint16_t gustmph);
    void setGustKmh(uint16_t gustmph);

    // t = temperature (in degrees Fahrenheit).
    void setTemperatureCelsius(int8_t temp);
    void setTemperatureFahrenheit(int8_t temp);

    // r = rainfall (in hundredths of an inch) in the last hour.
    void setRainFallLastHourHInch(uint8_t rainfall);
    void setRainFallLastHourMM(uint8_t rainfall);

    // p = rainfall (in hundredths of an inch) in the last 24 hours.
    void setRainFallLast24HInch(uint8_t rainfall);
    void setRainFallLast24MM(uint8_t rainfall);

    // P = rainfall (in hundredths of an inch) since midnight.
    void setRainFallSinceMidNightHInch(uint8_t rainfall);
    void setRainFallSinceMidNightMM(uint8_t rainfall);

    // h = humidity (in %. 00 = 100%).
    void setHumidity(uint8_t humidity);

    // b = barometric pressure (in tenths of millibars/tenths of hPascal).
    void setPressure(uint16_t pressure);

    void setAPRSSymbolTable(aprs_symbol_table_e stable);
    void setAPRSSymbol(char symbol);

    void reset();

    void setWXDevice(const char *wx_device);

    void setCall(const char *call);
    
    //  getter
    char* getWXField(void);
    char* getPos(void);
    char* getTrackInfo(char* retvar);
    char* getMsg(void);

    char* getAPRSTxPos(char* retvar);
    char* getAPRSTxTrackInfo(char* retvar);
    char* getAPRSTxWXInfo();
    char* getAPRSTxMsg();



    // GPS
    void setLng(double lng);
    void setLat(double lat);
    void setSpeedkmh(double speed);
    void setSpeedmph(double speed);
    void setCourse(double course);
    void setAltitudeMeter(double altitudemeter);
    void setAltitudeFeed(double altitudefeed);
    void setYear(uint16_t year);
    void setMonth(uint8_t month);
    void setDay(uint8_t day);
    void setHour(uint8_t hour);
    void setMinute(uint8_t minute);
    void setSecond(uint8_t second);

    // char* getAltitudeFeed(char* retvar);
    // char* getAltitudeMeter(char* retvar);


    // static helpers 



    /**
     * @brief converts given miles/hour to km/h (imperial2metric)
     * 
     * @param mph miles/hour
     * @return uint16_t km/h
     */
    static uint16_t mph2kmh(uint16_t mph);

    /**
     * @brief converts given km/h to miles/hour (metric2imperial)
     * 
     * @param kmh km/h
     * @return uint16_t mph
     */
    static uint16_t kmh2mph(uint16_t kmh);

    /**
     * @brief converts hunderts of inch to mm (imperial2metric)
     * 
     * @param hinch inch/100
     * @return uint16_t inch/100
     */
    static uint16_t hinch2mm(uint16_t hinch);

    /**
     * @brief converts mm to hunderts of inch (metric2imperial)
     * 
     * @param mm mm
     * @return uint16_t inch/100
     */
    static uint16_t mm2hinch(uint16_t mm);

    /**
     * @brief converts temperature fahrendheit to Celsius (imperial2metric)
     * 
     * @param temp °F
     * @return int16_t °C
     */
    static int16_t f2c(int16_t temp);

    /**
     * @brief converts temperature Celsius to fahrendheit (metric2imperial)
     * 
     * @param temp °C
     * @return int16_t  °F
     */
    static int16_t c2f(int16_t temp);

    /**
     * @brief converts wgs data from dc (decimal) to APRS - gms (grad,minutes,secundes), given from GPS Lib to APRS format
     * 
     * keep in mind the format for APRS differs from normal.
     * 52.2904N in APRS will be 5229.04N!
     * there no negative for S or W, it will be show as W or S
     * 
     * @param retvar _aprs_lng or _aprs_lat (char[10])
     * @param gpsdata lat or lng from GPS
     * @param lng must be true from lng, it sets fromat (xxxxx.xx for lng xxxx.xx for lat and also E/W or N/S)
     * 
     * @see https://www.gpskoordinaten.de/gps-koordinaten-konverter
     * @see https://www.giga.de/extra/gps/tipps/gps-koordinaten-umrechnen-online-oder-mit-formel/
     */
    static void dc2gms(char* retvar, float gpsdata, bool lng);

    static uint32_t feed2meter(uint32_t feed);
    static uint32_t meter2feed(uint32_t meter);


  
    /**
     * @brief APRS symbol table to use
     * @see setAPRSSymbolTable
     */
    char _aprs_symbol_table = primary;

    /**
     * @brief wx APRS symbol in use, - ^= house
     * @see setAPRSSymbol
     */
    char _aprs_symbol = '-';

    /**
     * @brief wx APRS normalized temperature in °F (imperial!)
     * @see setTemperatureCelsius / setTemperatureFahrenheit
     * @see getTemperatureCelsius / getTemperatureFahrenheit
     * @see c2f / f2c
     */
    char _wx_temp[4];

    /**
     * @brief wx APRS wind direction between 0°-360°
     * @see set
     * 
     */
    char _wx_winddirection[4];

    /**
     * @brief wx APRS wind seed in mph (imperial!)
     * @see 
     */
    char _wx_windspeed[4];
    char _wx_gust[4];
    char _wx_rainfalllasthourhinch[4];
    char _wx_rainfalllast24h[4];
    char _wx_rainfallsinceMidnight[4];
    char _wx_humidity[3];
    char _wx_pressure[5];
    char _wx_wx_device[WX_DEVICE_STR_LENGHT + 1];
    char _call[CALL_LENGHT + 1];
    char _wxcall[CALL_LENGHT + 1];
    char _aprs_wx_string[52];

    /**
     * @brief gets full trackinfo with position, speed, course
     * @see _aprs_pos
     */
    char _aprs_trackinfo[41];

    /**
     * @brief gps position and symbol only
     * @see _aprs_trackinfo
     * 
     */
    char _aprs_pos[25];


    char _aprs_lng[10];
    char _aprs_lat[10];
    char _aprs_speed[4];
    char _aprs_course[4];
    char _aprs_altitude[7];

    char _gps_year[5];
    char _gps_month[3];
    char _gps_day[3];
    char _gps_hour[3];
    char _gps_minute[3];
    char _gps_second[3];

  private:  
    void computeWXField(void);  
    void computeAPRSPos(void);
    void computeTrackInfo(void);

};

#endif
