- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- **[Data reception](/documentation/data-reception.md)**
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)

Define a function of type `void` that will be called if a correct message is received. This function receives 3 parameters: the received payload, its length and a pointer to a data structure of type `PJON_Packet_Info` that contains all packet's metadata:
```cpp
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  Serial.print("Header: ");
  Serial.print(packet_info.header, BIN);
  // If packet formatted for a shared medium
  if(packet_info.header & PJON_MODE_BIT) {
    Serial.print(" Receiver bus id: ");
    Serial.print(packet_info.receiver_bus_id[0]);
    Serial.print(packet_info.receiver_bus_id[1]);
    Serial.print(packet_info.receiver_bus_id[2]);
    Serial.print(packet_info.receiver_bus_id[3]);
    Serial.print(" Receiver id: ");
    Serial.print(packet_info.receiver_id);
    // If sender info is included
    if(packet_info.header & PJON_TX_INFO_BIT) {
      Serial.print(" Sender bus id: ");
      Serial.print(packet_info.sender_bus_id[0]);
      Serial.print(packet_info.sender_bus_id[1]);
      Serial.print(packet_info.sender_bus_id[2]);
      Serial.print(packet_info.sender_bus_id[3]);
    }
  }
  // If sender device id is included
  if(packet_info.header & PJON_TX_INFO_BIT) {
    Serial.print(" Sender id: ");
    Serial.print(packet_info.sender_id);
  }
  // Payload Length
  Serial.print(" Length: ");
  Serial.print(length);
  // If port id is included
  if(packet_info.header & PJON_PORT_BIT) {
    Serial.print(" Port bit: ");
    Serial.print(packet_info.port);
  }
  Serial.println();
}
```
Inform the instance to call `receiver_function` when a correct message is received:
```cpp
bus.set_receiver(receiver_function);
```
To correctly receive data call the `receive` function at least once per loop cycle:
```cpp
uint16_t response = bus.receive();
```
`receive` returns the following values:
- `PJON_ACK` (6) if a correct reception occurred
- `PJON_NAK` (21) if a mistake is found in CRC
- `PJON_BUSY` (666) if a transmission for other devices is occurring
- `PJON_FAIL` (65535) if no data is received

If you want to dedicate a certain timeframe to reception call the `receive` function passing the maximum reception time in microseconds:
```cpp
uint16_t response = bus.receive(1000);
```

To pass custom data to the receiver callback function, se the [ClassMemberCallback](../examples/ARDUINO/Local/SoftwareBitBang/ClassMemberCallback) example. This feature can be used for a lot of different use cases. Could be used to let multiple PJON objects call the same callback function, passing an int specifying which PJON instance has to be called, or a pointer to the PJON object, or an enum or whatever.
