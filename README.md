# WireClaw + Pico W Weather Monitor

An ESP32-S3 WireClaw agent connected over UART to a Raspberry Pi Pico-family weather-sensor node. This repository documents the integration and includes the Pico firmware supplied for this project.

> **Board-name note:** The uploaded firmware's own comments call the target **Raspberry Pi Pico 2 W**. The repository name is retained as requested (`wireclaw-pico-w-weather-monitor`), but verify your physical board before building. The included PlatformIO environment targets `rpipico2w`.

## Current scope

- **ESP32-S3 WireClaw** communicates with the Pico node using UART.
- **AHT20** measures temperature and relative humidity.
- **HW-611 / BMP280** measures atmospheric pressure.
- The Pico averages up to 10 sensor samples before storing the latest values.
- The Pico sends one JSON response when the ESP32 sends `READ`; it does not continuously transmit sensor JSON.
- Commands include `PING`, `STATUS`, `READ`, `LED ON`, `LED OFF`, `GPIO22 ON`, and `GPIO22 OFF`.

## Repository layout

```text
wireclaw-pico-w-weather-monitor/
├── README.md
├── CHANGELOG.md
├── LICENSE
├── .gitignore
├── docs/
│   ├── architecture.md
│   ├── wiring.md
│   ├── uart-protocol.md
│   └── testing-status.md
└── pico-w-weather-node/
    ├── platformio.ini
    └── src/
        └── main.cpp
```

## Hardware connections

### Pico 2 W sensor bus

| Signal | Pico GPIO |
|---|---:|
| I²C SDA | GP20 |
| I²C SCL | GP21 |
| UART0 TX | GP0 |
| UART0 RX | GP1 |
| Digital output | GP22 |
| Onboard LED | `LED_BUILTIN` |

### Cross-connect UART to ESP32-S3

| Pico node | ESP32-S3 WireClaw |
|---|---|
| GP0 / UART TX | GPIO19 / UART RX |
| GP1 / UART RX | GPIO20 / UART TX |
| GND | GND |

UART speed: **9600 baud**. Connect grounds together. Confirm the board pinout and logic-voltage compatibility before powering the circuit. Do not connect a 5 V UART signal directly to an ESP32 GPIO.

## Build the Pico firmware

1. Install Visual Studio Code and the PlatformIO IDE extension.
2. Open the `pico-w-weather-node` folder as the PlatformIO project.
3. Let PlatformIO install the configured platform and libraries.
4. Build the `pico2w` environment.
5. Connect the board and upload the firmware.

The configuration has been prepared from the supplied source and is **not represented as compile-tested**. If your actual board is the original Pico W rather than Pico 2 W, change the PlatformIO board target to the appropriate board supported by your installed platform.

## UART command protocol

| Command | Expected response / action |
|---|---|
| `PING` | `PONG` |
| `STATUS` | Sensor and output status text |
| `READ` | One JSON line with latest averaged values |
| `LED ON` | Turns the onboard LED on; returns `LED=ON` |
| `LED OFF` | Turns the onboard LED off; returns `LED=OFF` |
| `GPIO22 ON` | Sets GP22 HIGH; returns `GPIO22=ON` |
| `GPIO22 OFF` | Sets GP22 LOW; returns `GPIO22=OFF` |

Commands are newline-terminated. The firmware also writes diagnostic output to the Pico's USB serial port at **115200 baud**.

### Example JSON shape

```json
{
  "device": "Pico2W",
  "status": "OK",
  "aht_temp": 29.85,
  "humidity": 74.20,
  "pressure": 1006.68,
  "aht_status": "OK",
  "bmp_status": "OK",
  "sample_count": 10,
  "uptime_ms": 123456,
  "led": false,
  "gpio22": false
}
```

The numbers above illustrate the schema only; they are **not live measurements**.

## ESP32-S3 WireClaw side

The ESP32-side source is not included in this initial package because the uploaded file contains the Pico firmware only. Add the exact working WireClaw source under `esp32-wireclaw/` before treating this repository as a complete, reproducible firmware bundle. Keep secrets (Wi-Fi credentials, LLM API keys, Telegram bot tokens, NATS credentials) out of Git.

## Known limitations / validation checklist

- The source initializes the BMP280 at I²C address `0x76`, then tries `0x77`.
- The firmware checks sensor validity and reports `null` for unavailable values.
- GPIO22 is configured as an output and starts LOW. Check what is attached before issuing output commands.
- The code uses the Arduino `String` class for command and JSON assembly.
- The code and PlatformIO manifest still need a real build and hardware test in your environment.
- Confirm the exact board (Pico W vs Pico 2 W) and sensor module before release.

## License

This repository includes a placeholder MIT license file. Before publishing, verify that the upstream WireClaw and any bundled code's licenses permit redistribution, preserve required notices, and choose the appropriate license for your own additions.
