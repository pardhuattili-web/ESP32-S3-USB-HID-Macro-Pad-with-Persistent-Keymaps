#include "macro_keymap.h"

#include <stddef.h>
#include <string.h>

static bool action_valid(const macro_action_t *action)
{
    uint8_t i;
    if (action == NULL || action->key_count > MACRO_MAX_KEYS) {
        return false;
    }
    /* Key usage 0 is reserved for no key; reject it inside an active list. */
    for (i = 0U; i < action->key_count; ++i) {
        if (action->keys[i] == 0U) {
            return false;
        }
    }
    return true;
}

void macro_keymap_defaults(macro_keymap_t *map)
{
    if (map == NULL) {
        return;
    }
    (void)memset(map, 0, sizeof(*map));
    map->version = MACRO_CONFIG_VERSION;
    map->active_profile = 0U;

    /* Profile 0: coding */
    map->actions[0][0] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x1FU } }; /* S */
    map->actions[0][1] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x06U } }; /* C */
    map->actions[0][2] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x19U } }; /* V */
    map->actions[0][3] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x1AU } }; /* Z */

    /* Profile 1: editing / navigation */
    map->actions[1][0] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x2CU } }; /* Ctrl+Space */
    map->actions[1][1] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x2DU } }; /* Ctrl+- */
    map->actions[1][2] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x2EU } }; /* Ctrl+= */
    map->actions[1][3] = (macro_action_t){ .modifiers = MACRO_MOD_LALT, .key_count = 1U, .keys = { 0x4FU } }; /* Alt+Right */

    /* Profile 2: general productivity */
    map->actions[2][0] = (macro_action_t){ .modifiers = MACRO_MOD_LGUI, .key_count = 1U, .keys = { 0x15U } }; /* GUI+R */
    map->actions[2][1] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x17U } }; /* Ctrl+T */
    map->actions[2][2] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x16U } }; /* Ctrl+S */
    map->actions[2][3] = (macro_action_t){ .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x0BU } }; /* Ctrl+H */
}

macro_keymap_status_t macro_keymap_validate(const macro_keymap_t *map)
{
    uint8_t p;
    uint8_t b;

    if (map == NULL) {
        return MACRO_KEYMAP_INVALID_ARGUMENT;
    }
    if (map->version != MACRO_CONFIG_VERSION ||
        map->active_profile >= MACRO_PROFILE_COUNT) {
        return MACRO_KEYMAP_INVALID_PROFILE;
    }
    for (p = 0U; p < MACRO_PROFILE_COUNT; ++p) {
        for (b = 0U; b < MACRO_BUTTON_COUNT; ++b) {
            if (!action_valid(&map->actions[p][b])) {
                return MACRO_KEYMAP_INVALID_ACTION;
            }
        }
    }
    return MACRO_KEYMAP_OK;
}

macro_keymap_status_t macro_keymap_set_active_profile(macro_keymap_t *map, uint8_t profile)
{
    if (map == NULL) {
        return MACRO_KEYMAP_INVALID_ARGUMENT;
    }
    if (profile >= MACRO_PROFILE_COUNT) {
        return MACRO_KEYMAP_INVALID_PROFILE;
    }
    map->active_profile = profile;
    return MACRO_KEYMAP_OK;
}

macro_keymap_status_t macro_keymap_set_action(macro_keymap_t *map,
                                               uint8_t profile,
                                               uint8_t button,
                                               const macro_action_t *action)
{
    if (map == NULL || action == NULL) {
        return MACRO_KEYMAP_INVALID_ARGUMENT;
    }
    if (profile >= MACRO_PROFILE_COUNT) {
        return MACRO_KEYMAP_INVALID_PROFILE;
    }
    if (button >= MACRO_BUTTON_COUNT) {
        return MACRO_KEYMAP_INVALID_BUTTON;
    }
    if (!action_valid(action)) {
        return MACRO_KEYMAP_INVALID_ACTION;
    }
    map->actions[profile][button] = *action;
    return MACRO_KEYMAP_OK;
}

const macro_action_t *macro_keymap_get_action(const macro_keymap_t *map, uint8_t button)
{
    if (map == NULL || button >= MACRO_BUTTON_COUNT ||
        map->active_profile >= MACRO_PROFILE_COUNT) {
        return NULL;
    }
    return &map->actions[map->active_profile][button];
}
