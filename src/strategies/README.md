
### What is a Strategy?
A strategy is an abstraction layer used to physically transmit data. Thanks to the strategies PJON can operate transparently on a wide range of media and protocols. Take a look at the [strategies video introduction](https://www.youtube.com/watch?v=yPu45xoAHGg) for a brief showcase of their features.

| Strategy      | Physical layer | Protocol | Pins needed   |
| ------------- | -------------- | -------- | ------------- |
| [SoftwareBitBang](SoftwareBitBang) | Electrical impulses over conductive element | [PJDL](SoftwareBitBang/specification/PJDL-specification-v2.0.md) | 1 or 2 |
| [AnalogSampling](AnalogSampling)  | Light pulses over air or optic fiber | [PJDLS](AnalogSampling/specification/PJDLS-specification-v2.0.md) | 1 or 2 |
| [EthernetTCP](EthernetTCP)  | Electrical/radio impulses over wire/air | TCP | Ethernet port |
| [LocalUDP](LocalUDP)  | Electrical/radio impulses over wire/air | UDP | Ethernet port |
| [GlobalUDP](GlobalUDP)  | Electrical/radio impulses over wire/air | UDP | Ethernet port |
| [DualUDP](DualUDP)  | Electrical/radio impulses over wire/air | UDP | Ethernet port |
| [OverSampling](OverSampling)  | Electrical/radio impulses over wire/air | [PJDLR](OverSampling/specification/PJDLR-specification-v2.0.md) | 1 or 2 |
| [ThroughSerial](ThroughSerial)  | Electrical/radio impulses over wire/air | [TSDL](ThroughSerial/specification/TSDL-specification-v2.0.md) | 1 or 2 |
| [ThroughSerialAsync](ThroughSerialAsync)  | Electrical/radio impulses over wire/air | [TSDL](ThroughSerial/specification/TSDL-specification-v2.0.md) | 1 or 2 |
| [ThroughLoRa](ThroughLoRa)  | Radio impulses over air | LoRa | 3 or 4 |
| [ESPNOW](ESPNOW)  | Radio impulses over air | [ESPNOW](https://www.espressif.com/en/products/software/esp-now/overview) | WiFi link |
| [Any](Any)  | Virtual inheritance, any of the above | Any of the above | Any of the above |

A `Strategy` is a class containing a set of methods used to physically send and receive data along with the required getters to handle retransmission and collision:

```cpp
bool begin(uint8_t additional_randomness = 0)
```
Receives an optional parameter of type `uint8_t` used for randomness and returns `true` if the strategy is correctly initialized.

```cpp
uint32_t back_off(uint8_t attempts)
```
receives a paramenter of type `uint8_t` and returns the suggested delay for a given number of attempts.

```cpp
bool can_start()
```
Returns `true` if the medium is free for use and `false` if the medium is busy.

```cpp
void handle_collision()
```
Handles a collision.

```cpp
uint8_t get_max_attempts()
```
Returns the maximum number of attempts in case of failed transmission.

```cpp
void send_string(uint8_t *string, uint16_t length)
```
Receives a pointer to a string and its length and sends it through the medium. The sending procedure must be blocking.

```cpp
uint16_t receive_string(uint8_t *string, uint16_t max_length) { ... };
```
Receives a pointer where to store received information and an unsigned integer signalling the maximum string length. It should return the number of bytes received or `PJON_FAIL`.

```cpp
void send_response(uint8_t response) { ... };
```
Send a response to the packet's transmitter.

```cpp
uint16_t receive_response() { ... };
```
Receives a response from the packet's receiver.

```cpp
// Simple Serial data link layer implementation example
void send_response(uint8_t response) {
  Serial.print(response);
};
```
Above it is demonstrated how simply other communication protocols can be used to define a new custom strategy.

### How to define a new strategy
To define the strategy it is required to create a new folder named for example `YourStrategyName` in the `strategies`
directory and write the necessary file `YourStrategyName.h`:

```cpp
class YourStrategyName {
  public:
    uint32_t back_off(uint8_t attempts) { ... };
    bool     begin(uint8_t additional_randomness) { ... };
    bool     can_start() { ... };
    uint8_t  get_max_attempts() { ... };
    uint16_t receive_string(uint8_t *string, uint16_t max_length) { ... };
    uint16_t receive_response() { ... };
    void     send_response(uint8_t response) { ... };
    void     send_string(uint8_t *string, uint16_t length) { ... };
};
```

The last thing to do is to add the inclusion of the new strategy in `PJON_Strategies.h`.
If all is correct it should be possible to instantiate PJON using the new strategy:

```cpp
PJON<YourStrategyName> bus(44);
// Use PJON as always
```

Strategy related methods required for configuration can be defined within the strategy. For example the `SoftwareBitBang` strategy uses a dedicated setter for the communication pin:
```cpp
bus.strategy.set_pin(12);
```
