#include <string>

#include <esp_log.h>
#include <esp_timer.h>

#include "ticker.hh"
#include "mqtt.hh"

static const char* TAG = "TICKER";
extern int16_t temperature;
extern int16_t humidity;

void periodic_timer_callback(void* arg)
{
    if (mqtt_is_running == false) {
        return;
    }

    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us",
             time_since_boot);

    char timestr[80];
    snprintf((char*)&timestr, 80, "%lld", time_since_boot);
    std::string path = "ss/data/";
    std::string ip = "10.11.44.21";
    std::string rest = "/uptime";
    std::string foo = path + ip + rest;
    int msg_id = esp_mqtt_client_publish(mqtt_client,
                                         foo.c_str(),
                                         // "ss/data/" + ip.c_str() + "/uptime",
                                         timestr, 0, 1, 0);
    char str[8];
    sprintf(str, "%3.2f", (double) temperature / 10);
    rest = "/tempc";
    foo = path + ip + rest;
    msg_id = esp_mqtt_client_publish(mqtt_client,
                                     foo.c_str(),
                                     str, 0, 1, 0);

    sprintf(str, "%3.2f", (double) humidity / 10);
    rest = "/hum";
    foo = path + ip + rest;
    msg_id = esp_mqtt_client_publish(mqtt_client,
                                     foo.c_str(),
                                     str, 0, 1, 0);

    ESP_LOGI(TAG, "periodic timer publish, msg_id=%d", msg_id);
}

void ticker_init()
{
    esp_timer_create_args_t periodic_timer_args = {};
    periodic_timer_args.callback = &periodic_timer_callback;
    periodic_timer_args.name = "periodic";

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 500000));
}
