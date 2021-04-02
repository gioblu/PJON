
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
- **[IO setup](/documentation/io-setup.md)**
   - [`strategy`](/documentation/io-setup.md#io-setup)
---

## IO setup
The physical layer configuration is handled by the [strategy](/src/strategies/README.md) entity, if you want to communicate bi-directionally on a single pin with [SoftwareBitBang](/src/strategies/SoftwareBitBang) or [AnalogSampling](/src/strategies/AnalogSampling) use:
```cpp  
 PJONSoftwareBitBang bus;
 bus.strategy.set_pin(12);
```

With this setup it is possible to communicate in `PJON_HALF_DUPLEX` mode with up to 254 different devices on the same physical pin. If for some reason you need to keep separate the two lines, for example if using [OverSampling](/src/strategies/OverSampling) with separate radio transmitter and receiver modules, you can pass 2 separate pins:
```cpp  
 PJONOverSampling bus;
 bus.strategy.set_pins(11, 12);
```

If you don't need bidirectional communication and you have only the transmitter on one side and the receiver on the other side you can use the `PJON_NOT_ASSIGNED` constant:
```cpp  
 PJONOverSampling bus;
 // Operate in simplex mode
 bus.set_communication_mode(PJON_SIMPLEX);
  // Use only receiver pin
 bus.strategy.set_pins(11, PJON_NOT_ASSIGNED);
```

See the readme of the strategy you are using to have additional information on its dedicated physical layer configuration.
