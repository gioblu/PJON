## MQTTTranslate

| Medium | Pins used | Inclusion Constant |
|--------|-----------|--------------------|
| MQTT protocol   | NA    | `#include <PJONMQTTTranslate.h>`|

MQTTTranslate uses the [ReconnectingMqttClient](https://github.com/fredilarsen/ReconnectingMqttClient) library (minimum version required v1.1.1) to deliver PJON packets over TCP on local network (LAN) as a MQTT protocol client. It may be useful to connect PJON networks and more standard applications to each other using the MQTT protocol. This strategy works in one of four modes. The first two modes enable to implement a PJON bus via MQTT, the first mode is "closed" and the second is "open" to use by non-PJON programs. The last two modes are for behaving like MQTT devices normally do.

1. `MQTTT_MODE_BUS_RAW` mode sends binary JSON packets delivered to a topic like `pjon/device45` (where `45` is a receiver device id). Each device subscribes to a topic with its own name and receives packets like any other PJON strategy. This mode requires that all senders and receivers are linked with PJON for encoding/decoding, so other systems are not easily connected. The directory [examples/WINX86/Local/MQTTTranslate/PingPong](../../../examples/WINX86/Local/MQTTTranslate/PingPong) contains examples for windows, to build it, open the solution file in Visual Studio 2017. The directory [examples/ARDUINO/Local/MQTTTranslate/PingPong](../../../examples/ARDUINO/Local/MQTTTranslate/PingPong) contains the Arduino examples, to build them, just use the Arduino IDE.

2. `MQTTT_MODE_BUS_JSON` mode sends JSON packets with to, from and data, delivered to a topic like `pjon/device45` (where `45` is a receiver device id). Each device subscribes to a topic with its own name and receives packets like `{to:45,from:44,data:"message text sent from device 44 to device 45"}`.

3. `MQTTT_MODE_MIRROR_TRANSLATE` mode does not not use JSON encapsulation of values, and publishes to its own topic, not the receiver's. It publishes to an "output" folder and subscribes to an "input" folder. An outgoing packet with payload `P=44.1,T=22.0` results in the topics `pjon/device44/output/temperature`, with a value `44.1` and `pjon/device44/output/pressure`, with a value `22.0`. Likewise, when receiving an update of `pjon/device44/input/setpoint`, with a value `45` results in a packet with payload `S=45`. This mode supports a translation table to allow short names to be used in packets while topic names are longer. For example `T` translated in `temperature`. If no translation table is populated, the same names will be used in the packets and the topics. The directory [examples/ESP8266/Local/MQTTTranslate/EnvironmentController](../../../examples/ESP8266/Local/MQTTTranslate/EnvironmentController) contains the ESP8266 example, to build it, just use the Arduino IDE.

4. `MQTTT_MODE_MIRROR_DIRECT` mode works like `MQTTT_MODE_MIRROR_TRANSLATE`, but just passes the payload on without any translation, leaving the formatting to the user. It does not split packets into separate topics but transfers the packets as-is to one output topic and from one input topic `pjon/device44/output`, `pjon/device44/input`. The user sketch will have control of the format used, which can be plain text like `P=44.1,T=22.0` or a JSON text. The directory [examples/ARDUINO/Local/MQTTTranslate/SWBB-MQTT-Gateway](../../../examples/ARDUINO/Local/MQTTTranslate/SWBB-MQTT-Gateway) contains the Arduino SWBB-MQTT-Gateway example, that showcases bidirectional, transparent data transmission between an MQTT client and a [SoftwareBitBang](../SoftwareBitBang/README.md) bus. To build it, just use the Arduino IDE.

The "Translate" in the strategy name is because a translation table can be used to translate PJON packet contents to MQTT topics and back. This is to enable PJON packets to remain small `t=44.3` between devices with limited memory, while the MQTT packets are made more explicit `temperature` to support longer name syntax in external systems.

#### MAC address usage

The topic names like `pjon/device45/output/temperature` in the two MIRROR modes can be replaced with topic names containing the MAC address of the Ethernet/WiFi card of the device, like `pjon/DACA7EEFFE5D/output/temperature`. This is selected by setting the `MQTTT_USE_MAC` preprocessor definition.

This gives the option to flash the same sketch without modifications to a lot of devices that will all appear in dedicated topics, to enable plug and play.

Note that this functionality does not cover Windows/Linux/OsX in this release.

### Configuration

Before including the library it is possible to configure `MQTTTranslate` using predefined constants:

| Constant           | Purpose                                      | Supported value                                                                                        |
| ------------------ |--------------------------------------------- | ------------------------------------------------------------------------------------------------------ |
| `MQTTT_MODE`       | Select mode                                  | `MQTTT_MODE_BUS_RAW`, `MQTTT_MODE_BUS_JSON`, `MQTTT_MODE_MIRROR_TRANSLATE`, `MQTTT_MODE_MIRROR_DIRECT` |

Use `PJONMQTTTranslate` to instantiate an object ready to communicate using `MQTTTranslate` strategy:

```cpp  
  #include <PJONMQTTTranslate.h> // Include the PJON library
  // Use MQTTTranslate strategy with PJON device id 44
  PJONMQTTTranslate bus(44);
  uint8_t broker_ip[] = { 127, 0, 0, 1 };

  void setup() {  
    // Sets the broker's ip, port and topic used
    bus.strategy.set_address(broker_ip, 1883, "receiver");
  }
```
