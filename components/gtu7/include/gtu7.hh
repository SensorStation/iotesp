#pragma once

#include <chrono>

#include "vpio.hh"

struct location_t
{
    float latitude;
    float longitude;
    float elevation;

};

class GTU7 {
private:
    Pin* _tx;
    Pin* _rx;

    location_t loc;
    location_t last_loc;

public:
    GTU7() {}
    GTU7(uint8_t tx, uint8_t rx);

    int init();

    location_t *location();
    location_t *location(location_t *loc);
};
