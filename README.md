# ESP32-S3 USB HID Macro Pad with Persistent Keymaps

[![Host tests](https://github.com/pardhuattili-web/ESP32-S3-USB-HID-Macro-Pad-with-Persistent-Keymaps/actions/workflows/ci.yml/badge.svg)](https://github.com/pardhuattili-web/ESP32-S3-USB-HID-Macro-Pad-with-Persistent-Keymaps/actions/workflows/ci.yml)

A programmable four-key macro pad based on the ESP32-S3. It enumerates as a standard USB HID keyboard, includes three selectable profiles, debounces mechanical switches, and saves the active profile and keymap in ESP-IDF NVS.

> **Status:** Portable keymap logic and host-side unit tests are included. The ESP-IDF/TinyUSB application scaffold is provided, but a physical ESP32-S3 build and USB-host test must be performed before claiming hardware validation.

## Features

- USB HID keyboard output through native USB on ESP32-S3.
- Four configurable action buttons plus a dedicated profile-switch button.
- Three built-in profiles: coding, editing/navigation, and general productivity.
- Software debouncing and explicit press/release HID report handling.
- Versioned keymap stored as an NVS blob; invalid or missing configuration falls back to defaults.
- Portable keymap module with host-side unit tests.
- Host build with warnings-as-errors and GitHub Actions CI.

## Hardware

Recommended: ESP32-S3-DevKitC-1 or another ESP32-S3 board with native USB device support.

| Signal | Default GPIO | Wiring |
|---|---:|---|
| Profile switch | GPIO4 | Switch between GPIO4 and GND |
| Macro button 0 | GPIO5 | Switch between GPIO5 and GND |
| Macro button 1 | GPIO6 | Switch between GPIO6 and GND |
| Macro button 2 | GPIO7 | Switch between GPIO7 and GND |
| Macro button 3 | GPIO8 | Switch between GPIO8 and GND |
| Status LED | GPIO15 | Compatible onboard LED or an external LED with a suitable resistor |

Inputs use internal pull-ups and are active-low. **Check the exact development-board pinout** before wiring; module variants differ, and not every GPIO is available. Do not apply 5 V to GPIO pins. Use the native USB device connector wired to the ESP32-S3 USB D+/D− lines, not a USB-UART-only connector.

## Repository layout

```text
include/macro_keymap.h       Portable keymap API and data model
src/macro_keymap.c           Defaults, validation, profile/action helpers
tests/test_macro_keymap.c    Host unit tests
CMakeLists.txt               Host-only CMake project
main/CMakeLists.txt          ESP-IDF project
main/idf_component.yml       TinyUSB component dependency
main/main/app_main.c         GPIO, NVS, and USB HID application
docs/                         Architecture, wiring, configuration, test plan
.github/workflows/ci.yml      Automated host build and tests
```

## Build and test the portable core

Requires CMake 3.16+ and a C11 compiler (GCC or Clang).

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

These tests validate default profiles, profile switching, action bounds, malformed configurations, and invalid key usages. They do not compile the ESP-IDF application.

## Build the ESP-IDF firmware

1. Install ESP-IDF 5.1 or later and export its environment.
2. From the repository root, run:

```bash
idf.py set-target esp32s3
idf.py reconfigure
idf.py build
idf.py -p PORT flash monitor
```

The Component Manager dependency is declared in `main/idf_component.yml`. Review the exact ESP-IDF and `espressif/esp_tinyusb` versions installed. TinyUSB configuration and descriptor APIs vary between releases; check the app glue against the installed component if compilation reports API mismatches.

## How to use

1. Connect the board's native USB device port to a PC.
2. Wait for the device to enumerate as a standard HID keyboard.
3. Press one of the four macro buttons to send the mapped shortcut.
4. Press the profile button to cycle through profiles 0, 1, and 2. The selected profile is saved in NVS.
5. Edit the default action arrays in `src/macro_keymap.c` to customize the shortcuts, rebuild, and flash.

Mappings use USB HID usage IDs, not ASCII strings. Results depend on the host operating system and keyboard layout. The current implementation uses compiled-in shortcut defaults; it does not yet include a host-side configuration utility.

## Engineering notes and limitations

- NVS currently stores the native C structure as a blob. This is convenient for a demonstration but is not a stable cross-compiler format. The version field enables basic rejection; future work should serialize fields explicitly and implement migration.
- The application uses a small polling/debounce loop and does not yet queue HID events. Reports can be skipped when TinyUSB is not ready. A production design should add a bounded event queue, retry policy, and explicit disconnect recovery.
- Confirm the TinyUSB descriptor setup against the installed ESP-IDF component version.
- GPIO assignments are examples; verify board-specific restrictions before wiring.
- No hardware timing figures or physical USB test results are claimed until measured.

## Documentation

- [Architecture](docs/architecture.md)
- [Hardware and wiring](docs/hardware.md)
- [Configuration model](docs/configuration.md)
- [Test plan](docs/test-plan.md)

## License

MIT — see [LICENSE](LICENSE).
