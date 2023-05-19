#include "sensor.hh"

#include "dht.hh"
#include "event.hh"
#include "mqtt.hh"

const gpio_num_t RELAY_PIN     = (gpio_num_t) 5;
static const dht_sensor_type_t sensor_type = DHT_TYPE_AM2301;
static const gpio_num_t dht_gpio = (gpio_num_t)18;

void sensor_reader_ticker(void *arg)
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
