#pragma once

#include <string>
#include <driver/gpio.h>

static const int MAX_PINS = 38;

class Pin
{
private:
    std::string _description;

protected:
    gpio_num_t  _pin = (gpio_num_t) -1;

public:
    Pin();
    Pin(int p);
    Pin(gpio_num_t p);

    gpio_num_t pin(gpio_num_t p);

    std::string description(std::string d);
};

class Pinmap
{
private:
    

public:
    Pinmap();
};
