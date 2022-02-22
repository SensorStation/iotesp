#pragma once

#include <mqtt_client.h>

extern esp_mqtt_client_handle_t mqtt_client;
extern void mqtt_start();
extern bool mqtt_is_running;
