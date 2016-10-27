- [Addressing](https://github.com/gioblu/PJON/tree/6.0/documentation/addressing.md)
- **[Configuration](https://github.com/gioblu/PJON/tree/6.0/documentation/configuration.md)**
- [Data reception](https://github.com/gioblu/PJON/tree/6.0/documentation/data-reception.md)
- [Data transmission](https://github.com/gioblu/PJON/tree/6.0/documentation/data-transmission.md)
- [Error handling](https://github.com/gioblu/PJON/tree/6.0/documentation/error-handling.md)
- [IO setup](https://github.com/gioblu/PJON/tree/6.0/documentation/io-setup.md)


Before approaching to the PJON class it is possible to define the packets and content buffer length.  Pre-defining `MAX_PACKETS` and `PACKET_MAX_LENGTH` it is possible to configure this constants to reach the project and memory requirements. Obviously, the less memory is dedicated to this buffers, the more memory can be used for something else.
```cpp  
#define MAX_PACKETS 1
#define PACKET_MAX_LENGTH 20
#include <PJON.h>
/* PJON can store up to 1 packet of up to
   20 characters - packet overhead (from 4 to 13 depending by configuration) */
```

Templates can be scary at first sight, but they are quite straight-forward and efficient. Lets start coding, looking how to instantiate in the simplest way the `PJON` object that in the example is called bus with a wire compatible physical layer:
```cpp  
  PJON<> bus;

  // or

  PJON<SoftwareBitBang> bus;
```
The PJON bus runs by default through the [SoftwareBitBang](https://github.com/gioblu/PJON/wiki/SoftwareBitBang) strategy. There are 5 strategies available to communicate data with PJON on various media:

**[EthernetTCP](https://github.com/gioblu/PJON/tree/master/strategies/EthernetTCP)** | **Medium:** Ethernet port, wired or WiFi

With the EthernetTCP PJON strategy, multiple devices with Ethernet ports can use PJON to communicate with each other on a LAN, WAN or across the Internet.

**[LocalUDP](https://github.com/gioblu/PJON/tree/master/strategies/LocalUDP)** | **Medium:** Ethernet port, wired or WiFi

With the LocalUDP PJON strategy, multiple devices with Ethernet ports can use PJON to communicate with each other on a local subnet, wired or over WiFi or both.

**[OverSampling](https://github.com/gioblu/PJON/tree/master/strategies/OverSampling)** | **Medium:** Radio, Wire |
**Pins used:** 1 or 2

Oversampling strategy comes from the [PJON_ASK](https://github.com/gioblu/PJON_ASK) repository, and it was integrated in the PJON repository from version 3.0 beta, as a data link layer strategy. Bits are over-sampled to have high resilience in high interference scenarios, like using an ASK/FSK cheap radio transceivers in an urban environment. It is tested effectively with many versions of the ASK/FSK 315/433Mhz modules available on the market with up to 5km range, but it works nominally also through wires and the human body.

**[SoftwareBitBang](https://github.com/gioblu/PJON/tree/master/strategies/SoftwareBitBang)** | **Medium:** Wire | **Pins used:** 1 or 2

SoftwareBitBang is the default data link layer strategy used by the PJON template object. This implementation is based on `micros()` and `delayMicroseconds()`. It makes no use of dedicated timers or interrupt driven strategies to handle communication. It is designed to have a small memory footprint and to be extremely resilient to interference and timing inaccuracies. Thanks to the use of a dedicated digitalWriteFast library, can be achieved fast and reliable cross-architecture communication through one or two pins.

**[ThroughSerial](https://github.com/gioblu/PJON/tree/master/strategies/ThroughSerial)** | **Medium:** Hardware Serial port |
**Pins used:** 2

With ThroughSerial data link layer strategy, PJON can run through a software emulated or hardware Serial port. Thanks to this choice it is possible to leverage of virtually all the arduino compatible serial transceivers, like RS485, radio or infrared modules, still having PJON unchanged on top.

Configure network state (local or shared). If local, so if passing `false`, the PJON protol layer procedure is based on a single byte device id to univocally communicate with a device; if in shared mode, so passing `true`, the protocol adopts a 4 byte bus id to univocally communicate with a device in a certain bus:
```cpp  
  bus.set_shared_network(true);
```
Configure the communication mode:
```cpp  
  bus.set_communication_mode(SIMPLEX);     // Run in mono-directional SIMPLEX mode
  bus.set_communication_mode(HALF_DUPLEX); // Run in bi-directional HALF_DUPLEX mode
```
Configure acknowledge:
```cpp  
  bus.set_acknowledge(false);
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
