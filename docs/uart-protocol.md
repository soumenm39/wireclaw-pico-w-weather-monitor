# UART protocol

Commands are ASCII strings terminated by newline. The Pico accepts case-insensitive command names.

| Request | Reply |
|---|---|
| `PING` | `PONG` |
| `STATUS` | `AHT=OK/ERROR,BMP=OK/ERROR,LED=ON/OFF,GPIO22=ON/OFF` |
| `READ` | One JSON line with latest averaged values |
| `LED ON` | `LED=ON` |
| `LED OFF` | `LED=OFF` |
| `GPIO22 ON` | `GPIO22=ON` |
| `GPIO22 OFF` | `GPIO22=OFF` |
| Unknown command | `ERROR=UNKNOWN_COMMAND` |

The firmware discards an input command if it grows beyond 100 characters. Sensor JSON is sent only in response to `READ`; it is not sent automatically every sampling cycle.
