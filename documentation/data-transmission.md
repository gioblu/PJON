- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- **[Data transmission](/documentation/data-transmission.md)**
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)

The begin function has to be called in the setup or in the loop. The lack of this method call can lead to collision problems on startup, so be sure to call this function before starting transmitting or receiving.
```cpp  
  bus.begin();
```

Data transmission is handled by a packet manager, call the `update()` function once per loop cycle:
```cpp  
  bus.update();
```

To send a string to another device connected to the bus simply call `send()` function passing the id you want to contact, the string you want to send and its length:
```cpp
bus.send(100, "Ciao, this is a test!", 21);
```

I know that the packet length is boring to fill but is there to prevent buffer overflow. If sending arbitrary values `NULL` terminator strategy based on `strlen()` is not safe to detect the end of a string.

To send a value repeatedly simply call `send_repeatedly()` and add as last parameter the interval in microseconds you want between every sending:
```cpp
uint16_t one_second_test = bus.send_repeatedly(100, "Test sent every second!", 23, 1000000);
```
`send_repeatedly` returns the id of the packet in the packet's buffer, to remove this repeated task simply:
```cpp
bus.remove(one_second_test);
```

To broadcast a message to all connected devices, use the `PJON_BROADCAST` constant as recipient ID.
```cpp
bus.send(PJON_BROADCAST, "Message for all connected devices.", 34);
```

If you are using PJON in shared mode (so in a medium shared by n buses) you can send to a precise bus or group of devices:
```cpp
uint8_t bus_id[] = {127, 0, 0, 1};
bus.send(12, bus_id, "Ciao!", 5);
```

If you need to send a packet in a blocking manner use `send_packet` method to try to send it once.
```cpp
if(bus.send_packet(10, "All is ok?!", 11) == PJON_ACK) { // Try once
  Serial.println("10 is ok!");
}  
```
The sending is executed as soon as the method is called and it returns the following values:
- `PJON_ACK` (6) if a correct reception occurred
- `PJON_NAK` (21) if a mistake is found in CRC
- `PJON_BUSY` (666) if a transmission for other devices is occurring
- `PJON_FAIL` (65535) if no data is received

Use `send_packet_blocking` if it is necessary to try more than once and so comply with the proposed back-off exactly how is done by the `send` and `update` chain.
```cpp
if(bus.send_packet_blocking(10, "All is ok?!", 11) == PJON_ACK) { // Try with back-off
  Serial.println("10 is ok!");
}    
```
It also returns the result of transmission exactly as `send_packet`. Remember that `send_packet_blocking` does not try to receive while transmitting or retrying, so this should be used carefully.
