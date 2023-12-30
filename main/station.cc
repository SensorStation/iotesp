#include <esp_log.h>
#include <esp_timer.h>

#include "event.hh"
#include "mqtt.hh"
#include "net.hh"
#include "station.hh"

static const char *TAG = "STATION";
static void station_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data);

Station::Station() {
    // Initialize DHT with proper pin
    // Initialize relays with proper pin(s)
    // register station handler
    dht = new DHT((gpio_num_t)18);
    _id = net->mac2str();

    relays = new Relays();
    relays->add(5, "switch1");
}

void dht_read_data(void *arg)
{
    Station *st = (Station *) arg;
    if (st == NULL || st->dht == NULL) {
        ESP_LOGI(TAG, "DHT Not available for station");
        return;
    }
    st->dht->read_data();

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
    periodic_timer_args.callback = &dht_read_data;
    periodic_timer_args.name = "periodic";
    periodic_timer_args.arg = this;

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
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


static void station_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    // Station *station = static_cast<Station*>(event_data);
    Station *station = (Station *) handler_args;
    // ESP_LOGI(TAG, "%s:%s - %d: data_handler", base, get_id_string(base, id), val->i);
    if (base != DATA_EVENTS) {
        ESP_LOGW(TAG, "unwanted event: %s:%s: data_handler", base, get_id_string(base, id));
        return;
    }

    switch (id) {
    case EVENT_DATA_STATION:
        mqtt->publish("station", station->json());
        break;

    default:
        ESP_LOGW(TAG, "unknow event id: %s:%s:", base, get_id_string(base, id));
        return;
    }
}
