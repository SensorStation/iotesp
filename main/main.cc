/*
*/

#include <thread>
#include <chrono>

// #include "gtu7.hh"

#include "event.hh"
#include "log.hh"
#include "mqtt.hh"
#include "net.hh"
#include "ticker.hh"

#include "station.hh"
#include "relay.hh"

using namespace std::chrono;

Net     *net = NULL;
MQTT    *mqtt = NULL;
Station *station = NULL;

extern "C" void app_main(void)
{
    log_init();
    net = new Net();

    events_init();
    mqtt = new MQTT("10.11.1.11"); // use config broker

    station = new Station();
    station->start_reading();

    const auto sleep_time   = seconds { 5 };
    while(true) {
        std::this_thread::sleep_for(sleep_time);        
    }
}
