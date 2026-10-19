idf_component_register(
    SRCS "app_main.c" "../../src/macro_keymap.c"
    INCLUDE_DIRS "../../include"
    REQUIRES driver nvs_flash esp_tinyusb
)
