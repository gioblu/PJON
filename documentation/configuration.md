
### Documentation index
- [Addressing](/documentation/addressing.md)
- **[Configuration](/documentation/configuration.md)**
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

## Configuration
PJON uses predefined constants, setters and getters to support features and constraints configuration.  

### Buffers configuration
Before instantiating PJON it is possible to define the length of its buffers. Predefining `PJON_MAX_PACKETS` and `PJON_PACKET_MAX_LENGTH` it is possible to configure this constraints to reach the project memory requirements. Obviously, the less memory is dedicated to buffers, the more memory can be used for something else:
```cpp  
#define PJON_MAX_PACKETS 1
#define PJON_PACKET_MAX_LENGTH 20
#include <PJON.h>
/* PJON can store up to 1 packet of up to
   20 characters - packet overhead
   (from 5 to 22 bytes depending by configuration) */
```

### Data link configuration
PJON is instantiated passing a [strategy](/src/strategies/README.md) template parameter:
```cpp  
  PJON<SoftwareBitBang> bus;
```
In the example above the PJON object is instantiated passing [SoftwareBitBang](/src/strategies/SoftwareBitBang/README.md) strategy. Strategies are classes abstracting the data link layer, making PJON easy to be used on different media. It is possible to instantiate more than one PJON object using different strategies in the same program:
```cpp  
  PJON<SoftwareBitBang> wiredBus;
  PJON<EthernetTCP>     tcpBus;
```

| Strategy      | Physical layer | Protocol | Pins needed   |
| ------------- | -------------- | -------- | ------------- |
| [SoftwareBitBang](/src/strategies/SoftwareBitBang) | Electrical impulses over conductive element | [PJDL](../src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) | 1 or 2 |
| [AnalogSampling](/src/strategies/AnalogSampling)  | Light pulses over air or optic fiber | [PJDLS](../src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) | 1 or 2 |
| [EthernetTCP](/src/strategies/EthernetTCP)  | Electrical/radio impulses over wire/air | TCP | Ethernet port |
| [LocalUDP](/src/strategies/LocalUDP)  | Electrical/radio impulses over wire/air | UDP | Ethernet port |
| [GlobalUDP](/src/strategies/GlobalUDP)  | Electrical/radio impulses over wire/air | UDP | Ethernet port |
| [OverSampling](/src/strategies/OverSampling)  | Electrical/radio impulses over wire/air | [PJDLR](../src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) | 1 or 2 |
| [ThroughSerial](/src/strategies/ThroughSerial)  | Electrical/radio impulses over wire/air | [TSDL](../src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md) | 1 or 2 |
| [ThroughSerialAsync](/src/strategies/ThroughSerialAsync)  | Electrical/radio impulses over wire/air | [TSDL](../src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md) | 1 or 2 |
| [ThroughLoRa](/src/strategies/ThroughLoRa)  | Radio impulses over air | LoRa | 3 or 4 |
| [ESPNOW](/src/strategies/ESPNOW)  | Radio impulses over air | [ESPNOW](https://www.espressif.com/en/products/software/esp-now/overview) | WiFi link |
| [Any](/src/strategies/Any)  | Virtual inheritance, any of the above | Any of the above | Any of the above |

By default all strategies are included except `ThroughLoRa` and `ESPNOW`. To reduce memory footprint add for example `#define PJON_INCLUDE_SWBB` before PJON inclusion to include only `SoftwareBitBang` strategy. More than one strategy related constant can defined in the same program if that is required.

Supported definitions:
- `PJON_INCLUDE_SWBB` includes SoftwareBitBang
- `PJON_INCLUDE_AS` includes AnalogSampling
- `PJON_INCLUDE_ETCP` includes EthernetTCP
- `PJON_INCLUDE_GUDP` includes GlobalUDP
- `PJON_INCLUDE_LUDP` includes LocalUDP
- `PJON_INCLUDE_OS` includes OverSampling
- `PJON_INCLUDE_TS` includes ThroughSerial
- `PJON_INCLUDE_TSA` includes ThroughSerialAsync
- `PJON_INCLUDE_TL` includes ThroughLoRa
- `PJON_INCLUDE_EN` includes ESPNOW
- `PJON_INCLUDE_ANY` includes Any
- `PJON_INCLUDE_NONE` no strategy file included

Before using `ThroughLoRa` be sure to have [arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa) source available and to have defined the `PJON_INCLUDE_TL` constant before including `PJON.h`.

Before using `ESPNOW` be sure to have installed the required tools as described [here](/src/strategies/ESPNOW/README.md) and to have defined the `PJON_INCLUDE_EN` constant before including `PJON.h`.

### Network mode
The network mode can be changed with `set_shared_network` during runtime, for example moving from [local](/specification/PJON-protocol-specification-v3.0.md#local-mode) to [shared](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v3.0.md#shared-mode) mode:
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
Use `set_router` to configure the device in router mode, simply receiving all the incoming packets:
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
The [synchronous acknowledgement](/specification/PJON-protocol-acknowledge-specification-v1.0.md#synchronous-acknowledge) is by default enabled but can be disabled if required:
```cpp  
  bus.set_synchronous_acknowledge(false);
```

If the [asynchronous acknowledgement](/specification/PJON-protocol-acknowledge-specification-v1.0.md#asynchronous-acknowledge) feature is required you need to define the `PJON_INCLUDE_ASYNC_ACK` as following. The use of a constant has been chosen to save more than 1kB on sketches where this feature is not used (the packet id is used by the asynchronous acknowledgement process, so if necessary, play with that responsibly):
```cpp  
#define PJON_INCLUDE_ASYNC_ACK true
// Max number of old packet ids stored to avoid duplication
#define PJON_MAX_RECENT_PACKET_IDS 10  // by default 10
// If packet duplication occurs, higher PJON_MAX_RECENT_PACKET_IDS
#include <PJON.h>
```
Use `set_asynchronous_acknowledge` to enable the asynchronous acknowledgement:
```cpp  
  // Enable async ack
  bus.set_asynchronous_acknowledge(true);
```
See the [AsyncAck](/examples/ARDUINO/Network/SoftwareBitBang/AsyncAck) example to see more in detail how the asynchronous acknowledgement can be used.


### Packet identification
If packet duplication avoidance is required it is possible to add a 2 bytes [packet identifier](/specification/PJON-protocol-specification-v3.0.md#packet-identification) to guarantee uniqueness.
define the `PJON_INCLUDE_PACKET_ID` as following. The use of a constant has been chosen to save more than 1kB on sketches where this feature is not used:
```cpp  
#define PJON_INCLUDE_PACKET_ID true
// Max number of old packet ids stored to avoid duplication
#define PJON_MAX_RECENT_PACKET_IDS 10  // by default 10
// If packet duplication occurs, higher PJON_MAX_RECENT_PACKET_IDS
#include <PJON.h>
```
Use `set_packet_id` to enable the packet identification feature:
```cpp  
  bus.set_packet_id(true);
```
See the [UsePacketId](/examples/ARDUINO/Local/SoftwareBitBang/UsePacketId) example to see more in detail how the packet id can be used.

### Network service identification
Configure the instance to include a [network service identifier](/specification/PJON-protocol-specification-v3.0.md#network-services) in the packet. Ports from 0 to 8000 are reserved to known network services which index is present in the [known network services list](/specification/PJON-network-services-list.md), ports from 8001 to 65535 are free for custom use cases:
```cpp  
  bus.include_port(false);      // Avoid port inclusion (default)  
  bus.include_port(true, 8001); // Include custom port
```
See the [PortsUseExample](/examples/ARDUINO/Network/SoftwareBitBang/PortsUseExample) example to see more in detail how the port feature can be used.
