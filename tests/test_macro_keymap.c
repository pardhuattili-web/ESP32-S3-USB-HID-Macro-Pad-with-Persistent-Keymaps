#include "macro_keymap.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

static void test_defaults_are_valid(void)
{
    macro_keymap_t map;
    macro_keymap_defaults(&map);
    assert(macro_keymap_validate(&map) == MACRO_KEYMAP_OK);
    assert(map.version == MACRO_CONFIG_VERSION);
    assert(map.active_profile == 0U);
    assert(macro_keymap_get_action(&map, 0U) != NULL);
    assert(macro_keymap_get_action(&map, MACRO_BUTTON_COUNT) == NULL);
}

static void test_profile_switching(void)
{
    macro_keymap_t map;
    macro_keymap_defaults(&map);
    assert(macro_keymap_set_active_profile(&map, 2U) == MACRO_KEYMAP_OK);
    assert(map.active_profile == 2U);
    assert(macro_keymap_set_active_profile(&map, 3U) == MACRO_KEYMAP_INVALID_PROFILE);
    assert(map.active_profile == 2U);
}

static void test_action_validation(void)
{
    macro_keymap_t map;
    macro_action_t action = { .modifiers = MACRO_MOD_LCTRL, .key_count = 1U, .keys = { 0x06U } };
    macro_keymap_defaults(&map);
    assert(macro_keymap_set_action(&map, 1U, 3U, &action) == MACRO_KEYMAP_OK);
    assert(macro_keymap_get_action(&map, 3U)->keys[0] == 0x06U);
    assert(macro_keymap_set_action(&map, 3U, 0U, &action) == MACRO_KEYMAP_INVALID_PROFILE);
    assert(macro_keymap_set_action(&map, 0U, 4U, &action) == MACRO_KEYMAP_INVALID_BUTTON);

    action.key_count = MACRO_MAX_KEYS + 1U;
    assert(macro_keymap_set_action(&map, 0U, 0U, &action) == MACRO_KEYMAP_INVALID_ACTION);
    action.key_count = 1U;
    action.keys[0] = 0U;
    assert(macro_keymap_set_action(&map, 0U, 0U, &action) == MACRO_KEYMAP_INVALID_ACTION);
}

static void test_corrupt_config_rejected(void)
{
    macro_keymap_t map;
    macro_keymap_defaults(&map);
    map.version++;
    assert(macro_keymap_validate(&map) == MACRO_KEYMAP_INVALID_PROFILE);
    macro_keymap_defaults(&map);
    map.actions[1][1].key_count = 255U;
    assert(macro_keymap_validate(&map) == MACRO_KEYMAP_INVALID_ACTION);
}

int main(void)
{
    test_defaults_are_valid();
    test_profile_switching();
    test_action_validation();
    test_corrupt_config_rejected();
    puts("All macro keymap tests passed.");
    return 0;
}
