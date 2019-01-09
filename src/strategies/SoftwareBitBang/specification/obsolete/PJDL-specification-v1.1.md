
```cpp
/*
Milan, Italy - 31/03/2017
PJDL (Padded jittering data link) specification
is an invention and intellectual property of Giovanni Blu Mitolo
Copyright 2010-2019 All rights reserved

Related work: /src/strategies/SoftwareBitBang/
Compliant implementation versions: PJON 8.0 and following

Changelog: Response generalization / refactoring
*/
```
### PJDL (Padded Jittering Data Link)
PJDL (Padded Jittering Data Link) has been specified to enable a new software emulated simplex or half-duplex data link layer supporting one or many to many communication on a single channel or medium. It can be run on cheap and low performance microcontrollers, it supports communication for many devices connected to the same wire and stable operation in spite of interference. The main bus maximum length limit is related to its electric resistance, if at destination a high bit voltage is lower than the input-output port threshold voltage, its value can be detected erroneously. It has been tested with up to 50 meters long insulated wire of different qualities demonstrating the ability to achieve reliable communication also in this scenario.   
```cpp  
 ______     ______      ______      ______      ______
|      |   |      |    |      |    |      |    |      |
|DEVICE|   |DEVICE|    |DEVICE|    |DEVICE|    |DEVICE|
|______|   |______|    |______|    |______|    |______|
___|___________|___________|___________|___________|___
 ___|__     ___|__    ___|__     ___|__  | SINGLE WIRE BUS
|      |   |      |  |      |   |      | |
|DEVICE|   |DEVICE|  |DEVICE|   |DEVICE| |___/\/\/\___ GND
|______|   |______|  |______|   |______|     1-5 MΩ
```
It is suggested to add 1-5 MΩ pull-down resistor as shown in the graph above to protect MCU pins and to reduce interference.

### Basic concepts
* Use a pattern of predefined initial padding bits to identify a potential byte transmission
* Use the falling edge from 1 to 0, present in padding bits, to achieve byte level synchronization
* Detect interference or absence of communication at byte level
* Support collision avoidance
* Support string transmission
* Support 1 byte synchronous response to string transmission

#### Byte transmission
Every byte is prepended with 2 synchronization padding bits and transmission occurs LSB-first. The first is a longer than standard logic 1 followed by a standard logic 0. The reception method is based on finding a logic 1 as long as the first padding bit within a certain threshold, synchronizing to its falling edge and checking if it is followed by a logic 0. If this pattern is detected, reception starts, if not, interference, synchronization loss or simply absence of communication is detected at byte level.    
```cpp  
 __________ ___________________________
| SyncPad  | Byte                      |
|______    |___       ___     _____    |
|  |   |   |   |     |   |   |     |   |
|  | 1 | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|__|___|___|___|_____|___|___|_____|___|
  |
SWBB_ACCEPTANCE
(or minimum acceptable HIGH padding bit duration)
```
Padding bits add a certain overhead but are reducing the need of precise timing because synchronization is renewed every byte. All the first high padding bit duration minus `SWBB_ACCEPTANCE` is the synchronization timeframe the receiver has to synchronize and correctly receive a byte. If the length of the first padding bit is less than `SWBB_ACCEPTANCE` the received signal is considered interference.

#### String transmission
Before a packet transmission, the medium is analyzed to detect ongoing communication and avoid collision. Thanks to the presence of padding bits, also a packet composed by 100 bytes, all with a decimal value of 0, can be transmitted safely without risk of third-party collision.   
```cpp  
 ________________ _________________ ________________
|Sync | Byte     |Sync | Byte      |Sync | Byte     |
|___  |     __   |___  |      _   _|___  |      _   |
|   | |    |  |  |   | |     | | | |   | |     | |  |
| 1 |0|0000|11|00| 1 |0|00000|1|0|1| 1 |0|00000|1|00|
|___|_|____|__|__|___|_|_____|_|_|_|___|_|_____|_|__|
```
In a scenario where a stream of bytes is coming, low performance or clock inaccurate microcontrollers can be correctly synchronized back with transmitter every byte (thanks to padding bits) and easily detect interference or the end of transmission.

#### Synchronous response
A string transmission can be optionally followed by a synchronous response by its recipient.
```cpp  
Transmission                                            Response
 ______  ______  ______  ______  ______                   _____
| BYTE || BYTE || BYTE || BYTE || BYTE | CRC COMPUTATION | ACK |
|------||------||------||------||------|-----------------|-----|
|      ||      ||      ||      ||      | LATENCY         |  6  |
|______||______||______||______||______|                 |_____|
```

Between a string transmission and a synchronous response there is a variable timeframe related to latency and computation time. In order to avoid other devices to consider the channel free and disrupt an ongoing exchange, sender cyclically transmits a `BIT_WIDTH / 4` HIGH bit and consequently attempts to receive a response. On the other side receiver can synchronize its response transmission after the last incoming high bit and try more than once if necessary.
```cpp  
Transmission                                              Response
 ______  ______  ______  ______  ______   _   _   _   _   _ _____
| BYTE || BYTE || BYTE || BYTE || BYTE | | | | | | | | | | | ACK |
|------||------||------||------||------| | | | | | | | | | |-----|
|      ||      ||      ||      ||      | | | | | | | | | | |  6  |
|______||______||______||______||______|_| |_| |_| |_| |_| |_____|

```

The maximum time dedicated to potential acknowledgement reception and consequent channel jittering is defined by the use case constraints like maximum packet length and devices distance. Thanks to the presence of the jittering wave, many differently configured devices can coexist on the same medium with no risk of collision.
