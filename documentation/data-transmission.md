- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- **[Data transmission](/documentation/data-transmission.md)**
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)

The begin function has to be called in the setup, the lack of this call can lead to collisions on startup, so be sure to call it before making use of the instance.
```cpp  
  bus.begin();
```
The simplest way to send data is to use `send_packet`, this method composes the packet and tries to send it once. The first parameter is the id of the recipient, optionally you can pass the bus id if needed, then follows payload and its length. This call implies a single try and has no guarantee of success, but logs the result of the attempted transmission:
```cpp
// Local
if(bus.send_packet(10, "All is ok?!", 11) == PJON_ACK)
  Serial.println("10 is ok!");

// Shared
uint8_t bus_id[] = {0, 0, 0, 1};
if(bus.send_packet(10, bus_id, "All is ok?!", 11) == PJON_ACK)
  Serial.println("10 is ok!");
```
The sending is executed as soon as the method is called and it returns the following values:
- `PJON_ACK` (6) if a correct reception occurred
- `PJON_NAK` (21) if a mistake is found in CRC
- `PJON_BUSY` (666) if a transmission for other devices is occurring
- `PJON_FAIL` (65535) if no data is received

Use `send_packet_blocking` if it is necessary to try more than once and so comply with the specified back-off.
```cpp
if(bus.send_packet_blocking(10, "All is ok?!", 11) == PJON_ACK)
  Serial.println("10 is ok!");
```
`send_packet_blocking` returns the result of transmission as `send_packet` does.

If you prefer PJON to handle packets for you, you can make use of the packet handler, although a little more memory is needed. The first thing to do and never forget is to call the `update()` function once per loop cycle:
```cpp  
  bus.update();
```
To send data to another device connected to the bus simply call `send` passing the recipient's id (and its bus id if necessary), the payload you want to send and its length:
```cpp
// Local
bus.send(100, "Ciao, this is a test!", 21);

// Shared
uint8_t bus_id[] = {0, 0, 0, 1};
bus.send(100, bus_id, "Ciao, this is a test!", 21);
```
Payload length is boring to be added but is there to prevent buffer overflow. If sending arbitrary values `NULL` terminator strategy based on `strlen` is not safe to detect the end of a string. The `send` call returns an id, that is the reference to the packet you have dispatched. To send a value repeatedly simply call `send_repeatedly` and pass as last parameter the interval in microseconds you want between every sending:
```cpp
uint16_t one_second_test =
  bus.send_repeatedly(100, "Test sent every second!", 23, 1000000);
/* IMPORTANT: maximum interval supported is
   4293014170 microseconds or 71.55 minutes */
```
`send_repeatedly` returns the id of the packet in the packet's buffer as `send` does, to remove this repeated transmission simply:
```cpp
bus.remove(one_second_test);
```
To broadcast a message to all connected devices, use the `PJON_BROADCAST` constant as recipient ID.
```cpp
bus.send(PJON_BROADCAST, "Message for all connected devices.", 34);
```
To transmit data including a custom port, for example `8002` use send with its extended parameters:
```cpp
bus.include_port(true, 8002);
bus.send(100, "Port id test!", 13);

// Or call send passing the port as a parameter:

bus.send(
  100,                       // (uint8_t)         Recipient device id
  bus_id,                    // (const uint8_t *) Recipient bus id
  "Test including port id!", // (const char *)    Content
  23,                        // (uint16_t)        Length
  bus.config,                // (uint8_t)         Packet header
  110,                       // (uint16_t)        Packet id
  8002                       // (uint16_t)        Port identification
);

```
