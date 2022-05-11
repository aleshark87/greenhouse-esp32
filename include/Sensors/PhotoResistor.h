#ifndef __PHOTORESISTOR__
#define __PHOTORESISTOR__
#include <Arduino.h>

#define LUX_CALC_SCALAR 6207.987
#define LUX_CALC_EXPONENT -0.522
#define REF_RESISTANCE 5000
#define REF_VCC 5
#define ANALOG_VALUES 4095

class PhotoResistor {
public:
    PhotoResistor(const int pin);
    int getBrightness();
private:
    int pin;
};

#endif