
####What is a Strategy?
PJON codebase uses strategies to physically communicate through the medium used, abstracting the physical layer from its procedure. A Strategy is only a class containing a set of functions able to send and receive messages:

- `can_start()`
  Returns true if the medium is free and false if the medium is in use.

- `send_byte(uint8_t b, uint8_t pin)`
  Sends a byte on a pin

- `receive_byte(uint8_t pin)`
  Receives a byte from a pin

- `get_response()`
  Receives acknowledge of correct packet sending

You can define your own set of those 4 functions to use PJON with your personal
strategy on the media you prefer. If you need other functions, variables or
constants, those can be defined in your personal Strategy class. Other
communication protocols could be used inside those functions to transmit data.

```cpp
// Simple Serial physical layer example
static inline __attribute__((always_inline))
void send_byte(uint8_t b, uint8_t pin) {
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
    uint16_t receive_byte(uint8_t pin) { ... };
    void send_byte(uint8_t b, uint8_t pin) { ... };
    uint16_t get_response(uint8_t pin) { ... };
}
```

Simply add your code in the function declaration shown above and instantiate PJON using the strategy type you
have created: `PJON<YourStrategyName> bus();`.
