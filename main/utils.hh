#pragma once

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE("SensorStation", "Last error %s: 0x%x", message, error_code);
    }
}


