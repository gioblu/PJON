## ThroughLoRa

| Medium | Pins used | Inclusion |
|--------|-----------|--------------------|
| LoRa radio | 6     | `#include <PJONThroughLora.h>`|

With `ThroughLora` strategy, PJON can run through a software or hardware SPI in order to communicate with supported LoRa modules. See [Supported Shields/Modules](https://github.com/gioblu/PJON/tree/master/src/strategies/ThroughLoRa#supported-shieldsmodules).

This strategy is a wrapper around  [Arduino LoRa library](https://github.com/sandeepmistry/arduino-LoRa) created by [Sandeep Mistry](https://github.com/sandeepmistry) so all the credit to the specific LoRa implementation goes to him.

### Compatibility
- ATmega88/168/328 16MHz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16MHz (Arduino Mega)
- ATmega16u4/32u4 16MHz (Arduino Leonardo)
- STM32F103 ([Blue Pill](http://wiki.stm32duino.com/index.php?title=Blue_Pill))
- ESP32-based boards

### Getting started
1. Pass the `ThroughLora` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
2. Configure the shield/module pins according to the correct connection. See [Hardware connection](#hardware-connection).
3. Initialize the module with its specified frequency.

```cpp

#include <PJONThroughLora.h>

PJONThroughLora bus; //PJON Lora bus instance

bus.strategy.setPins(10,9,2); //CS pin, Reset pin, Interrupt pin
bus.strategy.setFrequency(868100000UL); //initialize 868 MHZ module
```

### Supported Shields/Modules
| Manufacturer                          | Hardware                                                     |
| ------------------------------------- | ------------------------------------------------------------ |
| [Dragino](http://www.dragino.com/)    | [Dragino Lora Shield](http://www.dragino.com/products/module/item/102-lora-shield.html) |
| [HopeRF](http://www.hoperf.com/)      | [RFM95W](http://www.hoperf.com/rf_transceiver/lora/RFM95W.html) / [RFM96W](http://www.hoperf.com/rf_transceiver/lora/RFM96W.html) / [RFM98W](http://www.hoperf.com/rf_transceiver/lora/RFM98W.html) |
| [Modtronix](http://modtronix.com/)    | [inAir4](http://modtronix.com/inair4.html) / [inAir9](http://modtronix.com/inair9.html) / [inAir9B](http://modtronix.com/inair9b.html) |
| [Adafruit](https://www.adafruit.com/) | [Adafruit Feather 32u4 LoRa Radio (RFM9x)](https://learn.adafruit.com/adafruit-feather-32u4-radio-with-lora-radio-module/overview) |
| [Ai-Thinker](https://en.ai-thinker.com/index.html) | Ra01 / [Ra02](https://en.ai-thinker.com/pro_view-57.html) (SX1278 based)

See the [Arduino LoRa readme](https://github.com/sandeepmistry/arduino-LoRa) for a more complete list of modules.

### Hardware connection
| General Wiring | Arduino |
| -------------- | ------- |
| VCC            | 3.3V    |
| GND            | GND     |
| SCK            | SCK     |
| MISO           | MISO    |
| MOSI           | MOSI    |
| NSS            | 10      |
| NRESET         | 9       |
| DIO0           | 2       |

- `NSS`, `NRESET`, and `DIO0` pins can be changed by using `PJON.strategy.setPins(ss, reset, dio0)`.
- `DIO0` pin is optional, it is only needed for receive callback mode. If `DIO0` pin is used, it **must** be interrupt capable via [`attachInterrupt(...)`](https://www.arduino.cc/en/Reference/AttachInterrupt).
- Some boards support custom pins for `SCK`, `MISO` and `MOSI` (ESP32 series for example). These can be set using `SPI.begin(sck, miso, mosi)` before calling `PJON.strategy.setFrequency(frequency)`.

### Usage Example
Here are listed basic examples of a transmitter and receiver code. After you include the necessary code to initialize the Lora module you can use the normal PJON functions to handle data communication.

Keep in mind that to use the LoRa strategy you must download the [Arduino LoRa library](https://github.com/sandeepmistry/arduino-LoRa).

More examples can be found in https://github.com/gioblu/PJON/tree/master/examples/ARDUINO/Local/ThroughLoRa

### Transmitter
```cpp

#include <PJONThroughLora.h>

PJONThroughLora bus(45);

void setup() {
  // Obligatory to initialize Radio with correct frequency
  bus.strategy.setFrequency(868100000UL);
  bus.begin();
  // Send B to device 44 every second
  bus.send_repeatedly(44, "B", 1, 1000000);
};

void loop() {
  bus.update();
};
```

### Receiver

```cpp
#include <PJONThroughLora.h>

// <Strategy name> bus(selected device id)
PJONThroughLora bus(44);

void setup() {
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW); // Initialize LED 13 to be off
  Serial.begin(9600);

  // Obligatory to initialize Radio with correct frequency
  bus.strategy.setFrequency(868100000UL);
  // Optional
  bus.strategy.setSignalBandwidth(250E3);
  bus.begin();
  bus.set_receiver(receiver_function);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
 if (payload[0] == 'B') {
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
};

void loop() {
  bus.receive(1000);
};
```

This API is a specific wrapper around the [Arduino-LoRa library](https://github.com/sandeepmistry/arduino-LoRa)

All the LoRa API is accessible through the exposed strategy in a PJON instance. Ex: `bus.strategy`

### Setup

Initialize the library with the specified frequency.

```cpp
bus.strategy.setFrequency(frequency);
```

- `frequency` - frequency in Hz (`433E6`, `866E6`, `915E6`)

Returns `1` on success, `0` on failure.

### Set pins

Override the default `NSS`, `NRESET`, and `DIO0` pins used by the library. **Must** be called before `LoRa.begin()`.

```cpp
bus.strategy.setPins(ss, reset, dio0);
```

- `ss` - new slave select pin to use, defaults to `10`
- `reset` - new reset pin to use, defaults to `9`
- `dio0` - new DIO0 pin to use, defaults to `2`, **must** be interrupt capable via [attachInterrupt(...)](https://www.arduino.cc/en/Reference/AttachInterrupt).

This call is optional and only needs to be used if you need to change the default pins used.

### Configuration

```cpp
int rssi = bus.strategy.packetRssi();
```
Returns the RSSI of the received packet.

```cpp
float snr = bus.strategy.packetSnr();
```
Returns the estimated SNR of the received packet in dB.

```cpp
bus.strategy.setSignalBandwidth(signalBandwidth);
```
Change the signal bandwidth of the radio, `signalBandwidth` represents the signal bandwidth in Hz, defaults to `125E3`. Supported values are `7.8E3`, `10.4E3`, `15.6E3`, `20.8E3`, `31.25E3`, `41.7E3`, `62.5E3`, `125E3`, and `250E3`.

```cpp
bus.strategy.setSpreadingFactor(spreadingFactor);
```
Change the spreading factor of the radio. `spreadingFactor` represents the spreading factor, defaults to `7`. Supported values are between `6` and `12`. If a spreading factor of `6` is set, implicit header mode must be used to transmit and receive packets.

```cpp
bus.strategy.setCodingRate4(codingRateDenominator);
```
Change the coding rate of the radio. `codingRateDenominator` represents denominator of the coding rate, defaults to `5`. Supported values are between `5` and `8`, these correspond to coding rates of `4/5` and `4/8`. The coding rate numerator is fixed at `4`.

```cpp
bus.strategy.setPreambleLength(preambleLength);
```
Change the preamble length of the radio. `preambleLength` represents preamble length in symbols, defaults to `8`. Supported values are between `6` and `65535`.

```cpp
bus.strategy.setSyncWord(syncWord);
```
Change the sync word of the radio. `syncWord` represents byte value to use as the sync word, defaults to `0x34`

```cpp
bus.strategy.setCRC(false); //To disable CRC
bus.strategy.setCRC(true); // To enable CRC
```
Enable or disable CRC usage, by default a CRC is not used.

```cpp
LoRa.setTxPower(txPower); //Configure the radio TX power

LoRa.setTxPower(txPower, outputPin); //Configure the radio TX power with extra boost pin
```
Change the TX power of the radio.

- `txPower` - TX power in dB, defaults to `17`
- `outputPin` - (optional) PA output pin, supported values are `PA_OUTPUT_RFO_PIN` and `PA_OUTPUT_PA_BOOST_PIN`, defaults to `PA_OUTPUT_PA_BOOST_PIN`.

Supported values are between `2` and `17` for `PA_OUTPUT_PA_BOOST_PIN`, `0` and `14` for `PA_OUTPUT_RFO_PIN`.

Most modules have the PA output pin connected to PA BOOST.

```cpp
bus.strategy.idle();
```
Put the radio in idle (standby) mode.

```cpp
bus.strategy.sleep();
```
Put the radio in sleep mode.

```cpp
byte b = bus.strategy.getRandom();
```
Generate a random byte, based on the Wideband RSSI measurement.

### Acknowledgement
Acknowledgement allows the sendor to request that the receiver acknowledge reception of a message so that multiple transmission attempts can be made if required. Limits on the allowed transmission duty cycle in various parts of the world, power budgets, message importance and network throughput may affect whether acknowledgement is required for some or all messages. Acknowledgement can be enabled or disabled for the sender using:
```cpp
bus.set_acknowledge(true/false);
```
Depending on the speed of the hardware on either end and the bandwidth and spreading factor configured, it may be necessary to increase the time the sender will wait for an acknowledgement before timing out. This can be set by defining `TL_RESPONSE_TIME_OUT` prior to including the header file.
```cpp
#define TL_RESPONSE_TIME_OUT 500000 // Timeout in us. Default is 100000
#include <PJONThroughLora.h>
```

In some edge cases the sender may be significantly slower than the receiver at switching between transmit and receive modes. This could result in the receiver sending a response before the sender has a chance to listen for it. A delay can be added between receiving a packet and acknowledging it by defining `TL_RESPONSE_DELAY` prior to including the header file. Keep this as short as possible.
```cpp
#define TL_RESPONSE_DELAY 30 // Delay between receiving a packet and sending a response in ms. (default 0)
#define TL_RESPONSE_TIME_OUT 500000 // Timeout in us. Default is 100000
#include <PJONThroughLora.h>
```
### Safety warning
In all cases, when installing or maintaining a PJON network, extreme care must be taken to avoid any danger. Before any practical test or hardware purchase for a wireless [ThroughLoRa](/src/strategies/ThroughLoRa) radio setup, compliance with government requirements and regulations must be ensured.
