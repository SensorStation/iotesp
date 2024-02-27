#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/soc_caps.h"
#include "esp_log.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

#include "adc.h"
#include "soil.hh"

static const char *TAG = "Soil";

#define ADC_CHANNEL     ADC_CHANNEL_4
#define ADC_ATTEN       ADC_ATTEN_DB_12

Soil::Soil(gpio_num_t p)
{
    _pin = p;
    adc_init();
}

int Soil::read()
{
    static int adc_raw[2][10];
    static int voltage[2][10];

    while(1) {
        printf("Soil Read\n");
        ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, ADC_CHANNEL, &adc_raw[0][0]));
        ESP_LOGI(TAG, "ADC%d Channel[%d] Raw Data: %d", ADC_UNIT_1 + 1, ADC_CHANNEL, adc_raw[0][0]);
        printf("ADC%d Channel[%d] Raw Data: %d\n", ADC_UNIT_1 + 1, ADC_CHANNEL, adc_raw[0][0]);
        if (do_calibration) {
            ESP_ERROR_CHECK(adc_cali_raw_to_voltage(adc_cali_handle, adc_raw[0][0], &voltage[0][0]));
            ESP_LOGI(TAG, "ADC%d Channel[%d] Cali Voltage: %d mV", ADC_UNIT_1 + 1, ADC_CHANNEL, voltage[0][0]);
            printf("ADC%d Channel[%d] Cali Voltage: %d mV\n", ADC_UNIT_1 + 1, ADC_CHANNEL, voltage[0][0]);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("<-- Soil Read -->\n");
    }
    return 0;
}
