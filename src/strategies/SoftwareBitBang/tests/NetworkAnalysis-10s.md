## NetworkAnalysis
This table was filled running the [NetworkAnalysis](../../../../examples/ARDUINO/Local/SoftwareBitBang/NetworkAnalysis) example for 10 seconds and averaging the obtained series of results.

| MODE | TX             | PIN | RX             | PIN | Bandwidth | Packets/s | Success rate |
|------|----------------|-----|----------------|-----|-----------|-----------|--------------|
| 1    | UNO/Nano       | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| 1    | UNO/Nano       | 12  | Leonardo/Micro | 12  | 1740kB/s  | 58        | 100%         |
| 1    | UNO/Nano       | 12  | Mega           | 12  | 1740kB/s  | 58        | 100%         |
| 1    | UNO/Nano       | 12  | ESP8266 160MHz | D1  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP8266 80MHz  | D1  | 1740kB/s  | 58        | 98.28%       |
| 1    | UNO/Nano       | 12  | ESP32 40MHz    | 12  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP32 80MHz    | 12  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP32 40MHz    | 25  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP32 80MHz    | 25  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | PB9 | Blue pill      | 12  |           |           |              |
| 1    | Leonardo/Micro | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| 1    | Leonardo/Micro | 12  | Leonardo/Micro | 12  | 1740kB/s  | 58        | 100%         |
| 1    | Leonardo/Micro | 12  | Mega           | 12  | 1740kB/s  | 58        | 100%         |
| 1    | Leonardo/Micro | 12  | ESP32          | 12  | 1560kB/s  | 52        | 86.54%       |
| 1    | Leonardo/Micro | 12  | SAMD21         | D0  | 1770kB/s  | 59        | 100%         |
| 1    | Mega           | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| 1    | Mega           | 12  | Leonardo/Micro | 12  | 1725kB/s  | 58        | 100%         |
| 1    | Mega           | 12  | ESP8266 80MHz  | D1  | 1770kB/s  | 59        | 100%         |
| 1    | Mega           | 12  | ESP8266 160MHz | D1  | 1770kB/s  | 59        | 100%         |
| 1    | Mega           | 12  | ESP32 40MHz    | 12  | 1755kB/s  | 58.5      | 99.5%        |
| 1    | Mega           | 12  | ESP32 80MHz    | 12  | 1755kB/s  | 58.5      | 99.5%        |
| 1    | Mega           | 12  | ESP32 40Hz     | 25  | 1755kB/s  | 58.5      | 99.5%        |
| 1    | Mega           | 12  | ESP32 80Hz     | 25  | 1755kB/s  | 58.5      | 99.5%        |
| 1    | Mega           | 12  | SAMD21         | D0  | 1770kB/s  | 59        | 100%         |
| 1    | Mega           | 12  | SAMD21         | D1  | 1770kB/s  | 59        | 100%         |
| 1    | ESP8266 80MHz  | D1  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| 1    | ESP8266 160MHz | D1  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| 1    | ESP8266 80MHz  | D1  | Leonardo/Micro | 12  | 1770kB/s  | 59        | 100%         |
| 1    | ESP8266 160MHz | D1  | Leonardo/Micro | 12  | 1770kB/s  | 59        | 100%         |
| 1    | ESP8266 80MHz  | D1  | Mega           | 12  | 1770kB/s  | 59        | 100%         |
| 1    | ESP8266 160MHz | D1  | Mega           | 12  | 1770kB/s  | 59        | 100%         |
| 1    | ESP8266 80MHz  | D1  | Blue pill      | PB9 |           |           |              |
| 1    | ESP8266 160MHz | D1  | Blue pill      | PB9 |           |           |              |
| 1    | ESP32 80MHz    | 12  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| 1    | ESP32 40MHz    | 12  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| 1    | ESP32 40MHz    | 12  | Leonardo/Micro | 12  | 1777kB/s  | 59        | 100%         |
| 1    | ESP32 80MHz    | 12  | Leonardo/Micro | 12  | 1777kB/s  | 59        | 100%         |
| 1    | ESP32          | 12  | Blue pill      | PB9 |           |           |              |
| 1    | SAMD21         | D0  | Mega           | 12  | 1770kB/s  | 58        | 98.28%       |
| 1    | SAMD21         | D0  | Leonardo/Micro | 12  | 1770kB/s  | 59        | 100%         |
| 1    | SAMD21         | D0  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| 1    | SAMD21         | D0  | ESP32 40MHz    | 12  | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | ESP32 80MHz    | 12  | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | ESP32 40MHz    | 25  | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | ESP32 80MHz    | 25  | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | Blue pill      | PB9 |           |           |              |
| 1    | Blue pill      | PB9 | Blue pill      | PB9 |           |           |              |
| 1    | Blue pill      | PB9 | UNO/Nano       | 12  |           |           |              |
| 1    | Blue pill      | PB9 | SAMD           | 12  |           |           |              |
| 1    | Blue pill      | PB9 | ESP8266        | D1  |           |           |              |
| 1    | Blue pill      | PB9 | ESP32          | 12  |           |           |              |
