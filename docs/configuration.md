# Keymap configuration

## In-memory structure

The portable API defines:

- `MACRO_PROFILE_COUNT = 3`
- `MACRO_BUTTON_COUNT = 4`
- `MACRO_MAX_KEYS = 6`
- Configuration version `1`

Each action contains an eight-bit modifier mask, a key count, and an array of up to six USB HID keyboard usage IDs. Modifier bits use the standard HID keyboard modifier bit positions.

## Editing defaults

Change the default action assignments in `src/macro_keymap.c`, then rebuild and flash. HID usage IDs are defined by the USB HID Usage Tables, not ASCII. For instance, the usage for a physical key may produce a different character with another keyboard layout.

## Persistent representation

The ESP-IDF application stores the native `macro_keymap_t` structure using NVS `nvs_set_blob`. It validates the blob size, version, active profile, and action content after load. Invalid or missing settings fall back to defaults and are saved.

This format is suitable for a compact demonstration, not long-term compatibility across architectures or compiler changes. For a robust release, serialize fields explicitly into a stable byte format, define endianness, add a checksum, and implement versioned migration.
