#pragma once

#include <string>
#include <mqtt_client.h>

class MQTT
{
private:
    esp_mqtt_client_handle_t _client;

    bool        _running = false;

public:
    MQTT();

    void publish(std::string topic, int val);
    bool running() { return _running; }
    bool running(bool v) { _running = v; return _running; }
};

// extern esp_mqtt_client_handle_t mqtt_client;
// extern void mqtt_start();
// extern void mqtt_publish(std::string topic, int val);

extern MQTT *mqtt;
