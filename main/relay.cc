#include "relay.hh"

Relay::Relay(int p)
{
    _pin = (gpio_num_t) p;
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
    return _is_on;
}

Relays::Relays()
{
    for (int i = 0; i < RELAY_MAX; i++) {
        _relays[i] = NULL;
    }
}

int Relays::add(int pin)
{
    _relays[_relay_count] = new Relay(pin);
    _relay_count++;
    return _relay_count - 1;
}

Relay* Relays::get(int idx)
{
    if (idx > _relay_count) {
        return NULL;
    }
    return _relays[idx];
}
