## NetworkAnalysis
This table was filled running the [NetworkAnalysis](../../../../examples/ARDUINO/Local/SoftwareBitBang/NetworkAnalysis) example for 10 seconds and averaging the obtained series of results.

| TX             | PIN | RX             | PIN | Bandwidth | Packets/s | Success rate |
|----------------|-----|----------------|-----|-----------|-----------|--------------|
| UNO/Nano       | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| UNO/Nano       | 12  | Leonardo/Micro | 12  | 1740kB/s  | 58        | 100%         |
| UNO/Nano       | 12  | Mega           | 12  | 1740kB/s  | 58        | 100%         |
| UNO/Nano       | 12  | ESP8266 160MHz | D1  | 1770kB/s  | 59        | 100%         |
| UNO/Nano       | 12  | ESP8266 80MHz  | D1  | 1620kB/s  | 57        | 98.28%       |
| UNO/Nano       | 12  | ESP32          | 12  | 1770kB/s  | 59        | 99.80%       |
| UNO/Nano       | PB9 | Blue pill      | 12  | 1770kB/s  | 59        | 100%         |
| Leonardo/Micro | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| Leonardo/Micro | 12  | Leonardo/Micro | 12  | 1740kB/s  | 58        | 100%         |
| Leonardo/Micro | 12  | Mega           | 12  | 1740kB/s  | 58        | 100%         |
| Leonardo/Micro | 12  | ESP32          | 12  | 1620kB/s  | 55        | 91.00%       |
| Leonardo/Micro | 12  | SAMD21         | D0  | 1770kB/s  | 59        | 100%         |
| Mega           | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| Mega           | 12  | Leonardo/Micro | 12  | 1725kB/s  | 57.5      | 100%         |
| Mega           | 12  | ESP8266 80MHz  | D1  | 1770kB/s  | 59        | 100%         |
| Mega           | 12  | ESP8266 160MHz | D1  | 1755kB/s  | 58.5      | 99.5%        |
| Mega           | 12  | ESP32          | 12  | 1770kB/s  | 59        | 99.00%       |
| Mega           | 12  | SAMD21         | D0  | 1770kB/s  | 59        | 100%         |
| ESP8266 80MHz  | D1  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| ESP8266 160MHz | D1  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| ESP8266 80MHz  | D1  | Mega           | 12  | 1770kB/s  | 59        | 100%         |
| ESP8266 160MHz | D1  | Mega           | 12  | 1770kB/s  | 59        | 100%         |
| ESP8266 80MHz  | D1  | Blue pill      | PB9 | 1764kB/s  | 58.8      | 98%          |
| ESP8266 160MHz | D1  | Blue pill      | PB9 | 1764kB/s  | 58.8      | 98%          |
| ESP32          | 12  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| ESP32          | 12  | Leonardo/Micro | 12  | 1620kB/s  | 55        | 91.00%       |
| ESP32          | 12  | Blue pill      | PB9 | 0kB/s     | 0         | 0%           |
| SAMD21         | D0  | Mega           | 12  | 1770kB/s  | 59        | 100%         |
| SAMD21         | D0  | Leonardo/Micro | 12  | 1590kB/s  | 53        | 88.68%       |
| SAMD21         | D0  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| SAMD21         | D0  | Blue pill      | PB9 | 1800kB/s  | 60        | 100%         |
| Blue pill      | PB9 | Blue pill      | PB9 | 1860kB/s  | 62        | 100%         |
| Blue pill      | PB9 | UNO/Nano       | 12  | 1770kB/s  | 59        | 99.5%        |
| Blue pill      | PB9 | SAMD           | 12  | 1830kB/s  | 61        | 100%         |
| Blue pill      | PB9 | ESP8266        | D1  | 0kB/s     |  0        | 0%           |
| Blue pill      | PB9 | ESP32          | 12  | 0kB/s     |  0        | 0%           |
