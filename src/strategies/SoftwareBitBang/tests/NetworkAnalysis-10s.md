## NetworkAnalysis
This table was filled running the [NetworkAnalysis](../../../../examples/ARDUINO/Local/SoftwareBitBang/NetworkAnalysis) example for 10 seconds and averaging the obtained series of results.

| MODE | TX             | PIN | RX             | PIN | Bandwidth | Packets/s | Success rate |
|------|----------------|-----|----------------|-----|-----------|-----------|--------------|
| 1    | UNO/Nano       | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| 1    | UNO/Nano       | 11  | UNO/Nano       | 11  | 1710kB/s  | 57        | 100%         |
| 1    | UNO/Nano       | 10  | UNO/Nano       | 10  | 1710kB/s  | 57        | 100%         |
| 1    | UNO/Nano       | 12  | Leonardo/Micro | 12  | 1740kB/s  | 58        | 100%         |
| 1    | UNO/Nano       | 12  | Mega           | 12  | 1740kB/s  | 58        | 100%         |
| 1    | UNO/Nano       | 12  | ESP8266 160MHz | D1  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP8266 80MHz  | D1  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP32 40MHz    | 12  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP32 80MHz    | 12  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP32 40MHz    | 25  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | ESP32 80MHz    | 25  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | SAMD21         | D0  | 1770kB/s  | 59        | 100%         |
| 1    | UNO/Nano       | 12  | Blue pill      | PB9 | 1770kB/s  | 59        | 100%         |
| 1    | Leonardo/Micro | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| 1    | Leonardo/Micro | 12  | Leonardo/Micro | 12  |           |           |              |
| 1    | Leonardo/Micro | 12  | Mega           | 12  | 1740kB/s  | 58        | 100%         |
| 1    | Leonardo/Micro | 12  | ESP32          | 12  |           |           |              |
| 1    | Leonardo/Micro | 12  | SAMD21         | D0  |           |           |              |
| 1    | Mega           | 12  | UNO/Nano       | 12  | 1740kB/s  | 58        | 100%         |
| 1    | Mega           | 12  | Leonardo/Micro | 12  | 1740kB/s  | 58        | 100%         |
| 1    | Mega           | 12  | ESP8266 80MHz  | D1  |           |           |              |
| 1    | Mega           | 12  | ESP8266 160MHz | D1  |           |           |              |
| 1    | Mega           | 12  | ESP32 40MHz    | 12  | 1710kB/s  | 57        | 96.49%       |
| 1    | Mega           | 12  | ESP32 80MHz    | 12  | 1710kB/s  | 57        | 96.49%       |
| 1    | Mega           | 12  | ESP32 40MHz    | 25  | 1710kB/s  | 57        | 96.49%       |
| 1    | Mega           | 12  | ESP32 80MHz    | 25  | 1710kB/s  | 57        | 96.49%       |
| 1    | Mega           | 12  | SAMD21         | D0  | 1770kB/s  | 59        | 100%         |
| 1    | Mega           | 12  | SAMD21         | D1  |           |           |              |
| 1    | ESP8266 80MHz  | D1  | UNO/Nano       | 12  |           |           |              |
| 1    | ESP8266 160MHz | D1  | UNO/Nano       | 12  |           |           |              |
| 1    | ESP8266 80MHz  | D1  | Leonardo/Micro | 12  |           |           |              |
| 1    | ESP8266 160MHz | D1  | Leonardo/Micro | 12  |           |           |              |
| 1    | ESP8266 80MHz  | D1  | Mega           | 12  |           |           |              |
| 1    | ESP8266 160MHz | D1  | Mega           | 12  |           |           |              |
| 1    | ESP8266 80MHz  | D1  | Blue pill      | PB9 |           |           |              |
| 1    | ESP8266 160MHz | D1  | Blue pill      | PB9 |           |           |              |
| 1    | ESP32 80MHz    | 12  | UNO/Nano       | 12  |           |           |              |
| 1    | ESP32 40MHz    | 12  | UNO/Nano       | 12  |           |           |              |
| 1    | ESP32 40MHz    | 12  | Leonardo/Micro | 12  |           |           |              |
| 1    | ESP32 80MHz    | 12  | Leonardo/Micro | 12  |           |           |              |
| 1    | ESP32          | 12  | Blue pill      | PB9 | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | Mega           | 12  | 1770kB/s  | 59        | 100%         |
| 1    | SAMD21         | D0  | Leonardo/Micro | 12  | 1710kB/s  | 57        | 100%         |
| 1    | SAMD21         | D0  | UNO/Nano       | 12  | 1770kB/s  | 59        | 100%         |
| 1    | SAMD21         | D0  | ESP32 40MHz    | 12  | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | ESP32 80MHz    | 12  | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | ESP32 40MHz    | 25  | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | ESP32 80MHz    | 25  | 1800kB/s  | 60        | 100%         |
| 1    | SAMD21         | D0  | Blue pill      | PB9 | 1860kB/s  | 61        | 100%         |
| 1    | Blue pill      | PB9 | Blue pill      | PB9 | 1830kB/s  | 61        | 100%         |
| 1    | Blue pill      | PB9 | UNO/Nano       | 12  | 1800kB/s  | 60        | 100%         |
| 1    | Blue pill      | PB9 | SAMD           | 12  | 1830kB/s  | 61        | 100%         |
| 1    | Blue pill      | PB9 | ESP8266 80MHz  | D1  | 1788kB/s  | 60        | 99.33%       |
| 1    | Blue pill      | PB9 | ESP8266 160MHz | D1  | 1800kB/s  | 60        | 100%         |
| 2    | Blue pill      | PB9 | Blue pill      | PB9 | 2040kB/s  | 68        | 100%         |
| 2    | Blue pill      | PB9 | UNO/Nano       | 12  | 2010kB/s  | 67        | 100%         |
| 2    | Blue pill      | PB9 | SAMD           | 12  | 2040kB/s  | 68        | 100%         |
| 2    | UNO/Nano       | 12  | Blue pill      | PB9 | 1980kB/s  | 66        | 100%         |
| 2    | SAMD21         | D0  | Blue pill      | PB9 | 2010kB/s  | 67        | 100%         |
| 2    | ESP32          | 12  | Blue pill      | PB9 | 2070kB/s  | 69        | 100%         |
| 2    | Blue pill      | PB9 | ESP8266 80MHz  | D1  | 2019kB/s  | 68        | 99.77%       |
| 2    | Blue pill      | PB9 | ESP8266 160MHz | D1  | 2070kB/s  | 69        | 100%         |
| 2    | UNO/Nano       | 12  | ESP8266 160MHz | D1  | 1938kB/s  | 66        | 97.8%        |
| 2    | UNO/Nano       | 12  | ESP8266 80MHz  | D1  | 1969kB/s  | 66        | 98.93%       |
| 2    | ESP8266 80MHz  | D1  | UNO/Nano       | 12  | 1980kB/s  | 66        | 100%         |
| 2    | ESP8266 160MHz | D1  | UNO/Nano       | 12  | 2010kB/s  | 67        | 100%         |
