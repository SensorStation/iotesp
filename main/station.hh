#pragma once

#include <map>
#include <string>

#include "dht.hh"
#include "relay.hh"
#include "oled.hh"

class Station
{
private:
    std::string _id;
    std::string _ipaddr;
    std::string _json;

public:
    Station();

    DHT         *dht;
    Relays      *relays;
    OLED        *oled;

    void update_display();

    void start_reading();
    std::string json();
};

