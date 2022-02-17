#include <esp_log.h>

#include "ticker.hh"
#include "mqtt.hh"

static const char* TAG = "TICKER";

void periodic_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG, "Periodic timer called, time since boot: %lld us",
             time_since_boot);
    char timestr[80];
    snprintf((char*)&timestr, 80, "%lld", time_since_boot);
    int msg_id = esp_mqtt_client_publish(mqtt_client,
                                     "ss/data/10.55.13.13/uptime",
                                     timestr, 0, 1, 0);
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
