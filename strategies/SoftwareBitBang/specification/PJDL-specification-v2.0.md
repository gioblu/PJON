- PJON (Padded Jittering Operative Network) Protocol specification:
[v1.1](/specification/PJON-protocol-specification-v1.1.md)
- Acknowledge specification: [v0.1](/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Dynamic addressing specification: [v0.1](/specification/PJON-dynamic-addressing-specification-v0.1.md)
- PJDL (Padded Jittering Data Link) specification:
**[PJDL v2.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md)** - [PJDLR v2.0](/strategies/OverSampling/specification/PJDLR-specification-v2.0.md)

```cpp
/*
Milan, Italy - 21/09/2017
PJDL (Padded Jittering Data Link) v2.0 specification
Invented by Giovanni Blu Mitolo, released into the public domain

Related implementation: /strategies/SoftwareBitBang/
Compliant implementation versions: PJON 9.0 and following
Changelog: Added byte stream initializer
*/
```
### PJDL (Padded Jittering Data Link) v2.0
PJDL (Padded Jittering Data Link) is a simplex or half-duplex data link layer, that can be easily software emulated, enabling one or many to many communication over a single conductive medium or bus, connected to device's input-output ports, in both master-slave and multi-master configuration. It has been engineered to have limited minimum requirements, and to be efficiently executed on limited microcontrollers with low clock accuracy. No additional hardware is required to apply PJDL, and, being implemented in less than 350 lines of code, it is easily portable to many different architectures. Bus maximum length is limited by its electric resistance; it has been tested with up to 50m long insulated wires and results demonstrate the same high performance achieved with shorter lengths.

### Basic concepts
* Define a synchronization pad initializer to identify a byte transmission
* Use synchronization pad's falling edge to achieve byte level synchronization
* Use 3 consequent synchronization pads identify a stream transmission
* Detect interference or absence of communication at byte level
* Support collision avoidance and detection
* Support error detection
* Support 1 byte synchronous response to stream transmission

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
It is suggested to add 1-5 MΩ pull-down resistor as shown in the graph above to protect MCU pins and to reduce interference. The higher is the proximity of sources of induced interference, the higher is the required resistance of the pull-down resistor.     

#### Byte transmission
Every byte is prepended with a synchronization pad and transmission occurs LSB-first. The first bit is a longer than standard logic 1 followed by a standard logic 0. The reception method is based on finding a logic 1 as long as the first padding bit within a certain threshold, synchronizing to its falling edge and checking if it is followed by a logic 0. If this pattern is detected, reception starts, if not, interference, synchronization loss or simply absence of communication is detected at byte level.    
```cpp  
 __________ ___________________________
| SyncPad  | Byte                      |
|______    |___       ___     _____    |
|  |   |   |   |     |   |   |     |   |
|  | 1 | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|__|___|___|___|_____|___|___|_____|___|
   |
Minimum acceptable HIGH padding bit duration
```
Padding bits add a certain overhead but are reducing the need of precise timing because synchronization is renewed every byte. The first padding bit duration is the synchronization timeframe the receiver has to receive a byte. If the length of the first padding bit is less than the minimum acceptable duration, the received signal is considered interference. The minimum acceptable duration of the first padding bit must be lower than a padding bit duration; a large minimum acceptable duration reduces the chances of occurring false positives, a small minimum acceptable duration instead mitigates timing inaccuracy, for this reason is it suggested to evaluate its setting depending on requirements and available resources.

#### Byte stream transmission
Before a stream transmission, the communication medium is analysed, if logic 1 is present ongoing communication is detected and collision avoided, if logic 0 is detected for a duration longer than a byte transmission plus its synchronization pad and a small random timeframe, stream transmission starts with 3 synchronization pads, followed by data bytes. The presence of synchronization pads with their logic 1 between each byte ensures that also a stream composed of a series of bytes with decimal value 0 can be transmitted safely without risk of third-party collision.
```cpp  
 _________________ __________________________________
|   STREAM INIT   | DATA 1-65535 bytes               |
|_____ _____ _____|________________ _________________|
|Sync |Sync |Sync |Sync | Byte     |Sync | Byte      |
|___  |___  |___  |___  |     __   |___  |      _   _|
|   | |   | |   | |   | |    |  |  |   | |     | | | |
| 1 |0| 1 |0| 1 |0| 1 |0|0000|11|00| 1 |0|00000|1|0|1|
|___|_|___|_|___|_|___|_|____|__|__|___|_|_____|_|_|_|
```
In a scenario where a stream is received, low performance microcontrollers with inaccurate clock can correctly synchronize with transmitter during the stream initializer, and consequently each byte is received. Thanks to the presence of synchronization pads, byte stream separation, transmission end and error detection can be achieved at byte level.

#### Synchronous response
A stream transmission can be optionally followed by a synchronous response by its recipient.
```cpp  
Transmission                                            Response
 ______  ______  ______  ______  ______                   _____
| INIT || BYTE || BYTE || BYTE || BYTE | CRC COMPUTATION | ACK |
|------||------||------||------||------|-----------------|-----|
|      ||      ||      ||      ||      | LATENCY         |  6  |
|______||______||______||______||______|                 |_____|
```

Between stream transmission and a synchronous response there is a variable timeframe influenced by latency and CRC computation time. In order to avoid other devices to consider the medium free and disrupt an ongoing exchange, sender cyclically transmits a shorter than one bit logic 1 (which exact length depends on practical requirements) and consequently attempts to receive a response. On the other side receiver can synchronize its response transmission after the last incoming high bit, detect if acknowledgement was lost by transmitter and try again if necessary.
```cpp  
Transmission                                              Response
 ______  ______  ______  ______  ______   _   _   _   _   _ _____
| INIT || BYTE || BYTE || BYTE || BYTE | | | | | | | | | | | ACK |
|------||------||------||------||------| | | | | | | | | | |-----|
|      ||      ||      ||      ||      | | | | | | | | | | |  6  |
|______||______||______||______||______|_| |_| |_| |_| |_| |_____|

```

The maximum time dedicated to potential acknowledgement reception and consequent medium jittering is estimated adding the maximum stream length CRC computation time to the expected latency. Thanks to the presence of the jittering wave, many differently configured devices can coexist on the same medium with no risk of collision.
