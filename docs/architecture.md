# Architecture

## Layers

1. **Portable keymap core** (`include/macro_keymap.h`, `src/macro_keymap.c`) contains the versioned profile/action model, defaults, validation, and helper functions. It has no ESP-IDF dependencies and can be host-tested.
2. **Platform application** (`main/main/app_main.c`) initializes NVS, configures GPIO, reads/debounces buttons, changes the active profile, and dispatches HID reports.
3. **USB HID transport** uses TinyUSB keyboard reports. The host sees a standard keyboard and does not require a custom driver.
4. **Persistent storage** uses an NVS blob in namespace `macro_pad`, key `keymap`. Loading validates the exact blob size and configuration version before use.

## Event flow

```text
GPIO level -> sampling/debounce -> stable press edge
    -> profile button: advance profile -> validate/persist -> LED pulse
    -> macro button: fetch active action -> HID key-down report -> key-up report
```

The GPIO loop is polled every 5 ms. The debounce interval is 30 ms. These values are starter settings, not measured response-time guarantees.

## Keymap model

The map supports three profiles and four macro buttons. Each action carries a modifier bitmask, a key count, and up to six HID usage IDs. A count of zero represents an empty action. Validation rejects unsupported configuration versions, out-of-range profile selection, key counts above the fixed array, and zero-valued entries in an active key list.

## Concurrency and failure handling

The first implementation is single-task application logic and does not use a separate input queue. USB reports may be skipped when TinyUSB is not ready. The profile is updated in RAM before NVS persistence; if persistence fails, the log reports the ESP error via `ESP_ERROR_CHECK` (which aborts). For a more fault-tolerant product, keep the previous profile on save failure and add queued/retried HID events.

## Why the core is separate

The model and validation rules can be tested on a developer workstation without compiling vendor SDK code. The same core can later be reused for a menu, serial configuration protocol, or simulator.
