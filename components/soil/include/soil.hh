#pragma once

#include <esp_log.h>
#include <driver/gpio.h>

class Soil
{
private:
    gpio_num_t  _pin;

    void _init();

public:
    Soil(gpio_num_t p);
    int read(); 
};
