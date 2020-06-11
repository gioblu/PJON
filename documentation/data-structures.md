
### Documentation index
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- **[Data structures](/documentation/data-structures.md)**
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

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
