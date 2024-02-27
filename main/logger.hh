#pragma once

#include <esp_log.h>

class Logger
{
private:
    int         _level;

public:
    Logger();
    void init();
};


