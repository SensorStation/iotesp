#pragma once

#include "driver/gpio.h"

const int ON    = 0;
const int OFF   = 1;

const auto relay_pin = 5;

class Relay {
private:    
    gpio_num_t  _pin;
    bool        _isOn = false;

public:
    Relay(int p) {
        _pin = (gpio_num_t) p;
        gpio_reset_pin(_pin);
        gpio_set_direction(_pin, GPIO_MODE_OUTPUT);
    }

    void on()   { gpio_set_level( _pin, ON ); }
    void off()  { gpio_set_level( _pin, OFF ); }
    
    bool IsOn() { return _isOn; }
};
   
extern Relay *relay; 
