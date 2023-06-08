#pragma once

#include <driver/uart.h>

class UART
{
private:

    int                 _pin = 5;          // put in config
    int                 _ring_size = 1024; // put in config

    uart_config_t       _uart_config;

public:
    UART();

    int baud(int b = 0);
    int start();
};
