#include "vpio.hh"

Pin::Pin()
{
}

Pin::Pin(uint8_t i)
{
    _idx = i;
}

Pin::Pin(uint8_t i, uint8_t d = OUTPUT)
{
    _idx = i;
    _dir = d;
}
