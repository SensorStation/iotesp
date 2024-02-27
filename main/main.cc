/*
*/

#include <thread>
#include <chrono>

#include "oled.hh"

#include "event.hh"
#include "logger.hh"
#include "mqtt.hh"
#include "net.hh"

#include "station.hh"
#include "relay.hh"

using namespace std::chrono;

Net     *net = NULL;
MQTT    *mqtt = NULL;
Station *station = NULL;
Logger  *logger = NULL;
Events  *events = NULL;

extern "C" void app_main(void)
{
    // log = new Logger();
    // log->init();
    logger = new Logger();
    logger->init();

    events = new Events();
    events->start();

    net = new Net();
    mqtt = new MQTT("10.11.1.11"); // use config broker

    station = new Station();
    station->start_reading();

    printf("Past Events\n");
    const auto sleep_time   = seconds { 5 };
    while(true) {
        std::this_thread::sleep_for(sleep_time);        
    }
}
