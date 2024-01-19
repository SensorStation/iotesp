#pragma once

extern "C" void oled_init(void);

class OLED
{
public:
    OLED() {
        oled_init();
    }
};
