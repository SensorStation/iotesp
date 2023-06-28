#pragma once

#include <array>
#include <esp_log.h>

#include "driver/gpio.h"

const int ON    = 0;
const int OFF   = 1;

const auto relay_pin = 5;

class Relay {
private:    
    gpio_num_t  _pin;
    bool        _is_on = false;

public:
    Relay(int p);

    void on();
    void off();
    
    bool is_on();
};
   
const int RELAY_MAX = 4;
class Relays
{
private:
    Relay*      _relays[RELAY_MAX];
    int         _relay_count;
    
public:
    Relays();

    int add(int pin);
    Relay* get(int idx);
};

extern Relays relays;
