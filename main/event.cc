#include <esp_log.h>

#include "msg.hh"
#include "event.hh"
#include "mqtt.hh"
#include "station.hh"
#include "relay.hh"
#include "globals.hh"

/* Event source periodic timer related definitions */
ESP_EVENT_DEFINE_BASE(DATA_EVENTS);
ESP_EVENT_DEFINE_BASE(CONTROL_EVENTS);
ESP_EVENT_DEFINE_BASE(TIMER_EVENTS);

static const char *TAG = "StationEvents";

char* get_id_string(esp_event_base_t base, int32_t id) {
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

    } else if (base == DATA_EVENTS) {

        switch (id) {
        case EVENT_DATA_STATION:
            event = (char *) "EVENT_PUBLICATION_STATION";
            break;
        }

    } else if (base == CONTROL_EVENTS) {

        switch (id) {
        case EVENT_CONTROL_RELAY:
            event = (char *) "EVENT_CONTROL_RELAY";
            break;

        default:
            event = (char *) "EVENT_CONTROL_UNKNOWN";
            ESP_LOGW(TAG, "Control event: uknown type id: %s", get_id_string(base, id));
            break;
        }

    } else {

        event = (char *) "TASK_ITERATION_EVENT";
    }

    return event;
}

static void control_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    // event_value *val = static_cast<event_value*>(event_data);
    Msg *msg = static_cast<Msg*>(event_data);
    ESP_LOGI(TAG, "%s:%s msg %p", base, get_id_string(base, id), msg);

    if (base != CONTROL_EVENTS) {
        ESP_LOGW(TAG, "control_handler unwanted event: %s:%s", base, get_id_string(base, id));
        return;
    }

    if (station->relays == NULL) {
        ESP_LOGW(TAG, "control_handler relay called but no relays found");
        return;
    }

    Relay *relay = station->relays->get(msg->device);
    if (relay == NULL) {
        ESP_LOGW(TAG, "control_handler bad relay device %s", msg->device.c_str());
        return;
    }

    bool onoff = false;
    switch (id) {
    case EVENT_CONTROL_RELAY:

        // get the value and index of the relay and either turn it on or off
        onoff = msg->to_bool();
        onoff ? relay->on() : relay->off();
        break;

    default:
        ESP_LOGW(TAG, "control-handler event id: %s:%s", base, get_id_string(base, id));
        return;
    }
}

void events_init()
{
    // Create the default event loop
    ESP_ERROR_CHECK(esp_event_handler_register(CONTROL_EVENTS, EVENT_CONTROL_RELAY, control_handler, NULL));
}
