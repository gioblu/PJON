
### Documentation index
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data structures](/documentation/data-structures.md)
- **[Data transmission](/documentation/data-transmission.md)**
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

---

## Data transmission

### `begin`
The begin method must be called before starting communication, the lack of this call can lead to collisions after boot, so be sure not to forget it.
```cpp  
  bus.begin();
```

### `send_packet`
The simplest way to send data is to use `send_packet`, this method composes the packet and tries to send it once. Consider that if the bus is busy or interference is present the transmission may not be attempted. The method returns the result of its operation. The first parameter is the device id of the recipient of type `uint8_t`, the second is the payload of type `const void *` and the third is the length of type `uint16_t`. This call implies a single attempt and has no guarantee of success, but logs the result of the attempted transmission:
```cpp
// Send to device id 10 the string "Hi!"
bus.send_packet(10, "Hi!", 3);
```
The payload length is boring to be added in each call but is there to prevent buffer overflow. If sending arbitrary values `NULL` terminator strategy based on `strlen` is not safe to detect the end of a string. The `send` method can receive other 3 optional parameters, the header of type `uint8_t`, a packet id of type `uint16_t` (pass 0 if you want to avoid the packet id inclusion) and a port of type `uint16_t`. In the example below a packet containing the payload "Hello" is sent to device id 10 using the actual instance's header configuration, without including the packet id and including the port `8002`.
```cpp
// All optional parameters available
bus.send_packet(
  10,         // Device id (uint8_t)
  "Hello",    // Payload   (const void *)
  5,          // Length    (uint16_t)
  bus.config, // Header    (uint8_t)  - Use default config
  0,          // Packet id (uint16_t) - Don't include packet id
  8002        // Port      (uint16_t)
);
```

`send_packet` returns the following values:
- `PJON_ACK` (6) if transmission occurred and acknowledgement is received if requested
- `PJON_BUSY` (666) if bus is busy
- `PJON_FAIL` (65535) if transmission failed

The `send_packet` result, of type `uint16_t`, can be used to determine if the transmission occurred successfully or not:
```cpp
uint16_t result = bus.send_packet(10, "All is ok?!", 11);

if(result == PJON_ACK) Serial.println("10 is ok!");
```

To broadcast a message to all connected devices, use the `PJON_BROADCAST` constant as recipient ID:
```cpp
bus.send_packet(PJON_BROADCAST, "Message for all connected devices.", 34);
```

If you need to transmit in shared mode or configure other protocol fields you can use `send_packet` passing a struct of type `PJON_Packet_Info`, the payload of type `const void *` and the length of type `uint16_t`:
```cpp
uint8_t bus_id[4] = {0, 0, 0, 1};
PJON_Packet_Info info;
info.rx.id = 10; // The recipient's device id is 10
info.rx.port = 8000; // The packet includes the port 8000
memcpy(info.rx.bus_id, bus_id, 4); // Copy recipient's bus id in info
bus.send_packet(info, "Ciao!", 5);
```

### `send_packet_blocking`
Use `send_packet_blocking` if it is necessary to try until the packet is effectively received by the recipient or a maximum amount of retries is reached. Consider that the method may block the operation of the program for up to 4 seconds in case of transmission failure.

```cpp
// Send to device id 10 the string "Hi!"
bus.send_packet_blocking(10, "Hi!", 3);
```
The `send_packet_blocking` method can receive other 4 optional parameters, the header of type `uint8_t`, a packet id of type `uint16_t` (pass 0 if you want to avoid the packet id inclusion), a port of type `uint16_t` and a timeout of type `uint32_t`. In the example below a packet containing the payload "Hello" is sent to device id 10 using the actual instance's header configuration, without including the packet id, including the port `8002` and passing a maximum timeout of 1 second:
```cpp
// All optional parameters available
bus.send_packet_blocking(
  10,         // Device id (uint8_t)
  "Hello",    // Payload   (const void *)
  5,          // Length    (uint16_t)
  bus.config, // Header    (uint8_t)  - Use default config
  0,          // Packet id (uint16_t) - Don't include packet id
  8002,       // Port      (uint16_t)
  1000000     // Timeout   (uint32_t) - 1 second
);
```
If you need to transmit data in shared mode you can use the same `send_packet_blocking` method including also the bus id. In the example below a packet containing the payload "Hi!" is sent to device id 10:
```cpp
// Send to bus id 0.0.0.1
uint8_t bus_id[] = {0, 0, 0, 1};
bus.send_packet_blocking(10, bus_id, "Hi!", 3);
```
As its local version, `send_packet_blocking` can receive other 4 optional parameters, the header of type `uint8_t`, a packet id of type `uint16_t` (pass 0 if you want to avoid the packet id inclusion), a port of type `uint16_t` and a timeout of type `uint32_t`. In the example below a packet containing the payload "Hello" is sent to device id 10 using the actual instance's header configuration, without including the packet id, including the port `8002` and passing a maximum timeout of 1 second:
```cpp
// All optional parameters available
bus.send_packet_blocking(
  10,         // Device id (uint8_t)
  bus_id,     // Bus id    (uint8_t *)
  "Hello",    // Payload   (const void *)
  5,          // Length    (uint16_t)
  bus.config, // Header    (uint8_t)  - Use default config
  0,          // Packet id (uint16_t) - Don't include packet id
  8002,       // Port      (uint16_t)
  1000000     // Timeout   (uint32_t) - 1 second
);
```
`send_packet_blocking` returns the following values:
- `PJON_ACK` (6) if transmission occurred and acknowledgement is received if requested
- `PJON_BUSY` (666) if bus is busy
- `PJON_FAIL` (65535) if transmission failed

The `send_packet_blocking` result, of type `uint16_t`, can be used to determine if the transmission occurred successfully or not:
```cpp
uint16_t result = bus.send_packet_blocking(10, "All is ok?!", 11);

if(result == PJON_ACK) Serial.println("10 is ok!");
```

If you need to transmit in shared mode or configure other protocol fields you can use `send_packet_blocking` passing a struct of type `PJON_Packet_Info`, the payload of type `const void *` and the length of type `uint16_t`:
```cpp
uint8_t bus_id[4] = {0, 0, 0, 1};
PJON_Packet_Info info;
info.rx.id = 10; // The recipient's device id is 10
info.rx.port = 8000; // The packet includes the port 8000
memcpy(info.rx.bus_id, bus_id, 4); // Copy recipient's bus id in info
bus.send_packet_blocking(info, "Ciao!", 5);
```

### `send`

When using the `send` method, PJON operates using its internal buffer, although a little more memory is required. The first thing to do and never forget is to call the `update` method once per loop cycle:
```cpp  
  bus.update();
```
Every time `update` is called the transmission is attempted for each packet present in the buffer.

To send data to another device connected to the bus simply call `send` passing the device id of the recipient of type `uint8_t` then the payload of type `const void *` and its length of type `uint16_t`. The return value of `send` is of type `uint16_t` and it is the id of the packet in the buffer or `PJON_FAIL` in case of error.
```cpp
bus.send(100, "Ciao, this is a test!", 21);
```
`send` can receive 3 optional parameters, the header of type `uint8_t`, a packet id of type `uint16_t` (pass 0 if you want to avoid the packet id inclusion) and a port of type `uint16_t`. In the example below a packet containing the payload "Hello" is sent to device id 2 using the actual instance's header configuration, without including the packet id and including the port `8002`:
```cpp
// Shared mode or using bus indexing
bus.send(
  2,          // (uint8_t)      Recipient device id
  "Hello",    // (const void *) Payload
  5,          // (uint16_t)     Length
  bus.config, // (uint8_t)      Packet header
  0,          // (uint16_t)     Packet id
  8002        // (uint16_t)     Port identification
);
```
To use the return value of `send` just save it in a variable of type `uint16_t`:
```cpp
uint16_t packet = bus.send(100, "Ciao, this is a test!", 21);

if(packet == PJON_FAIL) Serial.print("Something went wrong");
```

If you need to transmit in shared mode or configure other protocol fields you can use `send` passing a struct of type `PJON_Packet_Info`, the payload of type `const void *` and the length of type `uint16_t`:
```cpp
uint8_t bus_id[4] = {0, 0, 0, 1};
PJON_Packet_Info info;
info.rx.id = 10; // The recipient's device id is 10
info.rx.port = 8000; // The packet includes the port 8000
memcpy(info.rx.bus_id, bus_id, 4); // Copy recipient's bus id in info
bus.send(info, "Ciao!", 5);
```

### `send_repeatedly`
The `send_repeatedly` method can be used when it is required to schedule a repeated sending with a given interval. The first parameter is the device id of the recipient of type `uint8_t` then follows the payload of type `const void *`, its length of type `uint16_t` and the interval of type `uint32_t`:
```cpp
// Local sending example
uint16_t one_second_test =
  bus.send_repeatedly(100, "Test sent every second!", 23, 1000000);
/* IMPORTANT: maximum interval supported is
   4293014170 microseconds or 71.55 minutes */

```
`send_repeatedly` returns the id of the packet in the buffer as `send` does, to remove the repeated transmission simply call:
```cpp
bus.remove(one_second_test);
```
`send_repeatedly` can receive 3 optional parameters: the header of type `uint8_t`, a packet id of type `uint16_t` (pass 0 if you want to avoid the packet id inclusion) and a port of type `uint16_t`. In the example below a packet containing the payload "Hello" is sent to device id 2 every second using the actual instance's header configuration, without including the packet id and including the port `8002`:
```cpp
// Shared mode or using bus indexing
bus.send_repeatedly(
  2,          // (uint8_t)      Recipient device id
  "Hello",    // (const void *) Payload
  5,          // (uint16_t)     Length
  1000000     // (uint32_t)     Interval
  bus.config, // (uint8_t)      Packet header
  0,          // (uint16_t)     Packet id
  8002        // (uint16_t)     Port identification
);

If you need to transmit in shared mode or configure other protocol fields you can use `send_repeatedly` passing a struct of type `PJON_Packet_Info`, the payload of type `const void *`, the length of type `uint16_t` and the interval of type `uint32_t`:
```cpp
uint8_t bus_id[4] = {0, 0, 0, 1};
PJON_Packet_Info info;
info.rx.id = 10; // The recipient's device id is 10
info.rx.port = 8000; // The packet includes the port 8000
memcpy(info.rx.bus_id, bus_id, 4); // Copy recipient's bus id in info
bus.send_repeatedly(info, "Ciao!", 5, 1000000); // Send "Ciao!" every second
```
