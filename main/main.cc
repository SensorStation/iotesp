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
static const gpio_num_t dht_gpio = (gpio_num_t)5;

const auto sleep_time = seconds { 5 };

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

static void data_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    int *val = static_cast<int*>(event_data);
    ESP_LOGI(TAG, "%s:%s - %d: data_handler", base, get_id_string(base, id), *val);
}

int16_t temperature = 0;
int16_t humidity = 0;

void dht_test(void *pvParameters)
{
    int16_t *temp, *hum;

    // DHT sensors that come mounted on a PCB generally have
    // pull-up resistors on the data pin.  It is recommended
    // to provide an external pull-up resistor otherwise...

    // gpio_set_pull_mode(dht_gpio, GPIO_PULLUP_ONLY);

    while (1) {
        if (dht_read_data(sensor_type, dht_gpio, &humidity, &temperature) == ESP_OK) {
            printf("Humidity: %d%% Temp: %dC\n", humidity / 10, temperature / 10);
        } else {
            printf("Could not read data from sensor\n");            
        }

        temp = &temperature;
        hum  = &humidity;

        // post a publication event
        // ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_STOPPED, NULL, 0, portMAX_DELAY));
        ESP_ERROR_CHECK(esp_event_post(PUBLICATION_EVENTS,
                                       EVENT_PUBLICATION_TEMPC,
                                       static_cast<void*>(temp),
                                       sizeof(temperature),
                                       portMAX_DELAY));
        ESP_ERROR_CHECK(esp_event_post(PUBLICATION_EVENTS,
                                       EVENT_PUBLICATION_HUMIDITY,
                                       static_cast<void *>(hum),
                                       sizeof(humidity),
                                       portMAX_DELAY));

        // If you read the sensor data too often, it will heat up
        // http://www.kandrsmith.org/RJS/Misc/Hygrometers/dht_sht_how_fast.html
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}


void log_start()
{
    ESP_LOGI(TAG, "[APP] Sensor Station is Starting");
    ESP_LOGI(TAG, "[APP] Free memory: %ld bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set("MQTT_CLIENT", ESP_LOG_VERBOSE);
    esp_log_level_set("MQTT_EXAMPLE", ESP_LOG_VERBOSE);
    esp_log_level_set("TRANSPORT_BASE", ESP_LOG_WARN);
    esp_log_level_set("esp-tls", ESP_LOG_WARN);
    esp_log_level_set("TRANSPORT", ESP_LOG_WARN);
    esp_log_level_set("OUTBOX", ESP_LOG_WARN);
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

void events_start()
{
    // Create the default event loop
    // ESP_ERROR_CHECK(esp_event_handler_instance_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, all_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(PUBLICATION_EVENTS, EVENT_PUBLICATION_TEMPC, data_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(PUBLICATION_EVENTS, EVENT_PUBLICATION_HUMIDITY, data_handler, NULL, NULL));
}

extern "C" void app_main(void)
{
    log_start();
    net_start();
    events_start();
    mqtt_start();
    ticker_init();

    xTaskCreate(dht_test, "dht_test", configMINIMAL_STACK_SIZE * 3, NULL, 5, NULL);

    std::cout << "<<<<< Sleepy loop timer >>>>" << std::endl;
    while(true) {
        std::this_thread::sleep_for(sleep_time);        
    }
}
