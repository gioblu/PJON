- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)

PJON implements a totally software emulated network protocol stack in around 1500 lines of C++, thanks to the [strategies](/strategies/README.md) abstraction it can work nominally with any data-link is in use and can be applied in a variety of media and applications. This library can be successfully cross-compiled to work with virtually every Arduino compatible board and linux machine, for example ATtiny85 or Raspberry Pi. Thanks to the [interfaces](/interfaces/README.md) abstraction, PJON can be easily ported on whatever MCU/architecture exposing random generation, timing and IO system calls (for example on Arduino: `random`, `micros`, `pinMode`).

```cpp
  Implementation model
       _______________________________________________________
      | Layer 3 Network                                       |
   ___| PJON                                                  |
  |   | send / send_packet / send_repeatedly ...              |
  |   |_______________________________________________________|
  |   | Layer 2 data-link or strategy                         |
  --->| AnalogSampling / OverSampling / SoftwareBitBang ...   |___
      | can_start / send_string / receive_byte ...            |   |
      |_______________________________________________________|   |
   ___| Interfaces to system calls                            |<---
  |   | PJON_MICROS / PJON_RANDOM ...                         |
  |   |_______________________________________________________|
  |   | Layer 1 physical-layer                                |
  --->| System calls:                                         |
      | micros / random / delayMicroseconds ...               |
      |_______________________________________________________|
```

This library exposes to users a set of methods to easily configure and handle communication between devices.
