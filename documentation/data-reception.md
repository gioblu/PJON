
### Documentation index
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- **[Data reception](/documentation/data-reception.md)**
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

---

## Data reception
A function of type `void` is called by the PJON object when a packet is received. This function receives 3 parameters: the received payload of type `uint8_t *`, its length of type `uint16_t` and a pointer to a data structure of type `const PJON_Packet_Info` that contains all packet's metadata:

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

Within `receiver_function` it is possible to process data and meta-data when a packet is received. The `PJON_Packet_Info` struct contains all the protocol fields present in the packet:

```cpp
struct PJON_Packet_Info {
  PJON_End_Point tx;
  PJON_End_Point rx;
  void *custom_pointer;
  uint8_t header = PJON_NO_HEADER;
  uint8_t hops = 0;
  uint16_t id = 0;
  uint16_t port = PJON_BROADCAST;
};
```
`PJON_Packet_Info` contains `port` if [`PJON_INCLUDE_PORT`](/documentation/configuration.md#network-service-identification) is defined and `id` if [`PJON_INCLUDE_PACKET_ID`](/documentation/configuration.md#packet-identification) is defined. The conditional inclusion is present to reduce the footprint of programs where the port and the packet id are not used.

`PJON_Packet_Info` contains `tx` and `rx` of type `PJON_End_point` where the packet's sender and recipient information is contained:
```cpp
struct PJON_End_Point {
  uint8_t id = PJON_NOT_ASSIGNED;
  uint8_t bus_id[4] = {0, 0, 0, 0};
  uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
};
```
`PJON_End_Point` contains `mac` if [`PJON_INCLUDE_MAC`](/documentation/configuration.md#hardware-identification) is defined. The conditional inclusion is present to reduce the footprint of programs where the MAC address is not used.

The code below is part of the Arduino compatible [PortsUseExample](/examples/ARDUINO/Network/SoftwareBitBang/PortsUseExample/Receiver/Receiver.ino). When the `receiver_function` is called meta-data present in the `info` parameter is transmitted over serial:
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
Consider that SoftwareBitBang, OverSampling or AnalogSampling are strategies able receive data only while `bus.receive` is being executed, otherwise data is lost and transmitter will try again in future. In this particular case it is mandatory to dedicate a certain timeframe, depending on the duration of the other tasks, to efficiently receive data and avoid repetitions.
