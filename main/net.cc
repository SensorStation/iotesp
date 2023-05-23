#include <sstream>

#include <esp_netif.h>
#include <esp_wifi.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include "protocol_examples_common.h"

#include "event.hh"
#include "net.hh"

const char *TAG = "Net";

Net::Net()
{
    _mac[0] = 0x0;

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

    // TODO: Get IP Address and set to be used in mqtt messages
}

uint8_t *Net::get_mac()
{
    if (_mac[0] == 0x0) {
        esp_wifi_get_mac(WIFI_IF_STA, _mac);
    }
    return _mac;
}

std::string Net::mac2str()
{
    if (macstr.length() < 1) {
        char str[18];
        snprintf(str, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
                 _mac[0], _mac[1], _mac[2], _mac[3], _mac[4], _mac[5]);
        macstr = str;
    }    
    
    return macstr;
}
