# Contributing

1. Keep the portable keymap module free of ESP-IDF dependencies.
2. Make changes on a feature branch and explain the motivation in the commit or pull request.
3. Run host tests before submitting:
   ```bash
   cmake -S . -B build
   cmake --build build
   ctest --test-dir build --output-on-failure
   ``
4. For firmware changes, include the ESP-IDF and `espressif/esp_tinyusb` versions used to build.
5. Never claim a hardware test passed unless it was run on a physical board. Include board revision, operating system, steps, and observed result.
6. Avoid committing build directories, credentials, generated binaries, or local SDK paths.
