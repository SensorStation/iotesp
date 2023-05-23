#include <string>

#include <esp_event.h>
#include <esp_event.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_log.h>

#include "event.hh"
#include "utils.hh"
#include "mqtt_client.h"
#include "net.hh"

static const char* TAG = "MQTT";
esp_mqtt_client_handle_t mqtt_client;
bool mqtt_is_running = false;

// Todo get the IP address from the configuration
void mqtt_publish(std::string topic, int val) {
    std::string root = "ss/data/";
    // std::string ip = "10.11.44.21";
    std::string path = root + net->mac2str() + "/" + topic;

    char str[8];
    sprintf(str, "%3.2f", (double) val / 10);
    auto msg_id = esp_mqtt_client_publish(mqtt_client,
                                          path.c_str(),
                                          str, 0, 1, 0);
    ESP_LOGI(TAG, "periodic timer publish, msg_id=%d", msg_id);
}

// Handle incoming event data
static void mqtt_incoming_data(esp_mqtt_event_handle_t event)
{
    ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
    printf("DATA=%.*s\r\n", event->data_len, event->data);

    // post a publication event
    // ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_STOPPED, NULL, 0, portMAX_DELAY));
    // bool on = (bool) event->data;
    // ESP_ERROR_CHECK(esp_event_post(PUBLICATION_EVENTS,
    //                                EVENT_DEVICE_RELAY,
    //                                static_cast<void*>(&t),
    //                                sizeof(temperature),
    //                                portMAX_DELAY));
}

/*
 * @brief Event handler registered to receive MQTT events
 *
 *  This function is called by the MQTT client event loop.
 *
 * @param handler_args user data registered to the event.
 * @param base Event base for the handler(always MQTT Base in this example).
 * @param event_id The id for the received event.
 * @param event_data The data for the event, esp_mqtt_event_handle_t.
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id);

    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {

    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");

        // TODO make IP address configurable
        // TODO Publish charateristics of this sensor station
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        mqtt_is_running = true;

        // msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
        msg_id = esp_mqtt_client_subscribe(client, "ss/control/+/+", 0);
        ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
        ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_DATA:
        mqtt_incoming_data(event);
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_start()
{
    // Get the broker from configuration structure
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = "mqtt://10.11.1.11"; // should be: CONFIG_BROKER_URL;

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, (esp_mqtt_event_id_t) ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}
