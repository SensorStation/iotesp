#pragma once

#include <map>
#include <string>

#include "dht.hh"
#include "relay.hh"

class Station
{
private:
    std::string _id;
    std::string _ipaddr;

    char _json[256];

public:
    Station();

    DHT *dht;
    Relays *relays;

    void start_reading();
    char *json();
};

extern Station *station;
