- PJON (Padded Jittering Operative Network) Protocol specification:
[v1.0](/specification/PJON-protocol-specification-v1.0.md)
- Acknowledge specification: [v0.1](/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Dynamic addressing specification: [v0.1](/specification/PJON-dynamic-addressing-specification-v0.1.md)
- PJDL (Padded Jittering Data Link) specification:
**[PJDL v1.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v1.0.md)** - [PJDLR v1.0](/strategies/OverSampling/specification/PJDLR-specification-v1.0.md)

```cpp
/*
Milan, Italy - 10/04/2010
PJDL (Padded jittering data link) specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2017 All rights reserved

Related work: /strategies/SoftwareBitBang/
Compliant implementation versions: PJON 6.0 and following
*/
```
###PJDL (Padded Jittering Data Link)
PJDL (Padded Jittering Data Link) has been specified to enable a new way to transmit data in simplex and half-duplex mode using cheap and low performance microcontrollers, totally software emulated, without the need of hardware interrupts for its working procedure. It is designed to support many devices sharing the same medium, to avoid collisions and operate in spite of interference. Extended tests proved its effectiveness on different media like electricity, radio frequency and light.

###Basic concepts
* Use a pattern of predefined initial padding bits to identify a potential byte transmission
* Use the falling edge from 1 to 0, present in padding bits, to achieve byte level synchronization
* Detect interference or absence of communication at byte level
* Enable channel analysis and collision avoidance
* Enable a collision free synchronous acknowledgement pattern

####Byte transmission
Every byte is prepended with 2 synchronization padding bits and transmission occurs LSB-first. The first is a longer than standard logic 1 followed by a standard logic 0. The reception tecnique is based on finding a logic 1 as long as the first padding bit within a certain threshold, synchronizing to its falling edge and checking if it is followed by a logic 0. If this pattern is detected, reception starts, if not, interference, synchronization loss or simply absence of communication is detected at byte level.
```cpp  
 __________ ___________________________
| SyncPad  | Byte                      |
|______    |___       ___     _____    |
|  |   |   |   |     |   |   |     |   |
|  | 1 | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|_ |___|___|___|_____|___|___|_____|___|
   |
 SWBB_ACCEPTANCE (or minimum acceptable HIGH padding bit duration)
```
Padding bits are adding a certain overhead to information but are reducing the need of precise timing because synchronization is renewed every byte. All the first padding bit duration minus `SWBB_ACCEPTANCE` is the synchronization window the receiver has for every incoming byte. If the length of the first padding bit is less than `SWBB_ACCEPTANCE` the received signal is considered interference.

####Packet transmission
Before a packet transmission, the medium is analyzed to detect ongoing communication and avoid collision. Thanks to the presence of padding bits, also a packet composed by 100 bytes, all with a decimal value of 0, can be transmitted safely without risk of third-party collision.   
```cpp  
 ________________ _________________ ________________ ________________ __________________
|Sync | Byte     |Sync | Byte      |Sync | Byte     |Sync | Byte     |Sync | Byte       |
|___  |     __   |___  |      _   _|___  |      _   |___  |  _       |___  |  _    _    |
|   | |    |  |  |   | |     | | | |   | |     | |  |   | | | |      |   | | | |  | |   |
| 1 |0|0000|11|00| 1 |0|00000|1|0|1| 1 |0|00000|1|00| 1 |0|0|1|000000| 1 |0|0|1|00|1|000|
|___|_|____|__|__|___|_|_____|_|_|_|___|_|_____|_|__|___|_|_|_|______|___|_|_|_|__|_|___|
```
In a scenario where a stream of bytes is coming, low performance or clock inaccurate microcontrollers can be correctly synchronized back with transmitter every byte (thanks to padding bits) and easily detect interference or the end of transmission.

####Synchronous acknowledgement
After packet reception, CRC is calculated and a single character is transmitted: `PJON_ACK` (value 6) if the packet's content is correct or `PJON_NAK` (value 21) if an error is detected.
```cpp  
Transmission                                               Response
 ________________________________________                   _____
| ID |  HEADER  | LENGTH | CONTENT | CRC | CRC COMPUTATION | ACK |
|----|----------|--------|---------|-----|-------> <-------|-----|
| 12 | 00000100 |   5    |    64   |  72 | LATENCY         |  6  |
|____|__________|________|_________|_____|                 |_____|
```

Between a packet transmission and a synchronous acknowledgement transmission from the packet's receiver there is a variable timeframe influenced in its duration by medium latency and CRC computation time. In order to avoid other devices to consider the medium free and start transmitting in the middle of a transmission and a response, the packet's transmitter cyclically transmits a `BIT_WIDTH / 4` HIGH bit and consequently attempts to receive a response. On the other side the receiver can synchronize its acknowledgement transmission after the last incoming HIGH bit and try more than once if necessary.
```cpp  
Transmission                                                         Response
 ________________________________________   _   _   _   _   _   _   _ _____
| ID |  HEADER  | LENGTH | CONTENT | CRC | | | | | | | | | | | | | | | ACK |
|----|----------|--------|---------|-----| | | | | | | | | | | | | | |-----|
| 12 | 00000100 |  500   |         |  72 | | | | | | | | | | | | | | |  6  |
|____|__________|________|_________|_____|_| |_| |_| |_| |_| |_| |_| |_____|

```

The maximum time dedicated to potential acknowledgement reception and consequent channel jittering is defined by the use case constrains like maximum packet length and devices distance. Thanks to the presence of the jittering wave, many differently configured devices can coexist on the same medium with no risk of collision.
