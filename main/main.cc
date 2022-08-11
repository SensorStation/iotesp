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

#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_pthread.h>
#include <esp_timer.h>

#include <nvs_flash.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "protocol_examples_common.h"

#include "dht.hh"
#include "event.hh"
#include "mqtt.hh"
#include "ticker.hh"

using namespace std::chrono;

static const char *TAG = "SensorStation";
static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;
static const gpio_num_t dht_gpio = (gpio_num_t)18;

const auto sleep_time = seconds { 5 };

struct Value {
    int16_t value;
};


/* Event source periodic timer related definitions */
ESP_EVENT_DEFINE_BASE(PUBLICATION_EVENTS);
ESP_EVENT_DEFINE_BASE(TIMER_EVENTS);

esp_timer_handle_t TIMER;

static char* get_id_string(esp_event_base_t base, int32_t id) {
    char* event = (char *)"";

    if (base == TIMER_EVENTS) {
        switch(id) {
        case EVENT_TIMER_STARTED:
            event = (char *) "EVENT_TIMER_STARTED";
            break;

        case EVENT_TIMER_EXPIRY:
            event = (char *) "EVENT_TIMER_EXPIRY";
            break;

        case EVENT_TIMER_STOPPED:
            event = (char *) "EVENT_TIMER_STOPPED";
            break;
        }

    } else if (base == PUBLICATION_EVENTS ){

        switch (id) {
        case EVENT_PUBLICATION_TEMPC:
            event = (char *) "EVENT_PUBLICATION_TEMPC";
            break;

        case EVENT_PUBLICATION_HUMIDITY:
            event = (char *) "EVENT_PUBLICATION_HUMIDITY";
            break;
        }

    } else {

        event = (char *) "TASK_ITERATION_EVENT";
    }

    return event;
}

void net_start()
{
    // Start running different aspects of the system
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* This helper function configures Wi-Fi or Ethernet, as selected
     * in menuconfig.  Read "Establishing Wi-Fi or Ethernet
     * Connection" section in examples/protocols/README.md for more
     * information about this function.
     */
    ESP_ERROR_CHECK(example_connect());
}

static void publish(std::string topic, int val) {
    std::string root = "ss/data/";
    std::string ip = "10.11.44.21";
    auto path = root + ip + "/" + topic;

    char str[8];
    sprintf(str, "%3.2f", (double) val / 10);
    auto msg_id = esp_mqtt_client_publish(mqtt_client,
                                          path.c_str(),
                                          str, 0, 1, 0);
    ESP_LOGI(TAG, "periodic timer publish, msg_id=%d", msg_id);
}

static void data_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    Value *val = static_cast<Value*>(event_data);
    ESP_LOGI(TAG, "%s:%s - %d: data_handler", base, get_id_string(base, id), val->value);

    if (base != PUBLICATION_EVENTS) {
        ESP_LOGW(TAG, "unwanted event: %s:%s - %d: data_handler", base, get_id_string(base, id), val->value);
        return;
    }

    switch (id) {
    case EVENT_PUBLICATION_TEMPC:
        publish("tempc", val->value);
        break;

    case EVENT_PUBLICATION_HUMIDITY:
        publish("humidity", val->value);
        break;

    default:
        ESP_LOGW(TAG, "unknow event id: %s:%s - %d: data_handler", base, get_id_string(base, id), val->value);
        return;
    }
}

void events_start()
{
    // Create the default event loop
    ESP_ERROR_CHECK(esp_event_handler_register(PUBLICATION_EVENTS, EVENT_PUBLICATION_TEMPC, data_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(PUBLICATION_EVENTS, EVENT_PUBLICATION_HUMIDITY, data_handler, NULL));
}

void sensor_reader_cb(void* arg)
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

    if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK) {
        printf("Humidity: %d%% Temp: %dc\n", humidity / 10, temperature / 10);
    } else {
        printf("Could not read data from sensor\n");            
    }

    Value t;
    Value h;

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
    periodic_timer_args.callback = &sensor_reader_cb;
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

    // xTaskCreate(dht_test, "dht_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);

    std::cout << "<<<<< Sleepy loop timer >>>>>>" << std::endl;
    while(true) {
        std::this_thread::sleep_for(sleep_time);        
    }
}
