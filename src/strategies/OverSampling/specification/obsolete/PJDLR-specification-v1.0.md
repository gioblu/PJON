- PJON (Padded Jittering Operative Network) Protocol specification:
[v1.1](/specification/PJON-protocol-specification-v1.1.md)
- Acknowledge specification: [v0.1](/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Dynamic addressing specification: [v0.1](/specification/PJON-dynamic-addressing-specification-v0.1.md)
- PJDL (Padded Jittering Data Link) specification:
[PJDL v1.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v1.0.md) - **[PJDLR v1.0](/src/strategies/OverSampling/specification/PJDLR-specification-v1.0.md)**

```cpp
/*
Milan, Italy - 18/01/2017
PJDLR (Padded jittering data link) specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2019 All rights reserved

Related work: /src/strategies/SoftwareBitBang/
Compliant implementation versions: PJON 7.0-7.1

New feature: Packet preamble by Fred Larsen
*/
```
### PJDLR (Padded jittering data link / R version)
PJDLR (Padded jittering data link) has been specified to enable a new way to transmit data in simplex and half-duplex mode using cheap and low performance microcontrollers without the need of hardware interrupts for its working procedure. It is designed to support many devices sharing the same medium, to avoid collisions and operate in spite of interference. Extended tests proved its effectiveness on different media like electricity, radio frequency and light.

### Basic concepts
* Use a pattern of predefined initial padding bits to identify a potential byte transmission
* Use the falling edge from 1 to 0, present in padding bits, to achieve byte level synchronization
* Detect interference or absence of communication at byte level
* Enable channel analysis and collision avoidance
* Enable a collision free synchronous acknowledgement pattern

#### Byte transmission
Every byte is prepended with 2 synchronization padding bits and transmission occurs LSB-first. The first is a shorter than standard logic 1 followed by a standard logic 0. The reception tecnique is based on finding a logic 1 as long as the first padding bit within a certain threshold, synchronizing to its falling edge and checking if it is followed by a logic 0. If this pattern is detected, reception starts, if not, interference, synchronization loss or simply absence of communication is detected at byte level.
```cpp  
 _____ ___________________________
| Pad | Byte                      |
|_    |___       ___     _____    |
| |   |   |     |   |   |     |   |
|1| 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|_|___|___|_____|___|___|_____|___|
```
Padding bits are adding a certain overhead to information but are reducing the need of precise time tuning because synchronization is renewed every byte. All the first padding bit duration is the synchronization window the receiver has for every incoming byte. If the duration of the first padding bit is longer than expected the received signal is considered interference.

#### Packet transmission
Before a packet transmission, the medium is analyzed to detect ongoing communication and avoid collision. Thanks to the presence of padding bits, also a packet composed by 100 bytes, all with a decimal value of 0, can be transmitted safely without experiencing third-party collision. After assessed that the medium is free to use, a packet preamble, composed of a long 1 and a long 0, is transmitted to let a potential receiver to adjust its gain to the transmitted signal magnitude. The duration of the preamble bits have to be adjusted to match hardware sensitivity, gain refresh time and signal to noise ratio.

```cpp   
 _________ ______________ _______________ ______________ ______________ ________________
|Preamble |Pad| Byte     |Pad| Byte      |Pad| Byte     |Pad| Byte     |Pad| Byte       |
|_____    |_  |     __   |_  |      _   _|_  |      _   |_  |  _       |_  |  _    _    |
|     |   | | |    |  |  | | |     | | | | | |     | |  | | | | |      | | | | |  | |   |
|  1  | 0 |1|0|0000|11|00|1|0|00000|1|0|1|1|0|00000|1|00|1|0|0|1|000000|1|0|0|1|00|1|000|
|_____|___|_|_|____|__|__|_|_|_____|_|_|_|_|_|_____|_|__|_|_|_|_|______|_|_|_|_|__|_|___|
```
In a scenario where a stream of bytes is coming, low performance or clock inaccurate microcontroller can be correctly synchronized back with transmitter every byte (thanks to padding bits) and easily detect an interference or the end of transmission.

#### Synchronous acknowledgement
After packet reception, CRC is calculated and a single character is transmitted: `PJON_ACK` (value 6) if the packet's content is correct or `PJON_NAK` (value 21) if an error is detected.
```cpp  
 Transmission                                                        Response
 ________ ________________________________________                   ________ _____
|PREAMBLE| ID |  HEADER  | LENGTH | CONTENT | CRC | CRC COMPUTATION |PREAMBLE| ACK |
|____    |    |          |        |         |     |-------> <-------|____    |     |
|    |   | 12 | 00000100 |   5    |    64   |  72 | LATENCY         |    |   |  6  |
|____|___|____|__________|________|_________|_____|                 |____|___|_____|
```
