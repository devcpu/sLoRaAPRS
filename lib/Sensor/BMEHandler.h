#ifndef BME_HANDLER_H
#define BME_HANDLER_H

#include <Adafruit_BME280.h>
#include <Adafruit_Sensor.h>

// 1013.25 ^= -50.96m
// 1100.00 ^= 634.93m
// 1033.25 ^= 116.89m
// 1029.57 ^= 83.80 m
#define SEALEVELPRESSURE_HPA (1029.57)

bool BMEHandlerInit(void);
void setWXData(void);

extern Adafruit_BME280 bme;

#endif