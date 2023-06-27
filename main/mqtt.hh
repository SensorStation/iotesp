#pragma once

#include <string>
#include <vector>
#include <mqtt_client.h>

using namespace std;

class MQTT
{
private:
    bool                _running = false;
    string              _broker;
    vector<string>      _subscriptions;

    esp_mqtt_client_handle_t _client;

public:
    MQTT(string broker);

    void subscribe(esp_mqtt_client_handle_t c, string topic);

    void publish(std::string topic, int val);
    bool running() { return _running; }
    bool running(bool v) { _running = v; return _running; }
};

// extern esp_mqtt_client_handle_t mqtt_client;
// extern void mqtt_start();
// extern void mqtt_publish(std::string topic, int val);

extern MQTT *mqtt;
