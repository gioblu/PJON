
```cpp
/*
Milan, Italy - 10/04/2010
The Padded jittering data link layer specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2016 All rights reserved

Related work: https://github.com/gioblu/PJON/blob/master/strategies/SoftwareBitBang/
Compliant implementation versions: PJON 1.0-5.0
*/
```
###The Padded jittering data link layer
The first experimental specification of the Padded jittering data link layer has been drafted to propose a new way to transmit data with cheap and low performance microcontrollers without the necessity of hardware interrupts for its working procedure. Extended tests proved its effectiveness on different media like electricity, radio frequency and light.

###Basic concepts
* Use a pattern of predefined initial padding bits to identify a byte transmission
* Use the falling edge from 1 to 0, present in padding bits, to achieve byte level synchronization
* Detect interference or absence of communication at byte level

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
 ACCEPTANCE
```
Padding bits are adding a certain overhead to information but are reducing the need of precise time tuning because synchronization is renewed every byte. All the first padding bit duration minus `ACCEPTANCE` is the synchronization window the receiver has for every incoming byte. If the length of the first padding bit is less than `ACCEPTANCE` the received signal is considered interference.

####String transmission
```cpp  
 ________________ _________________ ________________ ________________ __________________
|Sync | Byte     |Sync | Byte      |Sync | Byte     |Sync | Byte     |Sync | Byte       |
|___  |     __   |___  |      _   _|___  |      _   |___  |  _       |___  |  _    _    |
|   | |    |  |  |   | |     | | | |   | |     | |  |   | | | |      |   | | | |  | |   |
| 1 |0|0000|11|00| 1 |0|00000|1|0|1| 1 |0|00000|1|00| 1 |0|0|1|000000| 1 |0|0|1|00|1|000|
|___|_|____|__|__|___|_|_____|_|_|_|___|_|_____|_|__|___|_|_|_|______|___|_|_|_|__|_|___|
```
In a scenario where a stream of byte is coming, following this strategy a low performance or clock inaccurate microcontroller can be correctly synchronized back with the transmitter every byte and easily detect an interference or the end of transmission.


####Synchronous acknowledgement
After packet reception, the CRC is calculated and a single character is transmitted, `ACK` (value 6) if the packet's content resulted corrected or `NAK` (value 21) if an error is detected.
```cpp  
Transmission                                               Response
 ________________________________________                   _____
| ID |  HEADER  | LENGTH | CONTENT | CRC | CRC COMPUTATION | ACK |
|----|----------|--------|---------|-----|-------> <-------|-----|
| 12 | 00000100 |   5    |    64   |  72 | LATENCY         |  6  |
|____|__________|________|_________|_____|                 |_____|
```

In the middle of packet transmission and acknowledgement transmission there is a variable timeframe, influenced by latency and CRC computation time. In order to avoid other devices consider the bus free and start transmitting in the middle of a transmission and a response, the packet's transmitter cyclically transmits a `BIT_WIDTH / 4` HIGH bit and then attempts to receive a response. On the other side the receiver can synchronize its acknowledgement transmission after the last incoming HIGH bit and try more than once if necessary.
```cpp  
Transmission                                                         Response
 ________________________________________   _   _   _   _   _   _   _ _____
| ID |  HEADER  | LENGTH | CONTENT | CRC | | | | | | | | | | | | | | | ACK |
|----|----------|--------|---------|-----| | | | | | | | | | | | | | |-----|
| 12 | 00000100 |  500   |         |  72 | | | | | | | | | | | | | | |  6  |
|____|__________|________|_________|_____|_| |_| |_| |_| |_| |_| |_| |_____|

```

The maximum time dedicated to potential acknowledgement reception and consequent channel jittering is defined by the use case constrains like maximum packet length and devices distance.
