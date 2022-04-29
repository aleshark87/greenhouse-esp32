#include <Sensors/DHT11Sensor.h>

DHT11Sensor::DHT11Sensor(const int pin){
    this->pin = pin;
    this->dht = new DHT_Unified(4, DHT_TYPE);
    this->dht->begin();
}

// rounds a number to 2 decimal places
// example: round(3.14159) -> 3.14
double round2(double value) {
   return (int)(value * 100 + 0.5) / 100.0;
}

double DHT11Sensor::getTemperature(){
    this->dht->temperature().getEvent(&event);
    double roundedTemp = round2(event.temperature);
    return roundedTemp;
}

int DHT11Sensor::getHumidity(){
    this->dht->humidity().getEvent(&event);
    return event.relative_humidity;
}

