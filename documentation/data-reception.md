
### Documentation index
- [Addressing](/documentation/addressing.md)
  - [`set_id`](/documentation/addressing.md#local-mode) [`device_id`](/documentation/addressing.md#local-mode) [`get_bus_id`](/documentation/addressing.md#shared-mode) [`set_bus_id`](/documentation/addressing.md#shared-mode) [`get_mac`](/documentation/addressing.md#get-or-set-hardware-identifier) [`set_mac`](/documentation/addressing.md#get-or-set-hardware-identifier)
- [Configuration](/documentation/configuration.md)
  - [`set_communication_mode`](/documentation/configuration.md#communication-mode) [`set_shared_network`](/documentation/configuration.md#network-mode) [`set_router`](/documentation/configuration.md#router-mode) [`include_sender_info`](/documentation/configuration.md#sender-information) [`set_crc_32`](/documentation/configuration.md#crc-configuration) [`set_packet_auto_deletion`](/documentation/configuration.md#packet-handling) [`set_acknowledge`](/documentation/configuration.md#acknowledgement) [`set_packet_id`](/documentation/configuration.md#packet-identification) [`include_port`](/documentation/configuration.md#network-service-identification) [`include_mac`](/documentation/configuration.md#hardware-identification)
- **[Data reception](/documentation/data-reception.md)**
  - [`set_receiver`](/documentation/data-reception.md#data-reception) [`receive`](/documentation/data-reception.md#data-reception)
- [Data structures](/documentation/data-structures.md)
  - [`PJON_Endpoint`](/documentation/data-structures.md#pjon_endpoint) [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info)
- [Data transmission](/documentation/data-transmission.md)
  - [`begin`](/documentation/data-transmission.md#begin) [`reply`](/documentation/data-transmission.md#reply) [`reply_blocking`](/documentation/data-transmission.md#reply_blocking) [`send`](/documentation/data-transmission.md#send) [`send_packet`](/documentation/data-transmission.md#send_packet) [`send_packet_blocking`](/documentation/data-transmission.md#send_packet_blocking) [`send_repeatedly`](/documentation/data-transmission.md#send_repeatedly)
- [Error handling](/documentation/error-handling.md)
  - [`set_error`](/documentation/error-handling.md#error-handling)
- [Routing](/documentation/routing.md)
  - [`PJONSimpleSwitch`](/documentation/routing.md#simpleswitch) [`PJONSwitch`](/documentation/routing.md#switch) [`PJONRouter`](/documentation/routing.md#router) [`PJONDynamicRouter`](/documentation/routing.md#dynamicrouter) [`PJONInteractiveRouter`](/documentation/routing.md#interactiverouter)
- [IO setup](/documentation/io-setup.md)
   - [`strategy`](/documentation/io-setup.md#io-setup)
---

## Data reception
A function of type `void` is called by the PJON object when a packet is received. This function receives 3 parameters: the received payload of type `uint8_t *`, its length of type `uint16_t` and a pointer to a data structure of type [`const PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info) that contains all packet's metadata:

```cpp
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info) {
  // Print received data in the serial monitor
  for(uint16_t i = 0; i < length; i++)
    Serial.print(payload[i]);
};
```

Register the `receiver_function` as the receiver callback:
```cpp
PJON<SoftwareBitBang> bus;
bus.set_receiver(receiver_function);
```

Within `receiver_function` it is possible to process data and meta-data when a packet is received. The [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info) struct contains all the protocol fields present in the packet:

The code below is part of the Arduino compatible [PortsUseExample](/examples/ARDUINO/Network/SoftwareBitBang/PortsUseExample/Receiver/Receiver.ino). When the `receiver_function` is called meta-data present in the `info` parameter is printed in the serial monitor:
```cpp
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &info) {
  Serial.print("Header: ");
  Serial.print(info.header, BIN);
  // If packet formatted for a shared medium
  if(info.header & PJON_MODE_BIT) {
    Serial.print(" Receiver bus id: ");
    Serial.print(info.rx.bus_id[0]);
    Serial.print(info.rx.bus_id[1]);
    Serial.print(info.rx.bus_id[2]);
    Serial.print(info.rx.bus_id[3]);
    Serial.print(" Receiver id: ");
    Serial.print(info.rx.id);
    // If sender info is included
    if(info.header & PJON_TX_INFO_BIT) {
      Serial.print(" Sender bus id: ");
      Serial.print(info.tx.bus_id[0]);
      Serial.print(info.tx.bus_id[1]);
      Serial.print(info.tx.bus_id[2]);
      Serial.print(info.tx.bus_id[3]);
    }
  }
  // If sender device id is included
  if(info.header & PJON_TX_INFO_BIT) {
    Serial.print(" Sender id: ");
    Serial.print(info.tx.id);
  }
  // Payload Length
  Serial.print(" Length: ");
  Serial.print(length);
  // If port id is included
  if(info.header & PJON_PORT_BIT) {
    Serial.print(" Port bit: ");
    Serial.print(info.port);
  }
  Serial.println();
};
```
Use `payload` before any transmission, the buffer where `payload` points to is overwritten when a new packet is dispatched.

To understand how to pass custom data to the receiver callback function, see the [ClassMemberCallback](../examples/ARDUINO/Local/SoftwareBitBang/ClassMemberCallback) example. This feature can be used to link other classes or instances passing any sort of data structure.

To receive data the `receive` function must be called as often as possible:
```cpp
uint16_t response = bus.receive();
```
`receive` returns the following values:
- `PJON_ACK` (6) if a correct reception occurred
- `PJON_NAK` (21) if a mistake is found in CRC
- `PJON_BUSY` (666) if a transmission for other devices is occurring
- `PJON_FAIL` (65535) if no data is received

If it is required to dedicate a certain time to reception call the `receive` function passing the maximum reception time in microseconds:
```cpp
uint16_t response = bus.receive(1000);
```
Consider that SoftwareBitBang, OverSampling or AnalogSampling are strategies able to receive data only while `bus.receive` is being executed, otherwise data is lost and transmitter will try again in future. In this particular case it is mandatory to dedicate a certain timeframe, depending on the duration of the other tasks, to efficiently receive data and avoid repetitions.
