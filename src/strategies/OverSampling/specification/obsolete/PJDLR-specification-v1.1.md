
```cpp
/*
Milan, Italy - 31/03/2017
PJDLR (Padded jittering data link) specification is an invention
and intellectual property of Giovanni Blu Mitolo
Copyright 2010-2021 All rights reserved

Related work: /src/strategies/SoftwareBitBang/
Compliant implementation versions: PJON 8.0 and following

Packet preamble feature proposed by Fred Larsen

Changelog: Response generalization / refactoring
*/
```
### PJDLR (Padded jittering data link / R version)
PJDLR (Padded jittering data link) has been specified to enable a new software emulated simplex or half-duplex data link layer supporting one or many to many communication on a single channel or medium. It can be run on cheap and low performance microcontrollers, it supports communication for many devices connected to the same medium and stable operation in spite of interference. Its procedure has been specified to obtain long range and high reliability using FSK/ASK/OOK radio transceivers as physical layer.

#### Basic concepts
* Use a pattern of predefined initial padding bits to identify a byte
* Use the falling edge from 1 to 0, present in padding bits, to achieve byte level synchronization
* Detect interference or absence of communication at byte level
* Support string transmission using a preamble symbol
* Support collision avoidance
* Support 1 byte synchronous response to string transmission

#### Byte transmission
Every byte is prepended with 2 synchronization padding bits and transmission occurs LSB-first. The first is a shorter than standard logic 1 followed by a standard logic 0. The reception method is based on finding a logic 1 as long as the first padding bit within a certain threshold, synchronizing to its falling edge and checking if it is followed by a logic 0. If this pattern is detected, reception starts, if not, interference, synchronization loss or simply absence of communication is detected at byte level.  
```cpp  
 _____ ___________________________
| Pad | Byte                      |
|_    |___       ___     _____    |
| |   |   |     |   |   |     |   |
|1| 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|_|___|___|_____|___|___|_____|___|
  |
OS_ACCEPTANCE
(or minimum acceptable HIGH padding bit duration)
```
Padding bits add a certain overhead but are reducing the need of precise timing because synchronization is renewed every byte. All the first high padding bit duration minus `OS_ACCEPTANCE` is the synchronization timeframe the receiver has to synchronize and correctly receive a byte. If the length of the first padding bit is less than `OS_ACCEPTANCE` the received signal is considered interference.

#### String transmission
Before a packet transmission, the medium is analyzed to detect ongoing communication and avoid collision. Thanks to the presence of padding bits, also a packet composed by 100 bytes, all with a decimal value of 0, can be transmitted safely without risk of third-party collision. After assessed that the medium is free to use, a packet preamble, composed of a long 1 and a long 0, is transmitted to let a potential receiver to adjust its gain to the transmitted signal magnitude. The duration of the preamble bits have to be adjusted to match hardware sensitivity, gain refresh time and signal to noise ratio.

```cpp   
 _________ ______________ _______________ ______________ ______________
|Preamble |Pad| Byte     |Pad| Byte      |Pad| Byte     |Pad| Byte     |
|_____    |_  |     __   |_  |      _   _|_  |      _   |_  |  _       |
|     |   | | |    |  |  | | |     | | | | | |     | |  | | | | |      |
|  1  | 0 |1|0|0000|11|00|1|0|00000|1|0|1|1|0|00000|1|00|1|0|0|1|000000|
|_____|___|_|_|____|__|__|_|_|_____|_|_|_|_|_|_____|_|__|_|_|_|_|______|
```
In a scenario where a stream of bytes is coming, low performance or clock inaccurate microcontrollers can be correctly synchronized back with transmitter every byte (thanks to padding bits) and easily detect interference or the end of transmission.


#### Synchronous response
A string transmission can be optionally followed by a synchronous response by its recipient.
```cpp  
Transmission                                                      Response
 ________ ______  ______  ______  ______                   ________ _____
|PREAMBLE| BYTE || BYTE || BYTE || BYTE | CRC COMPUTATION |PREAMBLE| ACK |
|____    |------||------||------||------|-----------------|____    |     |
|    |   |      ||      ||      ||      | LATENCY         |    |   |  6  |
|____|___|______||______||______||______|                 |____|___|_____|
```

The maximum time dedicated to potential acknowledgement reception has to be same for all connected devices, and it is defined by the use case constraints like maximum packet length and latency or physical distance between devices.
