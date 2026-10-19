#ifndef MACRO_KEYMAP_H
#define MACRO_KEYMAP_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MACRO_PROFILE_COUNT 3U
#define MACRO_BUTTON_COUNT 4U
#define MACRO_MAX_KEYS 6U
#define MACRO_CONFIG_VERSION 1U

/* USB HID usage IDs, not ASCII characters. Modifier bits follow the HID spec. */
#define MACRO_MOD_LCTRL  (1U << 0)
#define MACRO_MOD_LSHIFT (1U << 1)
#define MACRO_MOD_LALT   (1U << 2)
#define MACRO_MOD_LGUI   (1U << 3)
#define MACRO_MOD_RCTRL  (1U << 4)
#define MACRO_MOD_RSHIFT (1U << 5)
#define MACRO_MOD_RALT   (1U << 6)
#define MACRO_MOD_RGUI   (1U << 7)

typedef struct {
    uint8_t modifiers;
    uint8_t key_count;
    uint8_t keys[MACRO_MAX_KEYS];
} macro_action_t;

typedef struct {
    uint16_t version;
    uint8_t active_profile;
    macro_action_t actions[MACRO_PROFILE_COUNT][MACRO_BUTTON_COUNT];
} macro_keymap_t;

typedef enum {
    MACRO_KEYMAP_OK = 0,
    MACRO_KEYMAP_INVALID_ARGUMENT,
    MACRO_KEYMAP_INVALID_PROFILE,
    MACRO_KEYMAP_INVALID_BUTTON,
    MACRO_KEYMAP_INVALID_ACTION
} macro_keymap_status_t;

void macro_keymap_defaults(macro_keymap_t *map);
macro_keymap_status_t macro_keymap_validate(const macro_keymap_t *map);
macro_keymap_status_t macro_keymap_set_active_profile(macro_keymap_t *map, uint8_t profile);
macro_keymap_status_t macro_keymap_set_action(macro_keymap_t *map,
                                               uint8_t profile,
                                               uint8_t button,
                                               const macro_action_t *action);
const macro_action_t *macro_keymap_get_action(const macro_keymap_t *map, uint8_t button);

#ifdef __cplusplus
}
#endif

#endif
