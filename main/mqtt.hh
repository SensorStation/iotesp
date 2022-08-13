#pragma once

#include <string>

#include <mqtt_client.h>

extern esp_mqtt_client_handle_t mqtt_client;
extern bool mqtt_is_running;

extern void mqtt_start();
extern void mqtt_publish(std::string topic, int val);
