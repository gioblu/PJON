
### Documentation index
- [Addressing](/documentation/addressing.md)
- **[Configuration](/documentation/configuration.md)**
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

---

## Configuration
PJON uses predefined constants, setters and getters to support features and constraints configuration.  

### Buffers configuration
Before instantiating PJON it is possible to define the length of its buffers. Predefining `PJON_MAX_PACKETS` and `PJON_PACKET_MAX_LENGTH` it is possible to configure these constraints to reach the project memory requirements. Obviously, the less memory is dedicated to buffers, the more memory can be used for something else:
```cpp  
  #define PJON_MAX_PACKETS 1
  #define PJON_PACKET_MAX_LENGTH 20
  #include <PJON.h>
  /* PJON can dispatch up to 1 packet with a payload of up to
     20 bytes - packet overhead (5-35 bytes depending on configuration) */
```

### Strategy configuration
PJON is instantiated passing a [strategy](/src/strategies/README.md) template parameter:
```cpp  
  PJON<SoftwareBitBang> bus;
```
In the example above the PJON object is instantiated passing the [SoftwareBitBang](/src/strategies/SoftwareBitBang/README.md) strategy. Strategies are classes that abstract the physical transmission of data. It is possible to instantiate more than one PJON object using different strategies in the same program:
```cpp  
  PJON<SoftwareBitBang> wiredBus;
  PJON<EthernetTCP>     tcpBus;
```

| Strategy      | Physical layer | Protocol | Inclusion constant | Included by default |
| ------------- | -------------- | -------- | ------------------ | ------------------- |
| [AnalogSampling](/src/strategies/AnalogSampling)  | Light | [PJDLS](../src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) | `PJON_INCLUDE_AS` | yes |
| [Any](/src/strategies/Any)  | Virtual inheritance | Any | `PJON_INCLUDE_ANY` | yes |
| [DualUDP](/src/strategies/DualUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) | `PJON_INCLUDE_DUDP` | yes |
| [ESPNOW](/src/strategies/ESPNOW)  | WiFi | [ESPNOW](https://www.espressif.com/en/products/software/esp-now/overview) | `PJON_INCLUDE_EN` | no |
| [EthernetTCP](/src/strategies/EthernetTCP)  | Ethernet/WiFi | [TCP](https://tools.ietf.org/html/rfc793) | `PJON_INCLUDE_ETCP` | yes |
| [GlobalUDP](/src/strategies/GlobalUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) | `PJON_INCLUDE_GUDP` | yes |
| [LocalFile](/src/strategies/LocalFile)  | System memory | None | `PJON_INCLUDE_LF` | no |
| [LocalUDP](/src/strategies/LocalUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) | `PJON_INCLUDE_LUDP` | yes |
| [MQTTTranslate](/src/strategies/MQTTTranslate)  | Ethernet/WiFi | [MQTT](http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.pdf) | `PJON_INCLUDE_MQTT` | no |
| [OverSampling](/src/strategies/OverSampling)  | Radio | [PJDLR](../src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md) | `PJON_INCLUDE_OS` | yes |
| [SoftwareBitBang](/src/strategies/SoftwareBitBang) | Wire | [PJDL](../src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.1.md) | `PJON_INCLUDE_SWBB` | yes |
| [ThroughLoRa](/src/strategies/ThroughLoRa)  | Radio | [LoRa](https://lora-alliance.org/sites/default/files/2018-07/lorawan1.0.3.pdf) | `PJON_INCLUDE_TL` | no |
| [ThroughSerial](/src/strategies/ThroughSerial)  | Radio | [TSDL](../src/strategies/ThroughSerial/specification/TSDL-specification-v3.0.md) | `PJON_INCLUDE_TS` | yes |

By default all strategies are included except `MQTTTranslate`, `LocalFile`, `ThroughLoRa` and `ESPNOW`. To reduce memory footprint add for example `#define PJON_INCLUDE_SWBB` before including the library to select only the `SoftwareBitBang` strategy. More than one strategy related constants can be defined in the same program if that is required.

When a strategy is included by default the usage is really simple:
```cpp
#include<PJON.h>           // Include the library
PJON<SoftwareBitBang> bus; // Instantiation
```
If the strategy you want to use is not included by default, like ESPNOW, you can force its inclusion using its constant:
```cpp
#define PJON_INCLUDE_EN    // Force the inclusion of ESPNOW strategy
#include<PJON.h>           // Include the library
PJON<SoftwareBitBang> bus; // Instantiation
```

Before using `ThroughLoRa` be sure to have [arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa) library available and to have defined the `PJON_INCLUDE_TL` constant before including `PJON.h`.

Before using `ESPNOW` be sure to have installed the required tools as described [here](/src/strategies/ESPNOW/README.md) and to have defined the `PJON_INCLUDE_EN` constant before including `PJON.h`.

Before using `MQTTTranslate` be sure to have the [ReconnectingMqttClient](https://github.com/fredilarsen/ReconnectingMqttClient) library available and to have defined the `PJON_INCLUDE_MQTT` constant before including `PJON.h`.

### Network mode
The network mode can be changed with `set_shared_network` during runtime, for example moving from [local](/specification/PJON-protocol-specification-v4.0.md#local-mode) to [shared](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v4.0.md#shared-mode) mode:
```cpp  
  bus.set_shared_network(true);
```

### Communication mode
The communication mode can be configured using the `set_communication_mode` passing `PJON_SIMPLEX` for simplex or mono-directional mode or `PJON_HALF_DUPLEX` for half-duplex or bidirectional mode:
```cpp  
  // Run in mono-directional PJON_SIMPLEX mode
  bus.set_communication_mode(PJON_SIMPLEX);
  // Run in bi-directional PJON_HALF_DUPLEX mode
  bus.set_communication_mode(PJON_HALF_DUPLEX);
```

### Router mode
When an instance is configured in router mode it is able to receive all incoming packets without any bus or device id filtering. Use `set_router` to configure the router mode:
```cpp  
  bus.set_router(true);
```

### Sender information
PJON by default includes the sender's information in the packet, If required `include_sender_info` can be used as shown below to avoid including sender's information:
```cpp  
  bus.include_sender_info(false);
```

### CRC configuration
With `set_crc_32` CRC32 can be forced on each packet sent to higher reliability:
```cpp  
  bus.set_crc_32(true);
```

### Packet handling
If manual packet handling is required, packet automatic deletion can be avoided using `set_packet_auto_deletion` as shown below:
```cpp  
  bus.set_packet_auto_deletion(false);
```

### Acknowledgement
With the acknowledgement enabled the transmitter has reception certainty. It is by default enabled but can be disabled:
```cpp  
  bus.set_acknowledge(false);
```

### Packet identification
The instance can be configured to include a 16 bits [packet identifier](/specification/PJON-protocol-specification-v4.0.md#packet-identification) to guarantee packet uniqueness. Define `PJON_INCLUDE_PACKET_ID` as described below, if this constant is not present the feature is not included and around 300 bytes of program memory and 80 bytes of RAM are spared:
```cpp  
  // Include the packet id feature
  #define PJON_INCLUDE_PACKET_ID

  // Max number of old packet ids stored to avoid duplication
  // If packet duplication occurs, higher PJON_MAX_RECENT_PACKET_IDS
  #define PJON_MAX_RECENT_PACKET_IDS 10  // By default 10

  #include <PJON.h>
```
Use `set_packet_id` to enable the packet identification:
```cpp  
  bus.set_packet_id(true);
```
See the [UsePacketId](/examples/ARDUINO/Local/SoftwareBitBang/UsePacketId) example to see more in detail how the packet id can be used.

### Network service identification
The instance can be configured to include a [network service identifier](/specification/PJON-protocol-specification-v4.0.md#network-services) in the packet. Ports from 0 to 8000 are reserved to known network services which index is present in the [known network services list](/specification/PJON-network-services-list.md), ports from 8001 to 65535 are free for custom use cases. Define `PJON_INCLUDE_PORT` as described below, if this constant is not present the feature is not used and around 100 bytes of program memory and 2 bytes of RAM are spared:
```cpp  
  // Include the port id feature
  #define PJON_INCLUDE_PORT
  #include <PJON.h>
```
Use `include_port` to enable the network service identification:
```cpp
  bus.include_port(false); // Avoid port inclusion (default)
  bus.include_port(8001);  // Include a port
```
When a port is configured, packets that contain a different port are discarded. See the [PortsUseExample](/examples/ARDUINO/Network/SoftwareBitBang/PortsUseExample) to see more in detail how the port feature can be used.

### Hardware identification
The instance can be configured to include the [hardware identification](/specification/PJON-protocol-specification-v4.0.md#hardware-identification). If the feature is used both recipient's and sender's MAC addresses are included in the packet. Define `PJON_INCLUDE_MAC` as described below, if this constant is not present the feature is not included and around 200 bytes of program memory and 20 bytes of RAM are spared:
```cpp  
  // Include the port id feature
  #define PJON_INCLUDE_MAC
  #include <PJON.h>

  // Device's MAC address
  uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
  PJON<SoftwareBitBang> bus(mac);
```
Use `include_mac` to enable the network service identification by default:
```cpp  
  bus.include_mac(true); // Include MAC address by default
```
See the [BlinkTestMAC](/examples/ARDUINO/Local/SoftwareBitBang/BlinkTestMAC) example to see more in detail how the MAC feature can be used.
