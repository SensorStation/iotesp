#pragma once

#include <string>

extern "C" {

#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_vendor.h"

lv_disp_t *oled_init(int pin_i2c_clk, int pin_i2c_data);
}

class OLED
{
private:
    int _i2c_clk;
    int _i2c_sda;

    lv_disp_t *_disp = NULL;
    lv_obj_t  *_scr_info = NULL;
    lv_obj_t  *_scr_temp = NULL;

public:
    OLED(int clk, int sda);

    void update_info(std::string text);
    void update_temp(std::string temp, std::string hum);
};
