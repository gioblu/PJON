## PacketSeparationTest

This table was filled running the [PacketSeparationTest](../../../../examples/ARDUINO/Local/SoftwareBitBang/PacketSeparationTest) example for 10 seconds and averaging the obtained series of results.

| TX                  |PIN | RX               |PIN | RESULT                                              |
|---------------------|----|------------------|----|-----------------------------------------------------|
| UNO/Nano            | 12 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 100%                   |
| UNO/Nano            | 12 | Mega             | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.999%                |
| UNO/Nano            | 12 | ESP32            | 12 | SWBB_ACCEPTANCE 56 Accuracy: 100%                   |
| Mega                | 12 | Leonardo         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 100%                   |
| Mega                | 12 | ESP32            | 12 | SWBB_ACCEPTANCE 56 Accuracy: 100%                   |
| Leonardo            | 12 | Leonardo         | 12 | SWBB_ACCEPTANCE 56 Accuracy: 100%                   |
| Leonardo            | 12 | ESP32            | 12 | SWBB_ACCEPTANCE 56 Accuracy: 100%                   |
| Leonardo            | 12 | Mega             | 12 | SWBB_ACCEPTANCE 53 Accuracy: 100%                   |
| Leonardo            | 12 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.999%                |
| Mega                | 12 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.999%                |
| SAMD21              | D0 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.89%                 |
| SAMD21              | D0 | Mega             | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.88%                 |
| ESP32               | 12 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.87%                 |
