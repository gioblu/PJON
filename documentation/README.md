
### Documentation index
- [Addressing](/documentation/addressing.md)
  - [`set_id`](/documentation/addressing.md#local-mode) [`device_id`](/documentation/addressing.md#local-mode) [`get_bus_id`](/documentation/addressing.md#shared-mode) [`set_bus_id`](/documentation/addressing.md#shared-mode) [`get_mac`](/documentation/addressing.md#get-or-set-hardware-identifier) [`set_mac`](/documentation/addressing.md#get-or-set-hardware-identifier)
- [Configuration](/documentation/configuration.md)
  - [`set_communication_mode`](/documentation/configuration.md#communication-mode) [`set_shared_network`](/documentation/configuration.md#network-mode) [`set_router`](/documentation/configuration.md#router-mode) [`include_sender_info`](/documentation/configuration.md#sender-information) [`set_crc_32`](/documentation/configuration.md#crc-configuration) [`set_packet_auto_deletion`](/documentation/configuration.md#packet-handling) [`set_acknowledge`](/documentation/configuration.md#acknowledgement) [`set_packet_id`](/documentation/configuration.md#packet-identification) [`include_port`](/documentation/configuration.md#network-service-identification) [`include_mac`](/documentation/configuration.md#hardware-identification)
- [Data reception](/documentation/data-reception.md)
  - [`set_receiver`](/documentation/data-reception.md#data-reception) [`receive`](/documentation/data-reception.md#data-reception)
- [Data structures](/documentation/data-structures.md)
  - [`PJON_Endpoint`](/documentation/data-structures.md#pjon_endpoint) [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info)
- [Data transmission](/documentation/data-transmission.md)
  - [`begin`](/documentation/data-transmission.md#begin) [`reply`](/documentation/data-transmission.md#reply) [`reply_blocking`](/documentation/data-transmission.md#reply_blocking) [`send`](/documentation/data-transmission.md#send) [`send_packet`](/documentation/data-transmission.md#send_packet) [`send_packet_blocking`](/documentation/data-transmission.md#send_packet_blocking) [`send_repeatedly`](/documentation/data-transmission.md#send_repeatedly) [`reply`](/documentation/data-transmission.md#reply) [`reply_blocking`](/documentation/data-transmission.md#reply_blocking) [`forward`](/documentation/data-transmission.md#forward) [`forward_blocking`](/documentation/data-transmission.md#forward_blocking)
- [Error handling](/documentation/error-handling.md)
  - [`set_error`](/documentation/error-handling.md#error-handling)
- [Routing](/documentation/routing.md)
   - [`PJONSimpleSwitch`](/documentation/routing.md#simpleswitch) [`PJONSwitch`](/documentation/routing.md#switch) [`PJONRouter`](/documentation/routing.md#router) [`PJONDynamicRouter`](/documentation/routing.md#dynamicrouter) [`PJONInteractiveRouter`](/documentation/routing.md#interactiverouter)
- [IO setup](/documentation/io-setup.md)
   - [`strategy`](/documentation/io-setup.md#io-setup)
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
