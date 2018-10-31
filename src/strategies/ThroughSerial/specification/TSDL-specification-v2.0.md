### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v3.0](/specification/PJON-protocol-specification-v3.0.md)
- [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [Dynamic addressing specification v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md)
- [PJDLR (Padded Jittering Data Link over Radio) v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md)
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- **[TSDL (Tardy Serial Data Link) v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)**
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## TSDL v2.0
```
Invented by Giovanni Blu Mitolo
Originally published: 20/11/2017, latest revision: 31/10/2018
Related implementation: /src/strategies/ThroughSerial/
Compliant versions: PJON v10.0 and following
Released into the public domain
```
TSDL (Tardy Serial Data Link) is a simplex or half-duplex serial data link that supports both master-slave and multi-master configuration. It supports collision avoidance, reliable frame separation and optional synchronous response to frame transmissions.
```cpp  
 ______ TX   RX ______
|      |-------|      |
|DEVICE|       |DEVICE|
|______|-------|______|
        RX   TX
```
TSDL can be used to establish a point-to-point link between two devices if used with a bare serial link, or to support one or many to many communication using radio or RS485 transceivers.  

### Basic concepts
* Frame separation support provided by [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)
* Support collision avoidance
* Support optional 1 byte synchronous response to frame transmissions

### Collision avoidance
Before a frame transmission, the serial buffer is read, if not empty ongoing communication is detected and collision avoided, if empty for a duration longer than the time-in (that should be common on all connected devices) before transmission plus a short random time, frame transmission starts in which the packet is entirely transmitted.

### Synchronous response
A frame transmission can be optionally followed by a synchronous response by its recipient.
```cpp  
Transmission                                    Response
 _______  ______  ______  _____                   _____
| START || BYTE || BYTE || END | CRC COMPUTATION | ACK |
|-------||------||------||-----|-----------------|-----|
|  149  ||  H   ||  I   || 234 | LATENCY         |  6  |
|_______||______||______||_____|                 |_____|
```
Between frame transmission and a synchronous response there is a variable timeframe influenced by latency and CRC computation time. The maximum time dedicated to potential acknowledgement reception must be shorter than the transmission time-in (to avoid other devices to disrupt a response exchange) and it is estimated adding the maximum frame length CRC computation time to the expected latency.
