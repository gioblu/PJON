#### Network layer
- PJON (Padded Jittering Operative Network) Protocol specification: [v3.0](/specification/PJON-protocol-specification-v3.0.md)
- PJON Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- PJON Dynamic addressing specification: [v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- PJON Network services: [list](/specification/PJON-network-services-list.md)
#### Data link layer
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - **[PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)**
- TSDL (Tardy Serial Data Link) specification: [TSDL v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- SFSP (Secure Frame Separation Protocol) specification: [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

```cpp
/*
Milan, Italy
Originally published: 20/11/2017
PJDLS (Padded Jittering Data Link byte Stuffed) v2.0
Invented by Giovanni Blu Mitolo,
released into the public domain

Related implementation: /src/strategies/AnalogSampling/
Compliant versions: PJON v10.0 and following

Changelog:
- Frame separation obtained with SFSP v2.0
*/
```
### PJDLS v2.0
PJDLS (Padded Jittering Data Link byte Stuffed) is a simplex or half-duplex data link layer, that can be easily software emulated, enabling one or many to many communication in both master-slave and multi-master configuration, optimized for use cases where high quality synchronization cannot be achieved. Frame separation is obtained with the use of [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md). It has been engineered to have limited minimum requirements, and to be efficiently executed on limited microcontrollers with poor clock accuracy. No additional hardware is required to apply PJDLS, and, being implemented in c++, in less than 350 lines of code, it is easily portable to many different architectures.

#### Basic concepts
* Define a synchronization pad initializer to identify a byte
* Use synchronization pad's falling edge to achieve byte level synchronization
* Detect interference or absence of communication at byte level
* Frame separation obtained with [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)  
* Support collision avoidance
* Support 1 byte synchronous response to frame

#### Byte transmission
Each byte is prepended with a synchronization pad and transmission occurs LSB-first. The first bit is a longer than standard logic 1 followed by a standard logic 0. The reception method is based on finding a logic 1 as long as the first padding bit within a certain threshold, synchronizing to its falling edge and checking if it is followed by a logic 0. If this pattern is detected, reception starts, if not, interference, synchronization loss or simply absence of communication is detected at byte level.    
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
Padding bits add a certain overhead but are reducing the need of precise timing because synchronization is renewed every byte. The first padding bit duration is the synchronization timeframe the receiver has to receive a byte. If the length of the first padding bit is less than the minimum acceptable duration, the received signal is considered interference.

#### Synchronous response
A frame transmission can be optionally followed by a synchronous response by its recipient. This feature is available for both master-slave and multi-master. In multi-master configuration the maximum acceptable acknowledgement overall response time must be less than the initial channel analysis duration before frame transmission.
```cpp  
Transmission                                    Response
 _______  ______  ______  _____                   _____
| START || BYTE || BYTE || END | CRC COMPUTATION | ACK |
|-------||------||------||-----|-----------------|-----|
|  149  ||  H   ||  I   || 234 | LATENCY         |  6  |
|_______||______||______||_____|                 |_____|
```

In master-slave configuration the maximum time dedicated to potential acknowledgement reception it is defined by the use case constraints like maximum packet length and latency or physical distance between devices.

#### Communication modes
The proposed communication modes are the result of years of testing and optimization for light pulses communication and have been selected to be easily supported also by limited microcontrollers.  

| MODE | Bit timing | Sync bit timing | Pad-bit ratio | Speed               |
| ---- | ---------- | --------------- | ------------- | ------------------- |
| 1    | 750        | 1050            | 1.4           | 128B/s  -  1024Bb   |
| 2    | 572        | 728             | 1.2727        | 170B/s  -  1361Bb   |
| 3    | 188        | 428             | 2.2765        | 471B/s  -  3773Bb   |
| 4    | 128        | 290             | 2.2656        | 639B/s  -  5547Bb   |
| 5    | 56         | 128             | 2.2857        | 1582B/s - 12658Bb   |

Binary timing durations are expressed in microseconds.
