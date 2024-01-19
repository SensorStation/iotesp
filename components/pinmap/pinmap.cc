#include "pinmap.hh"

Pin::Pin() {}

Pin::Pin(int p)
{
    _pin = (gpio_num_t) p;
}

Pin::Pin(gpio_num_t p)
{
    _pin = (gpio_num_t) p;
}

gpio_num_t Pin::pin(gpio_num_t p = (gpio_num_t) -1)
{
    if (p > -1) {
        _pin = p;        
    }
    return _pin;
}

std::string Pin::description(std::string d = "")
{
    if (d != "") {
        _description = d;
    }
    return _description;
}
