# Wiring

## UART: Pico node ↔ ESP32-S3

| Pico 2 W | ESP32-S3 | Purpose |
|---|---|---|
| GP0 (UART TX) | GPIO19 (UART RX) | Pico → WireClaw |
| GP1 (UART RX) | GPIO20 (UART TX) | WireClaw → Pico |
| GND | GND | Common reference |

- UART: 9600 baud.
- USB serial diagnostics on Pico: 115200 baud.
- Cross TX/RX; do not connect TX to TX.
- Confirm the exact board pinout and voltage levels before wiring.

## I²C sensors on Pico

| Signal | GPIO |
|---|---:|
| SDA | GP20 |
| SCL | GP21 |

The firmware attempts BMP280 addresses `0x76` and `0x77`. AHT20 detection is handled by the Adafruit AHTX0 library.
