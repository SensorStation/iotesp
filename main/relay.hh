#pragma once

#include "driver/gpio.h"

const int OFF   = 0;
const int ON    = 1;

class Relay {
private:    
    gpio_num_t  _pin;
    bool        _isOn = false;

public:
    Relay(gpio_num_t p) {
        _pin = p;
        gpio_reset_pin(_pin);
        gpio_set_direction(_pin, GPIO_MODE_OUTPUT);
    }

    void On()   { gpio_set_level( _pin, ON ); }
    void Off()  { gpio_set_level( _pin, OFF ); }
    
    bool IsOn() { return _isOn; }
};
   
