#include <PhotoResistor.h>
#include <Arduino.h>

#define LUX_CALC_SCALAR 6207.987
#define LUX_CALC_EXPONENT -0.522
#define REF_RESISTANCE 10000
#define REF_VCC 5

PhotoResistor::PhotoResistor(const int pin){
    this->pin = pin;
    pinMode(pin, INPUT);
}

int PhotoResistor::getBrightness(){
    long analogValue = analogRead(34);
    float resistorVoltage = (float)analogValue / 4095 * 5;
    float ldrVoltage = REF_VCC - resistorVoltage;
    float ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;

    float ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
    int lux = static_cast<int>(ldrLux);
    if(lux > 260){
        return 260;
    }
    else{
        return lux;
    }
}

