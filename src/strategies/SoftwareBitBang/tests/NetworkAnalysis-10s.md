## NetworkAnalysis
This table was filled running the [NetworkAnalysis](../../../../examples/ARDUINO/Local/SoftwareBitBang/NetworkAnalysis) example for 10 seconds and averaging the obtained series of results.

| TX             | PIN | RX             | PIN | RESULT                                              |
|----------------|-----|----------------|-----|-----------------------------------------------------|
| UNO/Nano       | 12  | UNO/Nano       | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| UNO/Nano       | 12  | Leonardo/Micro | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| UNO/Nano       | 12  | Mega           | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| UNO/Nano       | 12  | ESP8266 160MHz | D1  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| UNO/Nano       | 12  | ESP8266 80MHz  | D1  | Bandwidth: 1620kB/s Packets/s: 57   Success: 98.28% |
| UNO/Nano       | 12  | ESP32          | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 99.00% |
| UNO/Nano       | PB9 | Bluepill       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| Leonardo/Micro | 12  | UNO/Nano       | 12  | Bandwidth: 1725kB/s Packets/s: 57.5 Success: 100%   |
| Leonardo/Micro | 12  | Leonardo/Micro | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| Leonardo/Micro | 12  | Mega           | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| Leonardo/Micro | 12  | ESP32          | 12  | Bandwidth: 1620kB/s Packets/s: 55   Success: 91.00% |
| Leonardo/Micro | 12  | SAMD21         | D0  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| Mega           | 12  | UNO/Nano       | 12  | Bandwidth: 1725kB/s Packets/s: 57.5 Success: 100%   |
| Mega           | 12  | Leonardo/Micro | 12  | Bandwidth: 1725kB/s Packets/s: 57.5 Success: 100%   |
| Mega           | 12  | ESP8266 80MHz  | D1  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| Mega           | 12  | ESP8266 160MHz | D1  | Bandwidth: 1755kB/s Packets/s: 58.5 Success: 99.5%  |
| Mega           | 12  | ESP32          | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 99.00% |
| Mega           | 12  | SAMD21         | D0  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP8266 80MHz  | D1  | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP8266 160MHz | D1  | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP8266 80MHz  | D1  | Mega           | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP8266 160MHz | D1  | Mega           | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP32          | 12  | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP32          | 12  | Leonardo/Micro | 12  | Bandwidth: 1620kB/s Packets/s: 55   Success: 91.00% |
| SAMD21         | D0  | Mega           | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| SAMD21         | D0  | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 99.00% |
| Blue pill      | PB9 | Blue pill      | PB9 | Bandwidth: 1860kB/s Packets/s: 62   Success: 100%   |
| Blue pill      | PB9 | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 99.5%  |
