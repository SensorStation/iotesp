#include <esp_log.h>
#include <esp_timer.h>

#include "event.hh"
#include "mqtt.hh"
#include "net.hh"
#include "station.hh"

static const char *TAG = "STATION";
static void station_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data);

Station::Station() {

    dht = new DHT((gpio_num_t) 23);
    _id = net->mac2str();

    relays = new Relays();
    relays->add(2, "red");
    relays->add(4, "yellow");
    relays->add(5, "green");

    oled = new OLED(22, 21);
    oled->update_info(_id);

#ifdef NOTNOW
    soil = new Soil((gpio_num_t) 32);
    soil->read();
#endif // NOTNOW
}

static void periodic_reader(void *arg)
{
    Station *st = (Station *) arg;
    if (st == NULL || st->dht == NULL) {
        ESP_LOGI(TAG, "DHT Not available for station");
        return;
    }
    st->dht->read_data();
    // st->soil->read();

    ESP_LOGI(TAG, "ESP Read DHT temperature and humidity");
    ESP_ERROR_CHECK(esp_event_post(DATA_EVENTS,
                                   EVENT_DATA_STATION,
                                   static_cast<void*>(&st),
                                   sizeof(Station),
                                   portMAX_DELAY));
}

void Station::start_reading()
{
    esp_timer_create_args_t periodic_timer_args = {};
    periodic_timer_args.callback = &periodic_reader;
    periodic_timer_args.name = "periodic";
    periodic_timer_args.arg = this;

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    printf("periodic_timer running\n");
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, 5000000)); 
    ESP_ERROR_CHECK(esp_event_handler_register(DATA_EVENTS, EVENT_DATA_STATION, station_handler, this));
}

std::string Station::json()
{
    char j[256];
    sprintf(j, "{ \"id\": \"%s\", \"sensors\": ", _id.c_str());
    _json = std::string(j);
    _json += dht->json() + ", ";
    _json += "\"relays\": " + relays->json();
    _json += "}";
    return _json;
}

void Station::update_display()
{
    char t[80], h[80];
    snprintf(t, 80, "%3.2f", dht->get_tempf());
    snprintf(h, 80, "%3.2f%%", dht->get_humidity());

    oled->update_temp(t, h);
}

static void station_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    Station *station = (Station *) handler_args;
    if (base != DATA_EVENTS) {
        ESP_LOGW(TAG, "unwanted event: %s:%s: data_handler", base, get_id_string(base, id));
        return;
    }

    switch (id) {
    case EVENT_DATA_STATION:
        mqtt->publish("station", station->json());
        station->update_display();
        break;

    default:
        ESP_LOGW(TAG, "unknow event id: %s:%s:", base, get_id_string(base, id));
        return;
    }
}
