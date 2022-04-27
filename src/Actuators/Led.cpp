#include <Actuators/Led.h>

Led::Led(const int pin){
    this->pin = pin;
    pinMode(pin, OUTPUT);
}

void Led::on(){
    digitalWrite(pin, HIGH);
}

void Led::off(){
    digitalWrite(pin, LOW);
}