#pragma once

#include <esp_event.h>

// Declare an event base
ESP_EVENT_DECLARE_BASE(CONTROL_EVENTS);
ESP_EVENT_DECLARE_BASE(DATA_EVENTS);
ESP_EVENT_DECLARE_BASE(TIMER_EVENTS); 


enum EVENT_TIMER_TYPE {
    EVENT_TIMER_STARTED,
    EVENT_TIMER_EXPIRY,
    EVENT_TIMER_STOPPED
};

enum EVENT_DATA_TYPE {
    EVENT_DATA_TEMPC,
    EVENT_DATA_HUMIDITY,
};

enum EVENT_CONTROL_TYPE {
    EVENT_CONTROL_NONE,
    EVENT_CONTROL_RELAY,
    EVENT_CONTROL_SWITCH,
};

union event_value {
    int         i;
    int8_t      i8;
    int16_t     i16;
    int32_t     i32;
    bool        b;
    void*       v;
};

extern void events_init();
