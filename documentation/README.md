- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)

PJON implements a totally software emulated network protocol stack in few thousands lines of C++ engineered to work nominally whatever data-link is in use, thanks to the [strategies](/strategies/README.md) abstraction, enabling the PJON protocol to be applied in a variety of media and applications. The PJON library can be successfully cross-compiled to work with virtually every Arduino compatible board and whatever linux machine, for example Raspberry Pi. Thanks to the [interfaces](/interfaces/README.md) abstraction, PJON can be easily ported on whatever computer providing with random generation, timing and IO system calls (for example on Arduino: `random`, `micros`, `pinMode`).

```cpp
  Implementation model
       _______________________________________________________
      | Layer 3 Network                                       |
   ___| PJON                                                  |
  |   | send / send_packet / send_repeatedly ...              |
  |   |_______________________________________________________|
  |   | Layer 2 data-link layer                               |
  --->| Strategies                                            |___
      | AnalogSampling / OverSampling / SoftwareBitBang ...   |   |
      |_______________________________________________________|   |
      | System calls                                          |   |
   ___| Interfaces:                                           |<---
  |   | PJON_MICROS / PJON_RANDOM ...                         |
  |   |_______________________________________________________|
  |   | Layer 1 physical-layer                                |
  --->| System calls:                                         |
      | micros / random / delayMicroseconds ...               |
      |_______________________________________________________|
```

This library exposes to users a set of methods to easily configure and handle communication between devices.
