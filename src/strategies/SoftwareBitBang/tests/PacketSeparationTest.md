## PacketSeparationTest

This table was filled running the [PacketSeparationTest](../../../../examples/ARDUINO/Local/SoftwareBitBang/PacketSeparationTest) example for 10 seconds and averaging the obtained series of results.

| MODE | TX                  |PIN | RX               |PIN | RESULT                                              |
|------|---------------------|----|------------------|----|-----------------------------------------------------|
| 1    | UNO/Nano            | 12 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 100%                   |
| 1    | UNO/Nano            | 12 | Mega             | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.999%                |
| 1    | UNO/Nano            | 12 | ESP32            | 12 | SWBB_ACCEPTANCE 56 Accuracy: 100%                   |
| 1    | Mega                | 12 | Leonardo         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 100%                   |
| 1    | Mega                | 12 | ESP32            | 12 | SWBB_ACCEPTANCE 56 Accuracy: 100%                   |
| 1    | Leonardo            | 12 | Leonardo         | 12 | SWBB_ACCEPTANCE 56 Accuracy: 100%                   |
| 1    | Leonardo            | 12 | ESP32            | 12 | SWBB_ACCEPTANCE 56 Accuracy: 100%                   |
| 1    | Leonardo            | 12 | Mega             | 12 | SWBB_ACCEPTANCE 53 Accuracy: 100%                   |
| 1    | Leonardo            | 12 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.999%                |
| 1    | Mega                | 12 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.999%                |
| 1    | SAMD21              | D0 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.89%                 |
| 1    | SAMD21              | D0 | Mega             | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.88%                 |
| 1    | ESP32               | 12 | UNO/Nano         | 12 | SWBB_ACCEPTANCE 53 Accuracy: 99.87%                 |
