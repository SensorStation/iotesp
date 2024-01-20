#include "oled.hh"

#ifdef __cplusplus
extern "C" {
#endif

// #include "esp_lcd_panel_io.h"
// #include "esp_lcd_panel_ops.h"

// #include "lvgl.h"
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
}

void OLED::rotation(std::string text)
{
    lv_obj_t *scr = lv_disp_get_scr_act(_disp);
    lv_obj_t *label = lv_label_create(scr);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
    lv_label_set_text(label, text.c_str());
    /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
    lv_obj_set_width(label, _disp->driver->hor_res);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
}

void OLED::display(std::string text)
{
    /* Rotation of the screen */
    lv_disp_set_rotation(_disp, LV_DISP_ROT_NONE);

    // Lock the mutex due to the LVGL APIs are not thread-safe
    if (lvgl_port_lock(0)) {
        rotation(text);
        lvgl_port_unlock();     // Release the mutex
    }

}
