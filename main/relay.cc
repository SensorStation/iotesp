#include "relay.hh"

Relay::Relay(int p, std::string id)
{
    _pin = (gpio_num_t) p;
    _id = id;

    gpio_reset_pin(_pin);
    gpio_set_direction(_pin, GPIO_MODE_OUTPUT);
}

void Relay::on()
{
    _is_on = true;
    gpio_set_level( _pin, ON );
}

void Relay::off()
{
    _is_on = false;
    gpio_set_level( _pin, OFF );
}

bool Relay::is_on()
{
    _is_on = gpio_get_level(_pin);
    return _is_on;
}

std::string Relay::json()
{
    char j[256];
    sprintf(j, "\"%s\": %s", _id.c_str(), is_on() ? "true" : "false" );

    return _json = std::string(j);
}


Relays::Relays()
{
}

void Relays::add(int pin, std::string id)
{
    _relays[id] = new Relay(pin, id);
}

Relay* Relays::get(std::string id)
{
    return _relays[id];
}

std::string Relays::json()
{
    _json = std::string("{");
    bool first = true;
    for (auto const& [k, r] : _relays) {
        if (first) {
            first = false;
        } else {
            _json += ",";
        }
        _json += r->json();
    }
    _json += "}";
    return (_json);
}
