- [Addressing](https://github.com/gioblu/PJON/tree/6.2/documentation/addressing.md)
- [Configuration](https://github.com/gioblu/PJON/tree/6.2/documentation/configuration.md)
- [Data reception](https://github.com/gioblu/PJON/tree/6.2/documentation/data-reception.md)
- [Data transmission](https://github.com/gioblu/PJON/tree/6.2/documentation/data-transmission.md)
- **[Error handling](https://github.com/gioblu/PJON/tree/6.2/documentation/error-handling.md)**
- [IO setup](https://github.com/gioblu/PJON/tree/6.2/documentation/io-setup.md)

PJON is designed to inform the user if an error is detected. A `void function` has to be defined as the error handler, it receives 2 parameters the first is the error code and the second is 1 byte additional data related to the error.

Error types:
- `PJON_CONNECTION_LOST` (value 101), `data` parameter contains lost device's id.
- `PJON_PACKETS_BUFFER_FULL` (value 102), `data` parameter contains buffer length.
- `PJON_CONTENT_TOO_LONG` (value 104), `data` parameter contains content length.

```cpp
void error_handler(uint8_t code, uint8_t data) {
  if(code == PJON_CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(data);
    Serial.println(" is lost.");
  }
  if(code == PJON_PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data, DEC);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher MAX_PACKETS in PJON.h if necessary.");
  }
  if(code == PJON_CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
}
```

Now inform the bus to call the error handler function in case of error:
```cpp
bus.set_error(error_handler);
```
