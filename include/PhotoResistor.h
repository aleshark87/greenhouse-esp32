#ifndef __PHOTORESISTOR__
#define __PHOTORESISTOR__
#include <Arduino.h>

class PhotoResistor {
public:
    PhotoResistor(const int pin);
    int getBrightness();
private:
    int pin;
};

#endif