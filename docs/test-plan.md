# Test plan

## Automated host tests

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Coverage includes defaults, active-profile changes, invalid indices, malformed action counts, zero HID usages within an active action, version mismatch, and invalid persisted-configuration state.

## Target hardware tests

| Test | Procedure | Expected result |
|---|---|---|
| USB enumeration | Connect native USB to a PC | Device appears as a standard HID keyboard |
| Each macro key | Open a text editor and trigger each configured shortcut | Host receives the intended key combination |
| Debounce | Tap and slowly press/release a switch repeatedly | No duplicate action from one physical press |
| Profile cycle | Press profile button three times | Active profile advances through 0, 1, 2, then wraps |
| Persistence | Change profile, power cycle, reconnect | Saved profile is restored |
| Invalid NVS blob | Remove or corrupt stored keymap in a development test | Firmware falls back to defaults |
| USB disconnect | Unplug during idle and reconnect | Device recovers; no stuck host key |
| Endpoint busy | Trigger keys rapidly | No stuck keys; note any dropped macro events |

## Acceptance criteria

- Host tests pass in CI.
- Firmware builds with the documented ESP-IDF/TinyUSB versions.
- USB enumerates and every macro sends the expected key-down/key-up sequence.
- Profile selection persists over reboot.
- Wiring and board revision are recorded in the README or a test report.

Do not report hardware tests as passed until they have been performed on a physical board. Add board, OS, ESP-IDF version, TinyUSB component version, and test date to the test report.
