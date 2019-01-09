
## TSDL v2.0
```
Invented by Giovanni Blu Mitolo
Originally published: 20/11/2017, latest revision: 9/11/2018
Related implementation: /src/strategies/ThroughSerial/
Compliant versions: PJON v10.0 and following
Released into the public domain
```
TSDL (Tardy Serial Data Link) is a simplex or half-duplex serial data link that supports both master-slave and multi-master modes. It supports collision avoidance, reliable frame separation and optional synchronous response to frame transmissions.
```cpp  
 ______ TX   RX ______
|      |-------|      |
|DEVICE|       |DEVICE|
|______|-------|______|
        RX   TX
```
TSDL can be used to establish a point-to-point link between two devices if used with a bare serial link, or to support one or many to many communication using radio or RS485 transceivers.  

### Medium access control
TSDL operates in master-slave mode applying the request-response procedure. Being impossible to implement a carrier-sense procedure over a serial port TSDL uses variation of slotted ALOHA medium access method for multi-master mode. Before a frame transmission the serial buffer is read, if not empty ongoing communication is detected and collision avoided, if empty for a duration longer than the response time-out plus a short random time, frame transmission starts in which the packet is entirely transmitted. Of all contention based random multiple access methods, slotted ALOHA, which maximum data throughput is only 36.8% of the available bandwidth, is one of the least efficient, therefore TSDL in multi-master mode should not be used in networks where many devices often need to arbitrarily transmit data.

### Frame transmission
Before a frame transmission the communication medium is analysed, if any data is received communication is detected and collision is avoided, if logic 0 is detected for a duration longer than the response time-out plus a small random time, data is transmitted encapsulated in a [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md) frame.

### Synchronous response
A frame transmission in both master-slave and multi-master modes can be optionally followed by a synchronous response of its recipient, all devices must use the same response time-out to avoid collisions. The acknowledgment reception phase must be shorter than the response time-out to be successful.
```cpp  
Transmission                                    Response
 _______  ______  ______  _____                   _____
| START || BYTE || BYTE || END | CRC COMPUTATION | ACK |
|-------||------||------||-----|-----------------|-----|
|  149  ||  H   ||  I   || 234 | LATENCY         |  6  |
|_______||______||______||_____|                 |_____|
```
The required response time-out for a given application can be determined practically transmitting the longest supported frame with the farthest physical distance between the two devices. The highest interval between packet transmission and acknowledgement measured plus a small margin is the correct time-out that should exclude acknowledgement losses.
