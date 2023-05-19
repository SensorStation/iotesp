#include <esp_timer.h>

#include "ticker.hh"
#include "sensor.hh"

void ticker_init()
{
    esp_timer_create_args_t periodic_timer_args = {};
    periodic_timer_args.callback = &sensor_reader_ticker;
    periodic_timer_args.name = "periodic";

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 500000));
}

