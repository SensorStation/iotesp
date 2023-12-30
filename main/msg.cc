#include "msg.hh"

static const char *TAG = "MSG";

Msg::Msg(char *topic, char *data)
{
    int i = 0;
    for (char *tok = strtok(topic, "/");
         tok != NULL && i < MAX_ARRAY_ELEMENTS;
         tok = strtok(NULL, "/"), i++) {

        switch (i) {
        case 1:
            source = string(tok);
            break;

        case 2:
            if (string(tok) == "control") {
                category = CONTROL;
            }
            break;

        case 3:
            if (string(tok) == "relay") {
                device = RELAY;
            }
            break;

        case 4:
            index = stoi(tok);
            break;
        }
    }

    if (!strncmp(data, "on", strlen(data))) {
        on = true;
    } else if (!strncmp(data, "off", strlen(data))) {
        on = false;
    } else {
        ESP_LOGW(TAG, "unknown relay state: %s", data);
    }
}

bool Msg::to_bool()
{
    return on;
}
