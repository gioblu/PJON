- [Addressing](/documentation/addressing.md)
- **[Configuration](/documentation/configuration.md)**
- [Data reception](/documentation/data-reception.md)
- [Data transmission](/documentation/data-transmission.md)
- [Error handling](/documentation/error-handling.md)
- [IO setup](/documentation/io-setup.md)


Before approaching to the PJON class it is possible to define the packets and content buffer length.  Pre-defining `PJON_MAX_PACKETS` and `PJON_PACKET_MAX_LENGTH` it is possible to configure this constants to reach the project and memory requirements. Obviously, the less memory is dedicated to this buffers, the more memory can be used for something else.
```cpp  
#define PJON_MAX_PACKETS 1
#define PJON_PACKET_MAX_LENGTH 20
#include <PJON.h>
/* PJON can store up to 1 packet of up to
   20 characters - packet overhead (from 4 to 13 depending by configuration) */
```
Templates can be scary at first sight, but they are quite straight-forward and efficient:
```cpp  
  PJON<SoftwareBitBang> bus;
```
In the example above the PJON object is instanced passing [SoftwareBitBang](/wiki/SoftwareBitBang) strategy as template parameter. Strategies are classes abstracting the data-link layer, making PJON easy to be uses on different media. It is possible to instance more than one PJON object with different strategies:
```cpp  
  PJON<SoftwareBitBang> wiredBus;
  PJON<EthernetTCP>     tcpBus;
```
There are 5 strategies available to communicate data with PJON:

| Strategy      | Medium        | Pins needed   |
| ------------- | ------------- | ------------- |
| [SoftwareBitBang](/strategies/SoftwareBitBang)  | wire   | 1 or 2 |
| [AnalogSampling](/strategies/AnalogSampling)  | light  | 1 or 2  |
| [EthernetTCP](/strategies/EthernetTCP)  | wired or WiFi  | Ethernet port  |
| [LocalUDP](/strategies/LocalUDP)  | wired or WiFi  | Ethernet port  |
| [OverSampling](/strategies/OverSampling)  | radio, wire  | 1 or 2 |
| [ThroughSerial](/strategies/ThroughSerial)  | serial port  | 1 or 2 |

By default all strategies are included. To reduce memory footprint add for example `#define PJON_INCLUDE_SWBB` before PJON inclusion, to include only `SoftwareBitBang` strategy. You can define more than one strategy related constant if necessary.

Supported definitions:
- `PJON_INCLUDE_SWBB` includes SoftwareBitBang
- `PJON_INCLUDE_AS` includes AnalogSampling
- `PJON_INCLUDE_ETCP` includes EthernetTCP
- `PJON_INCLUDE_LUDP` includes LocalUDP
- `PJON_INCLUDE_OS` includes OverSampling
- `PJON_INCLUDE_TS` includes ThroughSerial
- `PJON_INCLUDE_NONE` no strategy file included

Configure network state (local or shared). If local (passing `false`), the PJON protol layer procedure is based on a single byte device id to univocally communicate with a device; if in shared mode (passing `true`) the protocol adopts also a 4 byte bus id to univocally communicate with a device in a certain bus:
```cpp  
  bus.set_shared_network(true);
```
Configure the communication mode:
```cpp  
  bus.set_communication_mode(PJON_SIMPLEX); // Run in mono-directional PJON_SIMPLEX mode
  bus.set_communication_mode(PJON_HALF_DUPLEX);  // Run in bi-directional PJON_HALF_DUPLEX mode
```
Configure synchronous acknowledge:
```cpp  
  bus.set_synchronous_acknowledge(false); // Avoid sync ack
```
If you are interested in including the asynchronous acknowledgement feature in your sketch, you need to define the `PJON_INCLUDE_ASYNC_ACK` as following (made to save more than 1kB on sketches where this feature is not used):
```cpp  
#define PJON_INCLUDE_ASYNC_ACK true
#include <PJON.h>
```
Configure asynchronous acknowledge:
```cpp  
  bus.set_asynchronous_acknowledge(true); // Enable async ack
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
Avoid packet auto-deletion:
```cpp  
  bus.set_packet_auto_deletion(false);
```
