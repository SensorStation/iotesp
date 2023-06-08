#include "uart.hh"

UART::UART()
{
    _uart_config = {
        .baud_rate      = 9600,
        .data_bits      = 8,
        .parity         = 0,
        .stop_bits      = 1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
}

int baud(int b)
{
    if (b != 0) {
        _uart_config.baud = b;        
    }
    return _uart_config.baud;
}

int UART::start()
{
    int ret = uart_driver_install(_pin, CONFIG_NMEA_PARSER_RING_BUFFER_SIZE, 0,
                                  config->uart.event_queue_size, &esp_gps->event_queue, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(GPS_TAG, "install uart driver failed");
        goto err_uart_install;
    }
    
    if (uart_param_config(esp_gps->uart_port, &uart_config) != ESP_OK) {
        ESP_LOGE(GPS_TAG, "config uart parameter failed");
        goto err_uart_config;
    }

}
