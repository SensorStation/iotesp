#pragma once

#include <cstdint>

const uint8_t INPUT         = 0x0;
const uint8_t OUTPUT        = 0x1;  // 1: ranking.

enum PinType {
    None,
    Digital,
    Analog,
};

class Pin {
private:
    uint8_t     _idx    = -1;
    uint8_t      _dir   = INPUT; 
    PinType     _type   = Digital;

    void*       _val;

public:
    Pin();
    Pin(uint8_t i);
    Pin(uint8_t i, uint8_t d);

};
