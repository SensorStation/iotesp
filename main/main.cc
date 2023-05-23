/*
*/

#include <thread>
#include <chrono>

#include "event.hh"
#include "log.hh"
#include "mqtt.hh"
#include "net.hh"
#include "ticker.hh"

#include "relay.hh"

using namespace std::chrono;

Net *net = NULL;

extern "C" void app_main(void)
{
    log_init();
    net = new Net();

    events_start();

    mqtt_start();
    ticker_init();

    const auto sleep_time   = seconds { 5 };
    while(true) {
        std::this_thread::sleep_for(sleep_time);        
    }
}
