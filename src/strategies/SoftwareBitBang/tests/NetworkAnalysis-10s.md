## NetworkAnalysis
This table was filled running the [NetworkAnalysis](../../../../examples/ARDUINO/Local/SoftwareBitBang/NetworkAnalysis) example for 10 seconds and averaging the obtained series of results.

| TX             | PIN | RX             | PIN | RESULT                                              |
|----------------|-----|----------------|-----|-----------------------------------------------------|
| UNO/Nano       | 12  | UNO/Nano       | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| Mega           | 12  | UNO/Nano       | 12  | Bandwidth: 1725kB/s Packets/s: 57.5 Success: 100%   |
| Leonardo       | 12  | UNO/Nano       | 12  | Bandwidth: 1725kB/s Packets/s: 57.5 Success: 100%   |
| Mega           | 12  | Leonardo       | 12  | Bandwidth: 1725kB/s Packets/s: 57.5 Success: 100%   |
| Leonardo       | 12  | Leonardo       | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| Leonardo       | 12  | Mega           | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| Leonardo       | 12  | SAMD21         | D0  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| UNO/Nano       | 12  | Mega           | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| UNO/Nano       | 12  | Leonardo       | 12  | Bandwidth: 1740kB/s Packets/s: 58   Success: 100%   |
| UNO/Nano       | 12  | ESP8266 160MHz | D1  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP8266 80MHz  | D1  | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP8266 160MHz | D1  | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP8266 80MHz  | D1  | Mega           | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP8266 160MHz | D1  | Mega           | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| SAMD21         | D0  | Mega           | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| ESP32          | 12  | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| UNO/Nano       | PB9 | Bluepill       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 100%   |
| Bluepill       | PB9 | Bluepill       | PB9 | Bandwidth: 1860kB/s Packets/s: 62   Success: 100%   |
| Bluepill       | PB9 | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 99.5%  |
| SAMD21         | D0  | UNO/Nano       | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 99.00% |
| UNO/Nano       | 12  | ESP32          | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 99.00% |
| Mega           | 12  | ESP32          | 12  | Bandwidth: 1770kB/s Packets/s: 59   Success: 99.00% |
| UNO/Nano       | 12  | ESP8266 80MHz  | D1  | Bandwidth: 1620kB/s Packets/s: 57   Success: 98.28% |
| ESP32          | 12  | Leonardo       | 12  | Bandwidth: 1620kB/s Packets/s: 55   Success: 91.00% |
| Leonardo       | 12  | ESP32          | 12  | Bandwidth: 1620kB/s Packets/s: 55   Success: 91.00% |
