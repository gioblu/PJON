- [Addressing](/documentation/addressing.md)
- **[Configuration](/documentation/configuration.md)**
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)
- [Routing](/documentation/routing.md)

### Basic configuration
Before instantiating the PJON class it is possible to define the packets and content buffer length.  Predefining `PJON_MAX_PACKETS` and `PJON_PACKET_MAX_LENGTH` it is possible to configure this constraints to reach the project memory requirements. Obviously, the less memory is dedicated to this buffers, the more memory can be used for something else.
```cpp  
#define PJON_MAX_PACKETS 1
#define PJON_PACKET_MAX_LENGTH 20
#include <PJON.h>
/* PJON can store up to 1 packet of up to
   20 characters - packet overhead
   (from 5 to 22 bytes depending by configuration) */
```
Templates can be scary at first sight, but they are quite straight-forward and efficient:
```cpp  
  PJON<SoftwareBitBang> bus;
```
In the example above the PJON object is instantiated passing [SoftwareBitBang](/src/strategies/SoftwareBitBang/README.md) strategy as template parameter. Strategies are classes abstracting the data link layer, making PJON easy to be used on different media. It is possible to instantiate more than one PJON object with different strategies in the same sketch:
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
| [ThroughLoRa](/src/strategies/ThroughLoRa)  | Radio impulses over air | LoRa | 3 or 4 |

By default all strategies are included except `ThroughLoRa`. To reduce memory footprint add for example `#define PJON_INCLUDE_SWBB` before PJON inclusion to include only `SoftwareBitBang` strategy. More than one strategy related constant can defined in the same program if that is required.

Supported definitions:
- `PJON_INCLUDE_SWBB` includes SoftwareBitBang
- `PJON_INCLUDE_AS` includes AnalogSampling
- `PJON_INCLUDE_ETCP` includes EthernetTCP
- `PJON_INCLUDE_GUDP` includes GlobalUDP
- `PJON_INCLUDE_LUDP` includes LocalUDP
- `PJON_INCLUDE_OS` includes OverSampling
- `PJON_INCLUDE_TS` includes ThroughSerial
- `PJON_INCLUDE_TL` includes ThroughLoRa
- `PJON_INCLUDE_ANY` includes Any - Required for `StrategyLink` if using router
- `PJON_INCLUDE_NONE` no strategy file included

Before using `ThroughLoRa` be sure to have [arduino-LoRa](https://github.com/sandeepmistry/arduino-LoRa) source available and to have defined `PJON_INCLUDE_TL` constant before including `PJON.h`.

### Network configuration
Configure network mode (local or shared). If local (passing `false`) a single byte called device id is used for device identification; if in shared mode (passing `true`) a 4 byte bus id is also used to univocally identify a group of devices:
```cpp  
  bus.set_shared_network(true);
```
A PJON object can be instantiated to communicate in shared mode simply passing its bus id:
```cpp
uint8_t bus_id[4] = {1, 2, 3, 4};
PJON<SoftwareBitBang> bus(bus_id, 44);
// Device id 44, bus id 1.2.3.4 in shared mode
```
It is strongly suggested to request a unique PJON bus id for your group of devices [here](http://www.pjon.org/get-bus-id.php).

Configure the communication mode:
```cpp  
  // Run in mono-directional PJON_SIMPLEX mode
  bus.set_communication_mode(PJON_SIMPLEX);
  // Run in bi-directional PJON_HALF_DUPLEX mode
  bus.set_communication_mode(PJON_HALF_DUPLEX);
```
Configure synchronous acknowledge:
```cpp  
  // Avoid sync ack
  bus.set_synchronous_acknowledge(false);
```
Force CRC32 use for every packet sent:
```cpp  
  bus.set_crc_32(true);
```
PJON by default includes the sender information in the packet. If you don't need this information you can use the provided setter to reduce overhead and higher communication speed:
```cpp  
  bus.include_sender_info(false);
```
Configure your device to act as a router, so receiving all the incoming packets:
```cpp  
  bus.set_router(true);
```
Configure the instance to include a port identification in the packet. Ports from 0 to 8000 are reserved to known network services which index is present in the [known network services list](/specification/PJON-network-services-list.md), ports from 8001 to 65535 are free for custom use cases:
```cpp  
  bus.include_port(false);      // Avoid port inclusion (default)  
  bus.include_port(true, 8001); // Include custom port
```
See the [PortsUseExample](/examples/ARDUINO/Network/SoftwareBitBang/PortsUseExample) example to see more in detail how the port feature can be used.

Avoid packet auto-deletion when delivered:
```cpp  
  bus.set_packet_auto_deletion(false);
```

### Extended configuration
If packet duplication avoidance is required it is possible to add a 2 bytes id used to guarantee packet uniqueness.
define the `PJON_INCLUDE_PACKET_ID` as following. The use of a constant has been chosen to save more than 1kB on sketches where this feature is not used:
```cpp  
#define PJON_INCLUDE_PACKET_ID true
// Max number of old packet ids stored to avoid duplication
#define PJON_MAX_RECENT_PACKET_IDS 10  // by default 10
// If packet duplication occurs, higher PJON_MAX_RECENT_PACKET_IDS
#include <PJON.h>
```
Use the provided setter to add the packet id to configuration:
```cpp  
  bus.set_packet_id(true);
```
See the [UsePacketId](/examples/ARDUINO/Local/SoftwareBitBang/UsePacketId) example to see more in detail how the packet id can be used.

If the asynchronous acknowledgement feature is required you need to define the `PJON_INCLUDE_ASYNC_ACK` as following. The use of a constant has been chosen to save more than 1kB on sketches where this feature is not used (the packet id is used by the asynchronous acknowledgement process, so if necessary, play with that responsibly):
```cpp  
#define PJON_INCLUDE_ASYNC_ACK true
// Max number of old packet ids stored to avoid duplication
#define PJON_MAX_RECENT_PACKET_IDS 10  // by default 10
// If packet duplication occurs, higher PJON_MAX_RECENT_PACKET_IDS
#include <PJON.h>
```
Use the provided setter to use asynchronous acknowledgement:
```cpp  
  // Enable async ack
  bus.set_asynchronous_acknowledge(true);
```
See the [AsyncAck](/examples/ARDUINO/Network/SoftwareBitBang/AsyncAck) example to see more in detail how the asynchronous acknowledgement can be used.
