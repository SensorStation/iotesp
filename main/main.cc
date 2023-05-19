/* pthread/std::thread example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include <string>
#include <sstream>

#include <esp_log.h>
#include <esp_pthread.h>
#include <esp_timer.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <driver/gpio.h>

#include "dht.hh"
#include "event.hh"
#include "mqtt.hh"
#include "net.hh"

#include "relay.hh"

const gpio_num_t RELAY_PIN     = (gpio_num_t) 5;

using namespace std::chrono;

static const char *TAG = "SensorStation";
static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;
static const gpio_num_t dht_gpio = (gpio_num_t)18;

const auto sleep_time   = seconds { 5 };
const auto relay_pin    = 5;

esp_timer_handle_t TIMER;

void sensor_reader_ticker(void* arg)
{
    if (mqtt_is_running == false) {
        return;
    }

    // DHT sensors that come mounted on a PCB generally have
    // pull-up resistors on the data pin.  It is recommended
    // to provide an external pull-up resistor otherwise...

    // gpio_set_pull_mode(dht_gpio, GPIO_PULLUP_ONLY);
    // while (1) {
    int16_t temperature;
    int16_t humidity;

    auto err = dht_read_data(sensor_type, dht_gpio, &humidity, &temperature);
    if (err != ESP_OK) {
        printf("Could not read data from sensor\n");
        return;
    }
    event_value t;
    event_value h;

    t.value = temperature;
    h.value = humidity;

    // post a publication event
    // ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_STOPPED, NULL, 0, portMAX_DELAY));
    ESP_ERROR_CHECK(esp_event_post(PUBLICATION_EVENTS,
                                   EVENT_PUBLICATION_TEMPC,
                                   static_cast<void*>(&t),
                                   sizeof(temperature),
                                   portMAX_DELAY));

    ESP_ERROR_CHECK(esp_event_post(PUBLICATION_EVENTS,
                                   EVENT_PUBLICATION_HUMIDITY,
                                   static_cast<void *>(&h),
                                   sizeof(humidity),
                                   portMAX_DELAY));

}

void log_init()
{
    ESP_LOGI(TAG, "[APP] Sensor Station is Starting");
    ESP_LOGI(TAG, "[APP] Free memory: %ld bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_WARN);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_WARN);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_WARN);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_WARN);
    esp_log_level_set("esp-tls", ESP_LOG_WARN);
    esp_log_level_set("TRANSPORT", ESP_LOG_WARN);
    esp_log_level_set("OUTBOX", ESP_LOG_WARN);
}

void ticker_init()
{
    esp_timer_create_args_t periodic_timer_args = {};
    periodic_timer_args.callback = &sensor_reader_ticker;
    periodic_timer_args.name = "periodic";

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 500000));
}

extern "C" void app_main(void)
{
    log_init();
    net_start();

    events_start();
    mqtt_start();
    ticker_init();

    std::cout << "<<<<< Sleepy loop timer >>>>>>" << std::endl;
    while(true) {
        std::this_thread::sleep_for(sleep_time);        
    }
}
