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
    std::string _json;

public:
    Station();

    DHT         *dht;
    Relays      *relays;

    void start_reading();
    std::string json();
};

