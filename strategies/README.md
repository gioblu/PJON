
#### What is a Strategy?
PJON codebase uses strategies to physically communicate through the medium used, abstracting the data link layer from its procedure. We propose 6 strategies to communicate data through various media, take a look at the [strategies video introduction](https://www.youtube.com/watch?v=yPu45xoAHGg) for a brief showcase of their features. A Strategy is a class containing the back-off configuration and a set of methods able to send and receive messages:

```cpp
bool begin(uint8_t additional_randomness = 0)
```
Returns `true` if the strategy is correctly initialized (receives a optional uint8_t used for randomness)

```cpp
uint32_t back_off(uint8_t attempts)
```
Returns the suggested delay related to the attempts passed as parameter

```cpp
bool can_start()
```
Returns `true` if the medium is free for use and `false` if the medium is in use by some other device

```cpp
void handle_collision()
```
Handles a collision

```cpp
uint8_t get_max_attempts()
```
Returns the maximum number of attempts in case of failed transmission

```cpp
void send_string(uint8_t *string, uint16_t length)
```
Sends a string of a certain length through the medium

```cpp
uint16_t receive_byte() { ... };
```
Receives a byte though the medium

```cpp
void send_response(uint8_t response) { ... };
```
Send a response to the packet's transmitter

```cpp
uint16_t receive_response() { ... };
```
Receives a response from the packet's receiver

You can define your own set of 5 methods to use PJON with your personal strategy on the media you prefer. If you need other custom configuration or functions, those can be defined in your personal Strategy class. Other communication protocols could be used inside those methods to transmit data.

```cpp
// Simple Serial data link layer implementation example
void send_response(uint8_t response) {
  Serial.print(response);
};
```

#### How to define a new strategy
To define your new strategy you have only to create a new folder named for example `YourStrategyName` in `strategies`
directory and write the necessary file `YourStrategyName.h`:

```cpp
class YourStrategyName {
  public:
    uint32_t back_off(uint8_t attempts) { ... };
    bool  begin(uint8_t additional_randomness) { ... };
    bool  can_start() { ... };
    uint8_t  get_max_attempts() { ... };
    uint16_t receive_byte() { ... };
    uint16_t receive_response() { ... };
    void     send_response(uint8_t response) { ... };
    void     send_string(uint8_t *string, uint16_t length) { ... };
};
```

Simply add your code in the functions declaration shown above and instantiate PJON using the strategy type you
have created: `PJON<YourStrategyName> bus();`.
