#include <Actuators/Led.h>

Led::Led(const int pin){
    this->pin = pin;
    pinMode(pin, OUTPUT);
}

void Led::on(){
    digitalWrite(pin, HIGH);
    this->state = "on";
}

void Led::off(){
    digitalWrite(pin, LOW);
    this->state = "off";
}

String Led::getState(){
    return this->state;
}