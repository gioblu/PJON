- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- **[Data transmission](/documentation/data-transmission.md)**
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

### Data transmission
The begin function must be called before starting communication, the lack of this call can lead to collisions after boot, so be sure to call it before making use of the instance.
```cpp  
  bus.begin();
```
The simplest way to send data is to use `send_packet`, this method composes the packet and tries to send it once. The first parameter is the device id of the recipient of type `uint8_t`, optionally you can pass the bus id of type `const uint8_t *`, then follows the payload of type `const char *` and its length of type `uint16_t`. This call implies a single attempt and has no guarantee of success, but logs the result of the attempted transmission:
```cpp
// Local

// Send to device id 10 the string "Hi!"
bus.send_packet(10, "Hi!", 3);

// All optional parameters available
bus.send_packet(
  10,             // Device id (uint8_t)
  "Hello World!", // Content   (const char *)
  12,             // Length    (uint16_t)
  bus.config,     // Header    (uint8_t)  - Use default config
  0,              // Packet id (uint16_t) - Don't include packet id
  8002            // Port      (uint16_t)
);

// Shared or using bus indexing

// Send to bus id 0.0.0.1 - device id 10 the string "Hi!"
uint8_t bus_id[] = {0, 0, 0, 1};
bus.send_packet(10, bus_id, "Hi!", 3);

// All optional parameters available
bus.send_packet(
  10,             // Device id (uint8_t)
  bus_id,         // Bus id    (const uint8_t *)
  "Hello World!", // Content   (const char *)
  12,             // Length    (uint16_t)
  bus.config,     // Header    (uint8_t)  - Use default config
  0,              // Packet id (uint16_t) - Don't include packet id
  8002            // Port      (uint16_t)
);
```
The sending is executed as soon as the method is called and it returns the following values:
- `PJON_ACK` (6) if transmission occurred and acknowledgement is received if requested
- `PJON_BUSY` (666) if bus is busy
- `PJON_FAIL` (65535) if transmission failed

```cpp
// Use the value returned by send_packet to determine transmission result

// Local
if(bus.send_packet(10, "All is ok?!", 11) == PJON_ACK)
  Serial.println("10 is ok!");

// Shared or using bus indexing
if(bus.send_packet(10, bus_id, "All is ok?!", 11) == PJON_ACK)
  Serial.println("10 is ok!");
```

To broadcast a message to all connected devices, use the `PJON_BROADCAST` constant as recipient ID:
```cpp
bus.send_packet(PJON_BROADCAST, "Message for all connected devices.", 34);
```

Use `send_packet_blocking` if it is necessary to try until the packet is effectively received by the recipient and so comply with the specified back-off.
```cpp
// Send to device id 10 the string "Hi!"
bus.send_packet_blocking(10, "Hi!", 3);

// Use the value returned by send_packet to determine transmission result
if(bus.send_packet_blocking(10, "All is ok?!", 11) == PJON_ACK)
  Serial.println("10 is ok!");
```
`send_packet_blocking` returns the result of transmission as `send_packet` does.

PJON can also optionally handle packets for you, although a little more memory is required. The first thing to do and never forget is to call the `update()` function once per loop cycle:
```cpp  
  bus.update();
```
To send data to another device connected to the bus simply call `send` passing the recipient's id (and its bus id if necessary), the payload you want to send and its length:
```cpp
// Local
bus.send(100, "Ciao, this is a test!", 21);

// Shared or using bus indexing
uint8_t bus_id[] = {0, 0, 0, 1};
bus.send(100, bus_id, "Ciao, this is a test!", 21);

// All optional parameters available

// Local mode
bus.send(
  100,                       // (uint8_t)      Recipient device id
  "Test including port id!", // (const char *) Content
  23,                        // (uint16_t)     Length
  bus.config,                // (uint8_t)      Packet header
  1,                         // (uint16_t)     Packet id
  8002                       // (uint16_t)     Port identification
);

// Shared mode or using bus indexing
bus.send(
  100,                       // (uint8_t)         Recipient device id
  bus_id,                    // (const uint8_t *) Recipient bus id
  "Test including port id!", // (const char *)    Content
  23,                        // (uint16_t)        Length
  bus.config,                // (uint8_t)         Packet header
  1,                         // (uint16_t)        Packet id
  8002                       // (uint16_t)        Port identification
);

```
Payload length is boring to be added but is there to prevent buffer overflow. If sending arbitrary values `NULL` terminator strategy based on `strlen` is not safe to detect the end of a string. The `send` call returns an id, that is the reference to the packet you have dispatched. To send a value repeatedly simply call `send_repeatedly` and pass as last parameter the interval in microseconds you want between every sending:
```cpp
// Local sending example
uint16_t one_second_test =
  bus.send_repeatedly(100, "Test sent every second!", 23, 1000000);
/* IMPORTANT: maximum interval supported is
   4293014170 microseconds or 71.55 minutes */

// Shared sending example including all optional parameters
uint16_t one_second_test_shared =
  bus.send_repeatedly(
    100,                       // (uint8_t)         Recipient device id
    bus_id,                    // (const uint8_t *) Recipient bus id
    "Test including port id!", // (const char *)    Content
    23,                        // (uint16_t)        Length
    1000000,                   // (uint32_t)        Interval in microseconds
    bus.config,                // (uint8_t)         Packet header
    1,                         // (uint16_t)        Packet id
    8002                       // (uint16_t)        Port identification
  );
```
`send_repeatedly` returns the id of the packet in the packet's buffer as `send` does, to remove this repeated transmission simply:
```cpp
bus.remove(one_second_test);
```
