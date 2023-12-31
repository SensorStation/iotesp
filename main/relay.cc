#include "relay.hh"

Relay::Relay(int p, std::string id)
{
    _pin = (gpio_num_t) p;
    _id = id;
    _state = false;

    gpio_reset_pin(_pin);
    gpio_set_direction(_pin, GPIO_MODE_OUTPUT);

    gpio_num_t pin = (gpio_num_t)(_pin & 0x1F);

    // XXX - this is supposed to read the value of an output pin
    // so we know what state we are outputing.
    if (GPIO_REG_READ(GPIO_ENABLE_REG) & BIT(pin)) {
	_state = (GPIO_REG_READ(GPIO_OUT_REG)  >> _pin) & 1U;
    }
    
}

void Relay::on()
{
    _state = true;
    gpio_set_level( _pin, ON );
}

void Relay::off()
{
    _state = false;
    gpio_set_level( _pin, OFF );
}

bool Relay::state()
{
    return _state;
}

std::string Relay::json()
{
    char j[256];
    sprintf(j, "\"%s\": %s", _id.c_str(), state() ? "true" : "false" );

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
