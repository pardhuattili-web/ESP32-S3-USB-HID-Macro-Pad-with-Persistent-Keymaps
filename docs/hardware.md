# Hardware and wiring

## Recommended board

Use an ESP32-S3 development board with native USB device support, such as an ESP32-S3-DevKitC-1 variant. Confirm the board revision's schematic and pinout before wiring.

## Switch wiring

Each pushbutton connects its GPIO to GND when pressed. Firmware enables internal pull-ups and treats a low level as pressed.

| Function | GPIO | Other side |
|---|---:|---|
| Profile select | 4 | GND |
| Macro 0 | 5 | GND |
| Macro 1 | 6 | GND |
| Macro 2 | 7 | GND |
| Macro 3 | 8 | GND |
| Optional status LED | 15 | Proper LED/resistor circuit or compatible onboard LED |

Use momentary normally-open switches. Do not apply 5 V to GPIO. These pins are defaults, not universal pin assignments; avoid boot-strapping, flash/PSRAM, USB, or board-reserved pins based on the exact module design.

## USB

Connect the computer to the board's native USB device connector. Some boards have separate USB-UART and native USB connectors. The USB-UART port alone does not provide USB HID from the ESP32-S3.

## Bring-up checklist

- [ ] Verify board model and native USB routing.
- [ ] Test each switch input before enabling macros.
- [ ] Verify USB enumeration as a keyboard.
- [ ] Test press and release reports with a text editor.
- [ ] Confirm profile persists after reset and power removal.
- [ ] Confirm shortcuts on the target OS/layout before regular use.
