
### What is a Strategy?
A strategy is an abstraction layer used to physically transmit data. Thanks to the strategies PJON can operate transparently on a wide range of media and protocols. Take a look at the [strategies video introduction](https://www.youtube.com/watch?v=yPu45xoAHGg) for a brief showcase of their features.

| Strategy      | Physical layer | Protocol |
| ------------- | -------------- | -------- |
| [SoftwareBitBang](/src/strategies/SoftwareBitBang) | Wire | [PJDL](../src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.1.md) |
| [OverSampling](/src/strategies/OverSampling)  | Radio | [PJDLR](../src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md) |
| [AnalogSampling](/src/strategies/AnalogSampling)  | Light | [PJDLS](../src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) |
| [ThroughSerial](/src/strategies/ThroughSerial)  | Radio | [TSDL](../src/strategies/ThroughSerial/specification/TSDL-specification-v3.0.md) |
| [ThroughLoRa](/src/strategies/ThroughLoRa)  | Radio | [LoRa](https://lora-alliance.org/sites/default/files/2018-07/lorawan1.0.3.pdf) |
| [EthernetTCP](/src/strategies/EthernetTCP)  | Ethernet/WiFi | [TCP](https://tools.ietf.org/html/rfc793) |
| [LocalUDP](/src/strategies/LocalUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) |
| [GlobalUDP](/src/strategies/GlobalUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) |
| [DualUDP](/src/strategies/DualUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) |
| [MQTTTranslate](/src/strategies/MQTTTranslate)  | Ethernet/WiFi | [MQTT](http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.pdf) |
| [ESPNOW](/src/strategies/ESPNOW)  | WiFi | [ESPNOW](https://www.espressif.com/en/products/software/esp-now/overview) |
| [LocalFile](/src/strategies/LocalFile)  | System memory | None |
| [Any](/src/strategies/Any)  | Virtual inheritance, any of the above | Any |

A `Strategy` is a class containing a set of methods used to physically send and receive data along with the required getters to handle retransmission and collision:

```cpp
bool begin(uint8_t did = 0)
```
Receives an optional parameter of type `uint8_t` (when PJON calls `begin` it passes its own device id); returns `true` if the strategy is correctly initialized. There is no doubt that the strategy should not know about the PJON's device id, although that is practically useful in many cases.

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
void send_frame(uint8_t *data, uint16_t length)
```
Receives a pointer to the data and its length and sends it through the medium. The sending procedure must be blocking.

```cpp
uint16_t receive_frame(uint8_t *data, uint16_t max_length) { ... };
```
Receives a pointer where to store received information and an unsigned integer signalling the maximum data length. It should return the number of bytes received or `PJON_FAIL`.

```cpp
void send_response(uint8_t response)
```
Send a response to the packet's transmitter.

```cpp
uint16_t receive_response()
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
    uint32_t back_off(uint8_t attempts) { };
    bool     begin(uint8_t did) { };
    bool     can_start() { };
    uint8_t  get_max_attempts() { };
    uint16_t receive_frame(uint8_t *data, uint16_t max_length) { };
    uint16_t receive_response() { };
    void     send_response(uint8_t response) { };
    void     send_frame(uint8_t *data, uint16_t length) { };
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
