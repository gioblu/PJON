
####What is a Strategy?
PJON codebase uses strategies to physically communicate through the medium used, abstracting the physical layer from its procedure. A Strategy is only a class containing a set of functions able to send and receive messages:

```cpp
boolean can_start()
```
Should Return `true` if the medium is free for use and `false` if the medium is in use by some other device.

```cpp
void send_byte(uint8_t b)
```
Sends a byte on a pin

```cpp
uint16_t receive_byte()
```
Receives a byte from a pin

```cpp
void send_response(uint8_t response)
```
Send a response to the packet's transmitter

```cpp
uint16_t receive_response()
```
Receives a response from the packet's receiver

You can define your own set of 5 methods to use PJON with your personal strategy on the media you prefer. If you need other custom configuration or functions, those can be defined in your personal Strategy class. Other communication protocols could be used inside those methods to transmit data.

```cpp
// Simple Serial physical layer example
void send_byte(uint8_t b) {
  Serial.print(b);
}
```

####How to define a new strategy
To define your new strategy you have only to create a new folder named for example `YourStrategyName` in `strategies`
directory and write the necessary file `YourStrategyName.h`:

```cpp
class YourStrategyName {
  public:
    boolean can_start() { ... };
    uint16_t receive_byte() { ... };
    uint16_t receive_response() { ... };
    void send_byte(uint8_t b) { ... };
    void send_response(uint8_t response) { ... };
}
```

Simply add your code in the functions declaration shown above and instantiate PJON using the strategy type you
have created: `PJON<YourStrategyName> bus();`.
