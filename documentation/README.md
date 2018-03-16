
PJON implements a totally software emulated network protocol stack in around 1500 lines of code, thanks to the [strategies](/src/strategies/README.md) abstraction it can work nominally with any data link is in use and can be applied in a variety of media and applications. This library can be cross-compiled (`-std=c++11`) to work with any Arduino compatible device, linux or windows x86 machine.

Thanks to the [interfaces](/src/interfaces/README.md) abstraction, PJON can be easily ported on any MCU/architecture exposing random generation, timing and IO system calls (for example on Arduino: `random`, `micros`, `pinMode`).

```cpp
  Implementation model
       ___________________________________________________
      | Layer 3 Network                                   |
      | PJON                                              |
   ___| send, send_packet, send_repeatedly ...            |
  |   |___________________________________________________|
  |   | Layer 2 data link or strategy                     |
  |   | AnalogSampling, OverSampling, SoftwareBitBang ... |
  --->| can_start, send_string, receive_byte ...          |___   
      |___________________________________________________|   |
      | Interfaces to system calls                        |   |
   ___| PJON_MICROS, PJON_RANDOM, PJON_IO_MODE ...        |<---
  |   |___________________________________________________|
  |   | Layer 1 physical-layer                            |
  |   | System calls:                                     |
  --->| micros, random, delayMicroseconds ...             |
      |___________________________________________________|
```

This library exposes to users a set of methods to easily configure and handle communication between devices:

- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)
