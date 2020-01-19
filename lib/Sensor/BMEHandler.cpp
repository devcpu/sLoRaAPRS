#include <Arduino.h>
#include <BMEHandler.h>
#include <Registry.h>
#include <uxa_debug.h>
#include <LoRaAPRSConfig.h>

extern Registry reg;  // config & system status

// 1013.25 ^= -50.96m
// 1100.00 ^= 634.93m
// 1033.25 ^= 116.89m
// 1029.57 ^= 83.80 m
#define SEALEVELPRESSURE_HPA (1029.57)
Adafruit_BME280 bme;

float temperature = 0;
float pressure = 0;
float humidity = 0;
float altitude = 0;
bool bme280_ok = false;

// bool bmestatus =

bool BMEHandlerInit() {
  
  if (bme.begin(BME280_ADDRESS)) {
    Serial.printf("found BME280 on %#x\n", BME280_ADDRESS);
    bme280_ok = true;
  } else if (bme.begin(BME280_ADDRESS_ALTERNATE)) {
    Serial.printf("found BME280 on %#x\n", BME280_ADDRESS);
    bme280_ok = true;
  } else {
    Serial.printf("ERR> sorry, no BME280 found on %x nor %x\n", BME280_ADDRESS, BME280_ADDRESS_ALTERNATE);
    bme280_ok = false;
    return false;
  }

  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1,  // temperature
                  Adafruit_BME280::SAMPLING_X1,  // pressure
                  Adafruit_BME280::SAMPLING_X1,  // humidity
                  Adafruit_BME280::FILTER_OFF);

  // suggested rate is 1 / 60Hz(1m)
  bme.takeForcedMeasurement();
  temperature = bme.readTemperature();
  pressure = bme.readPressure() / 100.00F;
  humidity = bme.readHumidity();
  altitude = bme.readAltitude(SEALEVELPRESSURE_HPA);

  Serial.println("++++++++++++++++++++++++++++++++++++++++");
  Serial.printf("+   Temperature: %2.3f°C\n", temperature);
  Serial.printf("+   Humidity: %3.4f%\n", humidity);
  Serial.printf("+   Pressure: %4.6fhPa\n", pressure);
  Serial.printf("+   Approx. Altitude: %5.5fm\n", altitude);
  Serial.println("++++++++++++++++++++++++++++++++++++++++");
  return true;
}

void setWXData(void) {
  reg.WXdata.humidity = humidity;
  reg.WXdata.temp = temperature;
  reg.WXdata.pressure = pressure;
}

void Sensor_tick() {
  static uint64_t next_sensor_update = 0;
  if (next_sensor_update > millis()) {
    return;
  } else {
    setWXData();
    next_sensor_update = SENSOR_UPDATE_INTERVAL + millis();
  }
}