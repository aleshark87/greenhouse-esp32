#ifndef __DHT11SENSOR__
#define __DHT11SENSOR__

#define DHT_TYPE DHT11

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class DHT11Sensor {
public:
    DHT11Sensor(const int pin);
    float getTemperature();
    float getHumidity();
private:
    int pin;
    DHT_Unified* dht;
    sensors_event_t event;
};

#endif