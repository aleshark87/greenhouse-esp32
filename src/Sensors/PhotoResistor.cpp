#include <Sensors/PhotoResistor.h>
#include <Arduino.h>

PhotoResistor::PhotoResistor(const int pin){
    this->pin = pin;
    pinMode(pin, INPUT);
}

int PhotoResistor::getBrightness(){
    long analogValue = analogRead(34);
    float ldrResistance = calculateResistance(analogValue);
    float ldrLux = LUX_CALC_SCALAR * pow(ldrResistance, LUX_CALC_EXPONENT);
    int lux = static_cast<int>(ldrLux);
    if(lux > 260){
        return 260;
    }
    else{
        if(lux >= 0){
            return lux;
        }
        else{
            return -1;
        }
        
    }
}

float calculateResistance(const int analogValue){
    float resistorVoltage = (float)analogValue / ANALOG_VALUES * REF_VCC;
    float ldrVoltage = REF_VCC - resistorVoltage;
    float ldrResistance = ldrVoltage/resistorVoltage * REF_RESISTANCE;
    return ldrResistance;
}

