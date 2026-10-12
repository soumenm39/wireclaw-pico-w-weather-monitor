# System architecture

```text
Telegram / user
      |
      v
ESP32-S3 running WireClaw
      | UART1, 9600 baud
      | TX GPIO20 -> Pico GP1 (RX)
      | RX GPIO19 <- Pico GP0 (TX)
      v
Raspberry Pi Pico 2 W
      | I²C GP20 SDA / GP21 SCL
      +--> AHT20: temperature + relative humidity
      +--> HW-611 / BMP280: atmospheric pressure
      +--> onboard LED
      +--> GP22 digital output
```

The Pico firmware samples the sensors in its main loop and averages up to ten valid samples. The ESP32 requests a response with the `READ` command; the Pico returns a single JSON line over UART.
