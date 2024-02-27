#pragma once

#include <map>
#include <string>

#include "dht.hh"
#include "oled.hh"
#include "relay.hh"
#include "soil.hh"

class Station
{
private:
    std::string _id;
    std::string _ipaddr;
    std::string _json;

public:
    Station();

    DHT         *dht;
    OLED        *oled;
    // Soil        *soil;
    Relays      *relays;


    void update_display();

    void start_reading();
    std::string json();
};

