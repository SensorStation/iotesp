#include <esp_log.h>

#include "event.hh"
#include "mqtt.hh"

/* Event source periodic timer related definitions */
ESP_EVENT_DEFINE_BASE(DATA_EVENTS);
ESP_EVENT_DEFINE_BASE(PUBLICATION_EVENTS);
ESP_EVENT_DEFINE_BASE(TIMER_EVENTS);

static const char *TAG = "StationEvents";

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
    event_value *val = static_cast<event_value*>(event_data);
    ESP_LOGI(TAG, "%s:%s - %d: data_handler", base, get_id_string(base, id), val->value);

    if (base != PUBLICATION_EVENTS) {
        ESP_LOGW(TAG, "unwanted event: %s:%s - %d: data_handler", base, get_id_string(base, id), val->value);
        return;
    }

    switch (id) {
    case EVENT_PUBLICATION_TEMPC:
        mqtt->publish("tempc", val->value);
        break;

    case EVENT_PUBLICATION_HUMIDITY:
        mqtt->publish("humidity", val->value);
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
