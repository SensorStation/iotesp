#pragma once

#include <string>
#include <esp_wifi.h>

#define MAC_ADDR_SIZE   6

class Net
{
private:
    uint8_t _mac[MAC_ADDR_SIZE];
    std::string macstr;

public:
    Net();

    uint8_t *get_mac();
    std::string mac2str();
};

extern Net *net;
