
### Documentation index
- [Addressing](/documentation/addressing.md)  
  - [`device_id`](/documentation/addressing.md#local-mode)
  - [`get_bus_id`](/documentation/addressing.md#shared-mode)
  - [`get_mac`](/documentation/addressing.md#get-or-set-hardware-identifier)
  - [`set_bus_id`](/documentation/addressing.md#shared-mode)
  - [`set_id`](/documentation/addressing.md#local-mode)
  - [`set_mac`](/documentation/addressing.md#get-or-set-hardware-identifier)
- [Configuration](/documentation/configuration.md)
  - [`include_mac`](/documentation/configuration.md#hardware-identification)
  - [`include_port`](/documentation/configuration.md#network-service-identification)
  - [`include_sender_info`](/documentation/configuration.md#sender-information)
  - [`set_acknowledge`](/documentation/configuration.md#acknowledgement)
  - [`set_communication_mode`](/documentation/configuration.md#communication-mode)
  - [`set_crc_32`](/documentation/configuration.md#crc-configuration)
  - [`set_packet_auto_deletion`](/documentation/configuration.md#packet-handling)
  - [`set_packet_id`](/documentation/configuration.md#packet-identification)
  - [`set_router`](/documentation/configuration.md#router-mode)
  - [`set_shared_network`](/documentation/configuration.md#network-mode)
- [Data reception](/documentation/data-reception.md)
  - [`receive`](/documentation/data-reception.md#data-reception)
  - [`set_receiver`](/documentation/data-reception.md#data-reception)
- **[Data structures](/documentation/data-structures.md)**
  - [`PJON_Endpoint`](/documentation/data-structures.md#pjon_endpoint)
  - [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info)
- [Data transmission](/documentation/data-transmission.md)
  - [`begin`](/documentation/data-transmission.md#begin)
  - [`reply`](/documentation/data-transmission.md#reply)
  - [`reply_blocking`](/documentation/data-transmission.md#reply_blocking)
  - [`send`](/documentation/data-transmission.md#send)
  - [`send_packet`](/documentation/data-transmission.md#send_packet)
  - [`send_packet_blocking`](/documentation/data-transmission.md#send_packet_blocking)
  - [`send_repeatedly`](/documentation/data-transmission.md#send_repeatedly)
- [Error handling](/documentation/error-handling.md)
  - [`set_error`](/documentation/error-handling.md#error-handling)
- [Routing](/documentation/routing.md)
  - [`PJONSimpleSwitch`](/documentation/routing.md#simpleswitch)
  - [`PJONSwitch`](/documentation/routing.md#switch)
  - [`PJONRouter`](/documentation/routing.md#router)
  - [`PJONDynamicRouter`](/documentation/routing.md#dynamicrouter)
  - [`PJONInteractiveRouter`](/documentation/routing.md#interactiverouter)
- [IO setup](/documentation/io-setup.md)
   - [`strategy`](/documentation/io-setup.md#io-setup)
---

## Data structures

### `PJON_Endpoint`
`PJON_Endpoint` contains the device id, bus id and MAC address of a device:
```cpp
struct PJON_Endpoint {
  uint8_t id = PJON_NOT_ASSIGNED;
  uint8_t bus_id[4] = {0, 0, 0, 0};
  uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
};
```
`PJON_Endpoint` contains `mac` if [`PJON_INCLUDE_MAC`](/documentation/configuration.md#hardware-identification) is defined. The conditional inclusion is present to reduce the footprint of programs where the MAC address is not used.

### `PJON_Packet_Info`
`PJON_Packet_Info` contains all meta-data supported by the PJON packet format. The `tx` and `rx` data structures of type `PJON_Endpoint` contain the transmitter and receiver information.

```cpp
struct PJON_Packet_Info {
  PJON_Endpoint tx;
  PJON_Endpoint rx;
  void *custom_pointer;
  uint8_t header = PJON_NO_HEADER;
  uint8_t hops = 0;
  uint16_t id = 0;
  uint16_t port = PJON_BROADCAST;
};
```
`PJON_Packet_Info` contains `port` if [`PJON_INCLUDE_PORT`](/documentation/configuration.md#network-service-identification) is defined and `id` if [`PJON_INCLUDE_PACKET_ID`](/documentation/configuration.md#packet-identification) is defined. The conditional inclusion is present to reduce the footprint of programs where the port and the packet id are not used.

The `custom_pointer` can be used to link other classes or instances passing any sort of data structure. To understand how to use it see the [ClassMemberCallback](../examples/ARDUINO/Local/SoftwareBitBang/ClassMemberCallback) example.  
