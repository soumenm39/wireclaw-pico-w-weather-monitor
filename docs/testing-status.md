# Testing status

This repository was assembled from the user-provided Pico firmware text. It has not been compiled or hardware-tested as part of packaging.

## Before release

- [ ] Confirm the physical board is Pico 2 W (the supplied source identifies it as Pico 2 W).
- [ ] Build with PlatformIO.
- [ ] Verify AHT20 is detected.
- [ ] Verify BMP280 is detected at `0x76` or `0x77`.
- [ ] Check Pico USB serial diagnostics at 115200 baud.
- [ ] Send `PING` and verify `PONG`.
- [ ] Send `STATUS` and verify sensor/output states.
- [ ] Send `READ` and parse the JSON response on ESP32-S3.
- [ ] Test LED control.
- [ ] Test GP22 only with the intended load and safe wiring.
- [ ] Confirm WireClaw's serial device settings match GPIO19 RX, GPIO20 TX, and 9600 baud.
