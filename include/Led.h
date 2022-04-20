#ifndef __LED__
#define __LED__
#include <Arduino.h>

class Led {
public:
    Led(const int pin);
    void on();
    void off();
private:
    int pin;
};

#endif