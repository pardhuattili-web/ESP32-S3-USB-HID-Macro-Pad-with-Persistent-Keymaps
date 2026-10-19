#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "tinyusb.h"
#include "tusb.h"

#include "macro_keymap.h"

#define TAG "macro_pad"
#define NVS_NAMESPACE "macro_pad"
#define NVS_KEY_CONFIG "keymap"
#define BUTTON_DEBOUNCE_MS 30U
#define PROFILE_BUTTON_GPIO GPIO_NUM_4
#define BUTTON_0_GPIO GPIO_NUM_5
#define BUTTON_1_GPIO GPIO_NUM_6
#define BUTTON_2_GPIO GPIO_NUM_7
#define BUTTON_3_GPIO GPIO_NUM_8
#define STATUS_LED_GPIO GPIO_NUM_15

static const gpio_num_t button_gpios[MACRO_BUTTON_COUNT] = {
    BUTTON_0_GPIO, BUTTON_1_GPIO, BUTTON_2_GPIO, BUTTON_3_GPIO
};

typedef struct {
    bool stable_pressed;
    bool sampled_pressed;
    int64_t changed_at_us;
} button_state_t;

static button_state_t button_state[MACRO_BUTTON_COUNT];
static button_state_t profile_button_state;
static macro_keymap_t keymap;

static void hid_keyboard_report(uint8_t modifiers, const uint8_t *keys, uint8_t count)
{
    uint8_t report_keys[6] = {0U};
    uint8_t i;
    if (count > sizeof(report_keys)) {
        count = sizeof(report_keys);
    }
    for (i = 0U; i < count; ++i) {
        report_keys[i] = keys[i];
    }
    if (tud_hid_ready()) {
        (void)tud_hid_keyboard_report(0U, modifiers, report_keys);
    }
}

/* Press and release are sent separately to avoid stuck keys on rapid profile changes. */
static void dispatch_action(const macro_action_t *action)
{
    if (action == NULL || !tud_mounted()) {
        return;
    }
    hid_keyboard_report(action->modifiers, action->keys, action->key_count);
    vTaskDelay(pdMS_TO_TICKS(20U));
    hid_keyboard_report(0U, NULL, 0U);
}

static esp_err_t config_load(void)
{
    nvs_handle_t handle;
    size_t size = sizeof(keymap);
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_get_blob(handle, NVS_KEY_CONFIG, &keymap, &size);
    nvs_close(handle);
    if (err != ESP_OK || size != sizeof(keymap) ||
        macro_keymap_validate(&keymap) != MACRO_KEYMAP_OK) {
        return ESP_ERR_INVALID_STATE;
    }
    return ESP_OK;
}

static esp_err_t config_save(void)
{
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return err;
    }
    err = nvs_set_blob(handle, NVS_KEY_CONFIG, &keymap, sizeof(keymap));
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }
    nvs_close(handle);
    return err;
}

static void configure_buttons(void)
{
    uint64_t mask = (1ULL << PROFILE_BUTTON_GPIO);
    uint8_t i;
    for (i = 0U; i < MACRO_BUTTON_COUNT; ++i) {
        mask |= (1ULL << button_gpios[i]);
    }
    gpio_config_t inputs = {
        .pin_bit_mask = mask,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&inputs));
    gpio_config_t output = {
        .pin_bit_mask = (1ULL << STATUS_LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&output));
}

static bool debounce_read(gpio_num_t pin, button_state_t *state)
{
    const int64_t now = esp_timer_get_time();
    const bool pressed = gpio_get_level(pin) == 0;
    if (pressed != state->sampled_pressed) {
        state->sampled_pressed = pressed;
        state->changed_at_us = now;
    }
    if (pressed != state->stable_pressed &&
        (now - state->changed_at_us) >= ((int64_t)BUTTON_DEBOUNCE_MS * 1000LL)) {
        state->stable_pressed = pressed;
        return state->stable_pressed;
    }
    return false;
}

/* TinyUSB descriptor callbacks. The project uses a single HID keyboard interface. */
uint8_t const *tud_hid_descriptor_report_cb(uint8_t instance)
{
    (void)instance;
    static uint8_t const report_descriptor[] = {
        TUD_HID_REPORT_DESC_KEYBOARD()
    };
    return report_descriptor;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                              hid_report_type_t report_type, uint8_t *buffer,
                              uint16_t reqlen)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;
    return 0U;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize)
{
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}

static void usb_init(void)
{
    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .string_descriptor_count = 0,
        .external_phy = false
    };
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));
}

void app_main(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    macro_keymap_defaults(&keymap);
    if (config_load() != ESP_OK) {
        ESP_LOGW(TAG, "No valid saved keymap; loading defaults");
        ESP_ERROR_CHECK(config_save());
    }

    configure_buttons();
    usb_init();
    ESP_LOGI(TAG, "USB macro pad started; profile=%u", keymap.active_profile);

    while (true) {
        uint8_t i;
        if (debounce_read(PROFILE_BUTTON_GPIO, &profile_button_state)) {
            keymap.active_profile = (uint8_t)((keymap.active_profile + 1U) % MACRO_PROFILE_COUNT);
            ESP_ERROR_CHECK(config_save());
            ESP_LOGI(TAG, "Active profile changed to %u", keymap.active_profile);
            (void)gpio_set_level(STATUS_LED_GPIO, 1);
            vTaskDelay(pdMS_TO_TICKS(30U));
            (void)gpio_set_level(STATUS_LED_GPIO, 0);
        }
        for (i = 0U; i < MACRO_BUTTON_COUNT; ++i) {
            if (debounce_read(button_gpios[i], &button_state[i])) {
                ESP_LOGI(TAG, "Button %u pressed in profile %u", i, keymap.active_profile);
                dispatch_action(macro_keymap_get_action(&keymap, i));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(5U));
    }
}
