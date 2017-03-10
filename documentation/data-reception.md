- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- **[Data reception](/documentation/data-reception.md)**
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)

Define a `void function` that will be called if a correct message is received. This function receives 3 parameters: the transmission content, its length and a pointer to a `PJON_Packet_Info` data structure that contains all the info contained in the packet metadata:
```cpp
void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
  Serial.print("Header: ");
  Serial.print(packet_info.header, BIN);
  // If packet formatted for a shared medium
  if((packet_info.header & PJON_MODE_BIT) != 0) {
    Serial.print(" Receiver bus id: ");
    Serial.print(packet_info.receiver_bus_id[0]);
    Serial.print(packet_info.receiver_bus_id[1]);
    Serial.print(packet_info.receiver_bus_id[2]);
    Serial.print(packet_info.receiver_bus_id[3]);
    Serial.print(" Device id: ");
    Serial.print(packet_info.receiver_id);
    // If sender info is included
    if((packet_info.header & PJON_TX_INFO_BIT) != 0) {
      Serial.print(" Sender bus id: ");
      Serial.print(packet_info.sender_bus_id[0]);
      Serial.print(packet_info.sender_bus_id[1]);
      Serial.print(packet_info.sender_bus_id[2]);
      Serial.print(packet_info.sender_bus_id[3]);
      Serial.print(" device id: ");
      Serial.println(packet_info.sender_id);
      // Reply to sender with "Hello!"
      bus.reply("Hello!", 6);
    }  
  }

  Serial.print(" Content: ");
  for(uint8_t i = 0; i < length; i++)
    Serial.print((char)payload[i]);

  Serial.print(" Length: ");
  Serial.println(length);
};
```
Inform the bus to call `receiver_function` when a correct message is received:
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
