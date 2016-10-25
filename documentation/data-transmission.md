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
int one_second_test = bus.send_repeatedly(100, "Test sent every second!", 23, 1000000);
```

The `one_second_test` variable contains the id of the packet, to remove this repeated task simply:
```cpp
bus.remove(one_second_test);
```

To broadcast a message to all connected devices, use the `BROADCAST` constant as recipient ID.
```cpp
int broadcastTest = bus.send(BROADCAST, "Message for all connected devices.", 34);
```

If you need to send a packet in a blocking manner use `send_packet_blocking` method, instead of adding the packet to the queue like `send` does, `send_packet_blocking` executes the transmission and the backoff retry if necessary exactly how would have been executed by the `send` plus `update` chain while returning the effective result of the transmission. Remember that `send_packet_blocking` does not try to receive while transmitting or retrying, so this should be used carefully.
```cpp
if(bus.send_packet_blocking(10, "All is ok?!", 11) == ACK) {
  Serial.println("10 is ok!");
}  
```
