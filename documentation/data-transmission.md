
### Documentation index
- [Addressing](/documentation/addressing.md)
  - [`set_id`](/documentation/addressing.md#local-mode) [`device_id`](/documentation/addressing.md#local-mode) [`get_bus_id`](/documentation/addressing.md#shared-mode) [`set_bus_id`](/documentation/addressing.md#shared-mode) [`get_mac`](/documentation/addressing.md#get-or-set-hardware-identifier) [`set_mac`](/documentation/addressing.md#get-or-set-hardware-identifier)
- [Configuration](/documentation/configuration.md)
  - [`set_communication_mode`](/documentation/configuration.md#communication-mode) [`set_shared_network`](/documentation/configuration.md#network-mode) [`set_router`](/documentation/configuration.md#router-mode) [`include_sender_info`](/documentation/configuration.md#sender-information) [`set_crc_32`](/documentation/configuration.md#crc-configuration) [`set_packet_auto_deletion`](/documentation/configuration.md#packet-handling) [`set_acknowledge`](/documentation/configuration.md#acknowledgement) [`set_packet_id`](/documentation/configuration.md#packet-identification) [`include_port`](/documentation/configuration.md#network-service-identification) [`include_mac`](/documentation/configuration.md#hardware-identification)
- [Data reception](/documentation/data-reception.md)
  - [`set_receiver`](/documentation/data-reception.md#data-reception) [`receive`](/documentation/data-reception.md#data-reception)
- [Data structures](/documentation/data-structures.md)
  - [`PJON_Endpoint`](/documentation/data-structures.md#pjon_endpoint) [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info)
- **[Data transmission](/documentation/data-transmission.md)**
  - [`begin`](/documentation/data-transmission.md#begin) [`reply`](/documentation/data-transmission.md#reply) [`reply_blocking`](/documentation/data-transmission.md#reply_blocking) [`send`](/documentation/data-transmission.md#send) [`send_packet`](/documentation/data-transmission.md#send_packet) [`send_packet_blocking`](/documentation/data-transmission.md#send_packet_blocking) [`send_repeatedly`](/documentation/data-transmission.md#send_repeatedly)
- [Error handling](/documentation/error-handling.md)
  - [`set_error`](/documentation/error-handling.md#error-handling)
- [Routing](/documentation/routing.md)
  - [`PJONSimpleSwitch`](/documentation/routing.md#simpleswitch) [`PJONSwitch`](/documentation/routing.md#switch) [`PJONRouter`](/documentation/routing.md#router) [`PJONDynamicRouter`](/documentation/routing.md#dynamicrouter) [`PJONInteractiveRouter`](/documentation/routing.md#interactiverouter)
- [IO setup](/documentation/io-setup.md)
   - [`strategy`](/documentation/io-setup.md#io-setup)
---

## Data transmission

### `begin`
The `begin` method must be called before starting communication, the lack of this call can lead to collisions so be sure not to forget it:
```cpp  
  bus.begin();
```

### `send_packet`

| Buffered | Blocking | Attempts |
| -------- | -------- | -------- |
| No       | Yes      | 1        |

The simplest way to send data is to use `send_packet`, this method composes the packet and tries to send it once. Consider that if the bus is busy or interference is present the transmission may not be attempted. The method returns the result of its operation. The first parameter is the device id of the recipient of type `uint8_t`, the second is the payload of type `const void *` and the third is the length of type `uint16_t`. This call implies a single attempt and has no guarantee of success, but logs the result of the attempted transmission:
```cpp
// Send to device id 10 the string "Hi!"
bus.send_packet(10, "Hi!", 3);
```
 The `send_packet` method can receive 3 optional parameters, the header of type `uint8_t`, a packet id of type `uint16_t` (pass 0 if you want to avoid the packet id inclusion) and a port of type `uint16_t`. In the example below a packet containing the payload "Hello" is sent to device id 10 using the actual instance's header configuration, without including the packet id and including the port `8002`.
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

The `send_packet` method accepts any kind of data, in the example below a custom `struct` is sent to device id 1:
```cpp
// Define a custom data type
struct voltage_record { uint16_t v1; uint16_t v2; };
// Fill it with information
voltage_record record;
record.v1 = analogRead(A1);
record.v2 = analogRead(A2);
// Send to device id 1 the record struct
bus.send_packet(1, &record, sizeof(record));
```

`send_packet` returns the following values:
- `PJON_ACK` (6) if transmission occurred and acknowledgement is received if requested
- `PJON_BUSY` (666) if bus is busy
- `PJON_FAIL` (65535) if transmission failed

The `send_packet` return value, of type `uint16_t`, can be used to determine if the transmission occurred successfully or not:
```cpp
uint16_t result = bus.send_packet(10, "All is ok?!", 11);

if(result == PJON_ACK) Serial.println("10 is ok!");
```

To broadcast a message to all connected devices, use the `PJON_BROADCAST` constant as recipient ID:
```cpp
bus.send_packet(PJON_BROADCAST, "Message for all connected devices.", 34);
```

If you need to transmit in shared mode or configure other protocol fields you can use `send_packet` passing a struct of type [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info), the payload of type `const void *` and the length of type `uint16_t`:
```cpp
uint8_t bus_id[4] = {0, 0, 0, 1};
PJON_Packet_Info info;
info.rx.id = 10; // The recipient's device id is 10
info.rx.port = 8000; // The packet includes the port 8000
memcpy(info.rx.bus_id, bus_id, 4); // Copy recipient's bus id in info
bus.send_packet(info, "Ciao!", 5);
```

### `send_packet_blocking`

| Buffered | Blocking | Attempts                      |
| -------- | -------- | ----------------------------- |
| No       | Yes      | `strategy.get_max_attempts()` |

Use `send_packet_blocking` if it is necessary to try until the packet is effectively received by the recipient or a maximum amount of retries is reached. Consider that the method may block the operation of the program for up to 4 seconds in case of transmission failure.

```cpp
// Send to device id 10 the string "Hi!"
bus.send_packet_blocking(10, "Hi!", 3);
```
The `send_packet_blocking` method can receive 4 optional parameters, the header of type `uint8_t`, a packet id of type `uint16_t` (pass 0 if you want to avoid the packet id inclusion), a port of type `uint16_t` and a timeout of type `uint32_t`. In the example below a packet containing the payload "Hello" is sent to device id 10 using the actual instance's header configuration, without including the packet id, including the port `8002` and passing a maximum timeout of 1 second:
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
The `send_packet_blocking` method accepts any type of data, in the example below a custom `struct` is sent to device id 1:
```cpp
// Define a custom data type
struct voltage_record { uint16_t v1; uint16_t v2; };
// Fill it with information
voltage_record record;
record.v1 = analogRead(A1);
record.v2 = analogRead(A2);
// Send to device id 1 the record struct
bus.send_packet_blocking(1, &record, sizeof(record));
```

`send_packet_blocking` returns the following values:
- `PJON_ACK` (6) if transmission occurred and acknowledgement is received if requested
- `PJON_BUSY` (666) if bus is busy
- `PJON_FAIL` (65535) if transmission failed

The `send_packet_blocking` return value, of type `uint16_t`, can be used to determine if the transmission occurred successfully or not:
```cpp
uint16_t result = bus.send_packet_blocking(10, "All is ok?!", 11);

if(result == PJON_ACK) Serial.println("10 is ok!");
```

If you need to transmit in shared mode or configure other protocol fields you can use `send_packet_blocking` passing a struct of type [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info), the payload of type `const void *` and the length of type `uint16_t`:
```cpp
uint8_t bus_id[4] = {0, 0, 0, 1};
PJON_Packet_Info info;
info.rx.id = 10; // The recipient's device id is 10
info.rx.port = 8000; // The packet includes the port 8000
memcpy(info.rx.bus_id, bus_id, 4); // Copy recipient's bus id in info
bus.send_packet_blocking(info, "Ciao!", 5);
```

### `send`

| Buffered | Blocking | Attempts                      |
| -------- | -------- | ----------------------------- |
| Yes      | No       | `strategy.get_max_attempts()` |

When using the `send` method, PJON operates using its internal buffer, although a little more memory is required, this call is non-blocking and automatically handles back-off. The first thing to do and never forget is to call the `update` method once per loop cycle:
```cpp  
  bus.update();
```
Every time `update` is called the transmission is attempted for each packet present in the buffer.

To send data to another device connected to the bus simply call `send` passing the device id of the recipient of type `uint8_t` then the payload of type `const void *` and its length of type `uint16_t`. The return value of `send` of type `uint16_t` is the index of the packet in the `packets` buffer or `PJON_FAIL` in case of error.
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

The `send` method accepts any type of data, in the example below a custom `struct` is sent to device id 1:
```cpp
// Define a custom data type
struct voltage_record { uint16_t v1; uint16_t v2; };
// Fill it with information
voltage_record record;
record.v1 = analogRead(A1);
record.v2 = analogRead(A2);
// Send to device id 1 the record struct
bus.send(1, &record, sizeof(record));
```

To use the return value of `send` just save it in a variable of type `uint16_t`:
```cpp
uint16_t packet = bus.send(100, "Ciao, this is a test!", 21);

if(packet == PJON_FAIL) Serial.print("Something went wrong");
```

If you need to transmit in shared mode or configure other protocol fields you can use `send` passing a struct of type [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info), the payload of type `const void *` and the length of type `uint16_t`:
```cpp
uint8_t bus_id[4] = {0, 0, 0, 1};
PJON_Packet_Info info;
info.rx.id = 10; // The recipient's device id is 10
info.rx.port = 8000; // The packet includes the port 8000
memcpy(info.rx.bus_id, bus_id, 4); // Copy recipient's bus id in info
bus.send(info, "Ciao!", 5);
```

### `send_repeatedly`

| Buffered | Blocking | Attempts                      |
| -------- | -------- | ----------------------------- |
| Yes      | No       | `strategy.get_max_attempts()` |

The `send_repeatedly` method can be used when it is required to schedule a repeated transmission. The first parameter is the device id of the recipient of type `uint8_t` then follows the payload of type `const void *`, its length of type `uint16_t` and the interval of type `uint32_t`:
```cpp
// Local sending example
uint16_t one_second_test =
  bus.send_repeatedly(100, "Test sent every second!", 23, 1000000);
/* IMPORTANT: maximum interval supported is
   4293014170 microseconds or 71.55 minutes */

```
`send_repeatedly` returns the index of the packet in the `packets` buffer as `send` does, to remove the repeated transmission simply call:
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
```
The `send_repeatedly` method accepts any type of data, in the example below a custom `struct` is sent to device id 1:
```cpp
// Define a custom data type
struct voltage_record { uint16_t v1; uint16_t v2; };
// Fill it with information
voltage_record record;
record.v1 = analogRead(A1);
record.v2 = analogRead(A2);
// Send to device id 1 the record struct
bus.send_repeatedly(1, &record, sizeof(record));
```

If you need to transmit in shared mode or configure other protocol fields you can use `send_repeatedly` passing a struct of type [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info), the payload of type `const void *`, the length of type `uint16_t` and the interval of type `uint32_t`:
```cpp
uint8_t bus_id[4] = {0, 0, 0, 1};
PJON_Packet_Info info;
info.rx.id = 10; // The recipient's device id is 10
info.rx.port = 8000; // The packet includes the port 8000
memcpy(info.rx.bus_id, bus_id, 4); // Copy recipient's bus id in info
bus.send_repeatedly(info, "Ciao!", 5, 1000000); // Send "Ciao!" every second
```

### `reply`

| Buffered | Blocking | Attempts                      |
| -------- | -------- | ----------------------------- |
| Yes      | No       | `strategy.get_max_attempts()` |

The `reply` method can be called within the [receiver function](/documentation/data-reception.md#data-reception) to reply to a packet received previously:
```cpp
bus.reply("All fine!", 9);
```
Consider that `reply` dispatches a packet in the buffer like `send` or `send_repeatedly`. To use the return value of `send` just save it in a variable of type `uint16_t`:
```cpp
uint16_t packet = bus.reply("Ciao, this is a test!", 21);
if(packet == PJON_FAIL) Serial.print("Something went wrong");
```
The `reply` method accepts any type of data, in the example below a custom struct is sent to device id 1:
```cpp
// Define a custom data type
struct voltage_record { uint16_t v1; uint16_t v2; };
// Fill it with information
voltage_record record;
record.v1 = analogRead(A1);
record.v2 = analogRead(A2);
// Reply with the record struct
bus.reply(&record, sizeof(record));
```

### `reply_blocking`

| Buffered | Blocking | Attempts                      |
| -------- | -------- | ----------------------------- |
| No       | Yes      | `strategy.get_max_attempts()` |

The `reply_blocking` method can be called within the [receiver function](/documentation/data-reception.md#data-reception) to reply to a packet received previously:
```cpp
bus.reply_blocking("All fine!", 9);
```
Consider that `reply_blocking` is a blocking procedure that in case of failure can last a considerable amount of time. The `reply_blocking` result, of type `uint16_t`, can be used to determine if the transmission occurred successfully or not:
```cpp
uint16_t result = bus.reply_blocking("All is ok!", 11);
if(result == PJON_ACK) Serial.println("Responded successfully!");
```
The `reply_blocking` method accepts any type of data, in the example below a custom `struct` is sent to device id 1:
```cpp
// Define a custom data type
struct voltage_record { uint16_t v1; uint16_t v2; };
// Fill it with information
voltage_record record;
record.v1 = analogRead(A1);
record.v2 = analogRead(A2);
// Reply with the record struct
bus.reply_blocking(&record, sizeof(record));
```
