#pragma once

#include <array>
#include <map>
#include <string>

#include <esp_log.h>

#include "driver/gpio.h"

const int ON    = 0;
const int OFF   = 1;

const auto relay_pin = 5;

class Relay {
private:
    std::string _id;
    gpio_num_t  _pin;
    bool        _is_on = false;

    std::string _json;

public:
    Relay(int p, std::string id);

    void on();
    void off();
    
    bool is_on();

    std::string json();
};
   
class Relays
{
private:
    std::map<std::string, Relay*> _relays;
    std::string _json;

public:
    Relays();

    void add(int pin, std::string name);

    Relay* get(std::string name);
    std::string json();
};

extern Relays relays;
