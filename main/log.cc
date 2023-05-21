#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "log.hh"

static const char *TAG = "SensorStation";

void log_init()
{
    // TODO Announce the Free memory and version via MQTT
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

