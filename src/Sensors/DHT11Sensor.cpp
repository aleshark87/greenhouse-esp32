#include <Sensors/DHT11Sensor.h>

DHT11Sensor::DHT11Sensor(const int pin){
    this->pin = pin;
    this->dht = new DHT_Unified(4, DHT_TYPE);
    this->dht->begin();
}

float DHT11Sensor::getTemperature(){
    this->dht->temperature().getEvent(&event);
    return event.temperature;
}

float DHT11Sensor::getHumidity(){
    this->dht->humidity().getEvent(&event);
    return event.relative_humidity;
}

