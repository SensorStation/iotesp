#pragma once

#include <iostream>
#include <string.h>
#include <string>

#include <esp_log.h>

enum Category {
    CATEGORY_NONE,
    CONTROL,
    DATA,
};

enum Device {
    DEVICE_NONE,
    RELAY,
};

using namespace std;

const int MAX_ARRAY_ELEMENTS = 10;

struct Msg
{
public:
    // Device      device;
    Category    category;
    string      source;
    string      dtype;
    string      device;
    uint32_t    index;
    
    bool        on;

public:
    Msg(char *topic, char *data);
    bool to_bool();
};
