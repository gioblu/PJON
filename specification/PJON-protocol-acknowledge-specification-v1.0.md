#### Network layer
- PJON (Padded Jittering Operative Network) Protocol specification: [v3.0](/specification/PJON-protocol-specification-v3.0.md)
- Acknowledge specification: **[v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)**
- Dynamic addressing specification: [v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- PJON known protocols: [list](/specification/PJON-known-protocols-list.md)
#### Data link layer
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- SFSP (Secure Frame Separation Protocol) specification: [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

```cpp
/*
Milan, Italy
Originally published: 17/10/2016
latest revision: 15/10/2017
PJON™ protocol acknowledge specification v1.0
Invented by Giovanni Blu Mitolo,
released into the public domain

Related implementation: https://github.com/gioblu/PJON/
Compliant versions: PJON v9.0 and following
*/
```
### PJON™ protocol acknowledge specification v1.0
The PJON Standard supports both **synchronous** and **asynchronous** acknowledgement.
This two mechanisms are defined to ensure that a packet transmission ended positively with no errors and can be used individually or together.

#### Synchronous acknowledge
```cpp
Channel analysis       Transmission                 Response
 _____  ____________________________________________  _____
| C-A || ID |  HEADER  | LENGTH |CRC8| CONTENT |CRC8|| ACK |
|-----||----|----------|--------|----|---------|----||-----|
|  0  || 12 | 00000100 |   6    |    |    64   |    ||  6  |
|_____||____|__________|________|____|_________|____||_____|
```

The graph above contains a standard packet transmission with synchronous acknowledge request where the character `@` or `64` is sent to device id `12` with `00000100` header. As defined by the [PJON protocol layer specification v3.0](/specification/PJON-protocol-specification-v3.0.md) the third bit from right up in the header requests to transmitter a synchronous acknowledge response. How the synchronous acknowledgement procedure works depends on the medium and the strategy used, see [PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md), [PJDLR v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md), [PJDLS v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) and [TSDL v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md) specification.

#### Asynchronous acknowledge

```cpp
Channel analysis               Transmission                Response
 ____________________________________________________________  ___
|ID| HEADER |LENGTH|CRC|BUS ID|BUS ID|ID|PACKET ID|DATA|CRC32||ACK|
|--|--------|------|---|------|------|--|---------|----|-----||---|
|12|10001111|  20  |   | 0002 | 0001 |11|   99    | 64 |     || 6 |
|__|________|______|___|______|______|__|_________|____|_____||___|
                       |RXINFO| TX INFO |       
```
The graph above contains a standard packet transmission with asynchronous acknowledge request where the character `@` or `64` is sent to device id `12` with `0001111` header containing its packet id `99`. As defined by the [PJON protocol specification v3.0](/specification/PJON-protocol-specification-v3.0.md) the fourth bit from right up in the header requests to transmitter an asynchronous acknowledgment response and the presence of the packet id. The second bit from right up signals the inclusion of the sender's info necessary to send back an asynchronous acknowledgment packet when received.

#### PJON™ recursive acknowledgement pattern
In a scenario where there is no direct communication between two devices, a synchronous acknowledgement can't be obtained successfully, so an asynchronous acknowledgement packet has to be sent back from receiver to the packet's transmitter to inform of the correct packet reception.

```cpp
 BUS 0.0.0.1                      BUS 0.0.0.2
 ______             ______             ______
|      |           |      |           |      |
| ID 0 |___________|ROUTER|___________| ID 0 |
|______|           |______|           |______|
```
A router in the center is connected with two different buses, bus `0.0.0.1` and `0.0.0.2`, communication between device `0` of bus `0.0.0.1` with device `0` of bus `0.0.0.2` can be obtain only through the router.

```cpp  
Transmission                                             Response
 __________________________________________________________  ___
|ID| HEADER |LENGTH|CRC|BUS ID|BUS ID|ID|PACKET ID|DATA|CRC||ACK|
|--|--------|------|---|------|------|--|---------|----|---||---|
|0 |10001111|  20  |   | 0002 | 0001 |0 |   99    | 64 |   || 6 |
|__|________|______|___|______|______|__|_________|____|___||___|
                       |RXINFO| TX INFO |         
```
In the packet shown above device `0` of bus `0.0.0.1` sends `@` (64) to device `0` of bus `0.0.0.2`. Being the header `10001111` and so containing the `PACKET ID` and `ACK MODE` bit up (asynchronous acknowledgement request) the packet is formatted containing the 2 bytes integer packet id `99` (used by receiver to send back an asynchronous acknowledgement packet) immediately after the sender information. Being header's `00000100` bit up (synchronous acknowledgement request) receiver will acknowledge synchronously with an `PJON_ACK` or 6 in case of correct reception. This precise case is used as an example to show both features used at the same time to obtain an efficient and secure way to transmit packets with correct transmission certainty.

```cpp        
BUS 0.0.0.1                                          BUS 0.0.0.2

1 Packet send - 2 sync ack, forward - 3 sync ack, async ack send
 ______                      ______                      ______
|      |                    |      |                    |      |
| ID 0 |____________________|ROUTER|____________________| ID 0 |
|______|                    |______|                    |______|
   5 sync ack                   4 ack, async ack forward

/* If packet length - its overhead is 2, it is an asynchronous
   acknowledgement packet containing only its packet id */
```
1) Device `0` sends the packet, the router has a route to device `0` of bus `0.0.0.2` so responds with a synchronous acknowledgement
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ____ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|DATA|CRC||ACK|
|0 |10001111|  20  |   |0.0.0.2|0.0.0.1|0 |   99    | 64 |   || 6 |
|__|________|______|___|_______|_______|__|_________|____|___||___|
                       |RX INFO| TX INFO  |
```
2) Device `0` of bus `0.0.0.1` wait for an asynchronous acknowledgement of the packet sent. Router sends to device id `0` of bus `0.0.0.2` and receives a synchronous acknowledgement
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ____ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|DATA|CRC||ACK|
|0 |10001111|  20  |   |0.0.0.2|0.0.0.1|0 |   99    | 64 |   || 6 |
|__|________|______|___|_______|_______|__|_________|____|___||___|
                       |RX INFO| TX INFO  |
```
3) Device `0` of bus `0.0.0.2` sends an asynchronous acknowledgement packet to device `0` of bus `0.0.0.1`. Router has a route to device `0` of bus `0.0.0.1` so responds with a synchronous acknowledgement and device `0` of bus `0.0.0.2` ends the transaction after receiving a synchronous acknowledgement by the router
```cpp
 __ ________ ______ ___ _______ _______ __ _________ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|CRC||ACK|
|0 |10001111|  19  |   |0.0.0.1|0.0.0.2|0 |   99    |   || 6 |
|__|________|______|___|_______|_______|__|_________|___||___|
                       |RX INFO| TX INFO  |
```
4) Device `0` of bus `0.0.0.1` receives the asynchronous acknowledgement packet forwarded by the router and responds with a synchronous acknowledgement.

```cpp
 __ ________ ______ ___ _______ _______ __ _________ ___  ___
|ID| HEADER |LENGTH|CRC|BUS ID |BUS ID |ID|PACKET ID|CRC||ACK|
|0 |10001111|  19  |   |0.0.0.1|0.0.0.2|0 |   99    |   || 6 |
|__|________|______|___|_______|_______|__|_________|___||___|
                       |RX INFO| TX INFO  |
```
This documents is not intended to specify the routing procedure but uses routing as a necessary example to showcase clearly the power of the recursive acknowledgement pattern.
