
### Documentation index
- [Addressing](/documentation/addressing.md)
   - [`set_id`](/documentation/addressing.md#local-mode) [`device_id`](/documentation/addressing.md#local-mode) [`get_bus_id`](/documentation/addressing.md#shared-mode) [`set_bus_id`](/documentation/addressing.md#shared-mode) [`get_mac`](/documentation/addressing.md#get-or-set-hardware-identifier) [`set_mac`](/documentation/addressing.md#get-or-set-hardware-identifier)
- **[Configuration](/documentation/configuration.md)**
  - [`set_communication_mode`](/documentation/configuration.md#communication-mode) [`set_shared_network`](/documentation/configuration.md#network-mode) [`set_router`](/documentation/configuration.md#router-mode) [`include_sender_info`](/documentation/configuration.md#sender-information) [`set_crc_32`](/documentation/configuration.md#crc-configuration) [`set_packet_auto_deletion`](/documentation/configuration.md#packet-handling) [`set_acknowledge`](/documentation/configuration.md#acknowledgement) [`set_packet_id`](/documentation/configuration.md#packet-identification) [`include_port`](/documentation/configuration.md#network-service-identification) [`include_mac`](/documentation/configuration.md#hardware-identification)
- [Data reception](/documentation/data-reception.md)
  - [`set_receiver`](/documentation/data-reception.md#data-reception) [`receive`](/documentation/data-reception.md#data-reception)
- [Data structures](/documentation/data-structures.md)
  - [`PJON_Endpoint`](/documentation/data-structures.md#pjon_endpoint) [`PJON_Packet_Info`](/documentation/data-structures.md#pjon_packet_info)
- [Data transmission](/documentation/data-transmission.md)
  - [`begin`](/documentation/data-transmission.md#begin) [`reply`](/documentation/data-transmission.md#reply) [`reply_blocking`](/documentation/data-transmission.md#reply_blocking) [`send`](/documentation/data-transmission.md#send) [`send_packet`](/documentation/data-transmission.md#send_packet) [`send_packet_blocking`](/documentation/data-transmission.md#send_packet_blocking) [`send_repeatedly`](/documentation/data-transmission.md#send_repeatedly) [`reply`](/documentation/data-transmission.md#reply) [`reply_blocking`](/documentation/data-transmission.md#reply_blocking) [`forward`](/documentation/data-transmission.md#forward) [`forward_blocking`](/documentation/data-transmission.md#forward_blocking)
- [Error handling](/documentation/error-handling.md)
  - [`set_error`](/documentation/error-handling.md#error-handling)
- [Routing](/documentation/routing.md)
  - [`PJONSimpleSwitch`](/documentation/routing.md#simpleswitch) [`PJONSwitch`](/documentation/routing.md#switch) [`PJONRouter`](/documentation/routing.md#router) [`PJONDynamicRouter`](/documentation/routing.md#dynamicrouter) [`PJONInteractiveRouter`](/documentation/routing.md#interactiverouter)
- [IO setup](/documentation/io-setup.md)
   - [`strategy`](/documentation/io-setup.md#io-setup)
---

## Configuration
PJON uses predefined constants, setters and getters to support features and constraints configuration.  

### Buffers configuration
Before including the library, it is possible to define the length of its buffers by defining the `PJON_MAX_PACKETS` and `PJON_PACKET_MAX_LENGTH` constants:
```cpp  
  #define PJON_MAX_PACKETS 1
  #define PJON_PACKET_MAX_LENGTH 20
  /* PJON can dispatch up to 1 packet with a payload of up to
     20 bytes - packet overhead (5-35 bytes depending on configuration) */
```

### Strategy configuration
Strategies are classes that abstract the physical transmission of data. `PJON` uses [strategies](/src/strategies/README.md) as template parameters although since version 13.0 that complexity is hidden behind a [macro](../src/PJONSoftwareBitBang.h):
```cpp
  #include <PJONSoftwareBitBang.h>
  PJONSoftwareBitBang bus;
```
In the example above, the PJON object is instantiated using the [SoftwareBitBang](/src/strategies/SoftwareBitBang/README.md) strategy. It is possible to instantiate more than one PJON object using different strategies in the same program:
```cpp
#include <PJONSoftwareBitBang.h>
#include <PJONEthernetTCP.h>

PJONSoftwareBitBang wiredBus;
PJONEthernetTCP     tcpBus;
```

The table below lists the strategies available:

| Strategy      | Physical layer | Protocol | Inclusion |
| ------------- | -------------- | -------- | --------- |
| [AnalogSampling](/src/strategies/AnalogSampling)  | Light | [PJDLS](../src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) | `#include <PJONAnalogSampling.h>` |
| [Any](/src/strategies/Any)  | Virtual inheritance | Any | `#include <PJONAny.h>` |
| [DualUDP](/src/strategies/DualUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) | `#include <PJONDualUDP.h>` |
| [ESPNOW](/src/strategies/ESPNOW)  | WiFi | [ESPNOW](https://www.espressif.com/en/products/software/esp-now/overview) | `#include <PJONESPNOW.h>` |
| [EthernetTCP](/src/strategies/EthernetTCP)  | Ethernet/WiFi | [TCP](https://tools.ietf.org/html/rfc793) | `#include <PJONEthernetTCP.h>` |
| [GlobalUDP](/src/strategies/GlobalUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) | `#include <PJONGlobalUDP.h>` |
| [LocalFile](/src/strategies/LocalFile)  | File system | None | `#include <PJONLocalFile.h>` |
| [LocalUDP](/src/strategies/LocalUDP)  | Ethernet/WiFi | [UDP](https://tools.ietf.org/html/rfc768) | `#include <PJONLocalUDP.h>` |
| [MQTTTranslate](/src/strategies/MQTTTranslate)  | Ethernet/WiFi | [MQTT](http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.pdf) | `#include <PJONMQTTTranslate.h>` |
| [OverSampling](/src/strategies/OverSampling)  | Radio | [PJDLR](../src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md) | `#include <PJONOverSampling.h>` |
| [SoftwareBitBang](/src/strategies/SoftwareBitBang) | Wire | [PJDL](../src/strategies/SoftwareBitBang/specification/PJDL-specification-v5.0.md) | `#include <PJONSoftwareBitBang.h>` |
| [ThroughLoRa](/src/strategies/ThroughLoRa)  | Radio | [LoRa](https://lora-alliance.org/sites/default/files/2018-07/lorawan1.0.3.pdf) | `#include <PJONThroughLora.h>` |
| [ThroughSerial](/src/strategies/ThroughSerial)  | Wire | [TSDL](../src/strategies/ThroughSerial/specification/TSDL-specification-v3.0.md) | `#include <PJONThroughSerial.h>` |

Before using `ThroughLoRa` be sure to have [arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa) library available. Before using `ESPNOW` be sure to have installed the required tools as described [here](/src/strategies/ESPNOW/README.md). Before using `MQTTTranslate` be sure to have the [ReconnectingMqttClient](https://github.com/fredilarsen/ReconnectingMqttClient) library available.

### Random seed
When `begin` is called an analog pin is used to sample the seed for the random generator. By default PJON uses pin `A0` to sample the seed, if you need to use another pin call `set_random_seed` as shown below:
```cpp
  bus.set_random_seed(A1); // A1 is used to sample the seed
  bus.begin();             // Seed sampling occurs
```

### Network mode
The network mode can be changed with `set_shared_network` during runtime, for example moving from [local](/specification/PJON-protocol-specification-v4.0.md#local-mode) to [shared](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v4.0.md#shared-mode) mode:
```cpp  
  bus.set_shared_network(true);
```

### Communication mode
The communication mode can be configured using `set_communication_mode` passing `PJON_SIMPLEX` for simplex or mono-directional mode or `PJON_HALF_DUPLEX` for half-duplex or bidirectional mode:
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

  #include <PJONSoftwareBitBang.h>
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
  #include <PJONSoftwareBitBang.h>
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
  #include <PJONSoftwareBitBang.h>

  // Device's MAC address
  uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
  PJONSoftwareBitBang bus(mac);
```
Use `include_mac` to enable the network service identification by default:
```cpp  
  bus.include_mac(true); // Include MAC address by default
```
See the [BlinkTestMAC](/examples/ARDUINO/Local/SoftwareBitBang/BlinkTestMAC) example to see more in detail how the MAC feature can be used.
