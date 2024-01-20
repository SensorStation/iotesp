idf_component_register(SRCS
                       oled.cc
                       oled_init.c
                       lvgl_demo_ui.c
                       INCLUDE_DIRS include)

add_compile_options(-Wdeprecated-enum-enum-conversion)
