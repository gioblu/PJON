
### Documentation index
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

---

## Documentation
PJON implements a totally software-defined network protocol stack in around 1500 lines of code, thanks to the [strategies](/src/strategies/README.md) abstraction it can operate transparently on different media, physical layers and other protocols. Thanks to the [interfaces](/src/interfaces/README.md) abstraction PJON can be easily ported on any MCU or architecture. The graph below shows the implementation's architecture.

```cpp
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
