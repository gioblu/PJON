
### Documentation index
- [Addressing](/documentation/addressing.md)
- [Configuration](/documentation/configuration.md)
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- **[Error handling](/documentation/error-handling.md)**
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

## Error handling
PJON is designed to inform the user if an error is detected. A function of type `void` can be defined and registered to be called by the PJON object in case of error; it receives 3 parameters, the first is the error code of type `uint8_t`, the second is additional data related to the error of type `uint16_t` and the third is a general purpose custom pointer of type `void *`.

Error types:
- `PJON_CONNECTION_LOST` (value 101), `data` parameter contains lost packet's index in the packets buffer.
- `PJON_PACKETS_BUFFER_FULL` (value 102), `data` parameter contains buffer length.
- `PJON_CONTENT_TOO_LONG` (value 104), `data` parameter contains content length.

```cpp
void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(bus.packets[data].content[0], DEC);
    Serial.println(" is lost.");
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher PJON_MAX_PACKETS if necessary.");
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
}
```

Now inform the instance to call `error_handler` in case of error:
```cpp
bus.set_error(error_handler);
```
