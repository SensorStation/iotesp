#include "oled.hh"

#ifdef __cplusplus
extern "C" {
#endif

// #include "esp_lcd_panel_io.h"
// #include "esp_lcd_panel_ops.h"

#include "lvgl.h"
// #include "esp_lvgl_port.h"
// #include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
}
#endif

OLED::OLED(int clk, int sda)
{
    _i2c_clk = clk;
    _i2c_sda = sda;
    _disp = oled_init(_i2c_clk, _i2c_sda);

    /* Set rotation of the screen */
    lv_disp_set_rotation(_disp, LV_DISP_ROT_NONE);
    if (lvgl_port_lock(0)) {
        _scr_info = lv_disp_get_scr_act(_disp);
        _scr_temp = lv_obj_create(_scr_info);

        lvgl_port_unlock();     // Release the mutex
    }
}

void OLED::update_info(std::string text)
{
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(0)) {
        lv_scr_load(_scr_info);
        lv_obj_t *label = lv_label_create(_scr_info);
        lv_label_set_text(label, text.c_str());
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
        lv_obj_set_width(label, _disp->driver->hor_res);
        lvgl_port_unlock();     // Release the mutex
    }
}

void OLED::update_temp(std::string tempf, std::string hum)
{
    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(0)) {
        lv_scr_load(_scr_temp);
        
        lv_obj_t *temp_label = lv_label_create(_scr_temp);
        lv_label_set_text(temp_label, tempf.c_str());
        lv_obj_align(temp_label, LV_ALIGN_TOP_MID, 0, 2);

        lv_obj_t *hum_label = lv_label_create(_scr_temp);
        lv_label_set_text(hum_label, hum.c_str());
        lv_obj_set_height(hum_label, lv_pct(70));
        lv_obj_align(hum_label, 0, 20, 20);
        lvgl_port_unlock();     // Release the mutex
    }

}
