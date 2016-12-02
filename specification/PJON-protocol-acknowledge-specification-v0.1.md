- PJON Protocol specification:
[v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md) - [v0.2](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md) - [v0.3](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md) - [v1.0](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md)
- Acknowledge specification: **[v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md)**
- Dynamic addressing specification: [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md)

```cpp
/*
Milan, Italy - 17/10/2016
The PJON™ protocol acknowledge specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2016 All rights reserved

Related work: https://github.com/gioblu/PJON/
Compliant implementation versions: PJON 6.0 and following
*/
```
###PJON™ protocol acknowledge specification v0.1
The PJON Standard supports both **synchronous** and **asynchronous** acknowledgement.
This two mechanisms are defined to ensure that a packet transmission ended positively with no errors and can be used individually or together.

####Synchronous acknowledge
```cpp  
Channel analysis   Transmission                                     Response
    _____           ________________________________________           _____
   | C-A |         | ID |  HEADER  | LENGTH | CONTENT | CRC |         | ACK |
<--|-----|---< >---|----|----------|--------|---------|-----|--> <----|-----|
   |  0  |         | 12 | 00000100 |   5    |    64   |  72 |         |  6  |
   |_____|         |____|__________|________|_________|_____|         |_____|
```

The graph above contains a standard packet transmission with synchronous acknowledge request where the character `@` or `64` is sent to device id `12` with `00000100` header. As defined by the [PJON protocol layer specification v1.0](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md) the third bit from right up in the header requests to transmitter a synchronous acknowledge response. How the synchronous acknowledgement procedure works depends on the medium and the strategy used (See [Padded Jittering data link layer specification v0.1](https://github.com/gioblu/PJON/blob/master/strategies/SoftwareBitBang/specification/padded-jittering-protocol-specification-v0.1.md)).

####Asynchronous acknowledge

```cpp
Channel analysis                     Transmission
   _____     _________________________________________________________________
  | C-A |   | ID |  HEADER  | LENGTH | SENDER ID | PACKET ID | CONTENT | CRC |
<-|-----|< >|----|----------|--------|-----------|-----------|---------|-----|>
  |  0  |   | 12 | 00001010 |   18   |    11     |    99     |   64    |     |
  |_____|   |____|__________|________|___________|___________|_________|_____|

```
The graph above contains a standard packet transmission with asynchronous acknowledge request where the character `@` or `64` is sent to device id `12` with `0001110` header containing its packet id `99`. As defined by the [PJON protocol layer specification v1.0](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md) the fourth bit from right up in the header requests to transmitter an asynchronous acknowledge response and the presence of the packet id. The second bit from right up signals the inclusion of the sender's info necessary to send back an asynchronous acknowledge packet when received.

####PJON™ recursive acknowledgement pattern
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
Channel analysis                     Transmission                                            Response
 _____     _______________________________________________________________________________     _____
| C-A |   | ID |  HEADER  | LENGTH |   BUS ID   | BUS ID | ID | PACKET ID | CONTENT | CRC |   | ACK |
|-----|< >|----|----------|--------|------------|--------|----|-----------|---------|-----|> <|-----|
|  0  |   | 0  | 00001111 |   18   |    0002    |  0001  | 0  |    99     |   64    |     |   |  6  |
|_____|   |____|__________|________|____________|________|____|___________|_________|_____|   |_____|
                                   |  RX INFO   |   TX INFO   |           
```
In the packet shown above device `0` of bus `0.0.0.1` sends `@` or `64` to device `0` of bus `0.0.0.2` containing its packet id `99` that will be used by receiver to send back an asynchronous acknowledgement packet where header `00001000` bit up requests an asynchronous acknowledgement and `00000100` requests a synchronous acknowledgement. This precise case is used as an example to show both features used at the same time to obtain an efficient and secure way to transmit packets with correct transmission certainty.

```cpp        
BUS 0.0.0.1                                                                            BUS 0.0.0.2

1 Packet tx                         2 rx, sync ACK, packet tx         3 rx, sync ACK, async ACK tx
TX START--->0-00001111-18-0002-0001-0-99-@-CRC-><-ACK->0-00001111-18-0002-0001-0-99-@-CRC-><-ACK-|
 ______                                        ______                                    ______  |
|      |                                      |      |                                  |      | |
| ID 0 |______________________________________|ROUTER|__________________________________| ID 0 | |
|______|                                      |______|                                  |______| |
                                                                                                 |
TX END-------ACK-><-0-00001111-14-0001-0002-0-99-CRC-<-ACK-><-0-00001111-14-0001-0002-0-99-CRC-<-|
                5 rx, sync ACK                4 rx, sync ACK, packet tx

/* If packet length - its overhead is 4, it is an asynchronous acknowledgement packet
   containing only its packet id */
```
- Device `0` sends the packet, the router has a route to device `0` of bus `0.0.0.2` so responds with a synchronous acknowledgement
- Device `0` of bus `0.0.0.1` wait for an asynchronous acknowledgement of the packet sent
- Router sends to device id `0` of bus `0.0.0.2` and receives a synchronous acknowledgement
- Device `0` of bus `0.0.0.2` sends an asynchronous acknowledgement packet to device `0` of bus `0.0.0.1`
- Router has a route to device `0` of bus `0.0.0.1` so responds with a synchronous acknowledgement
- Device `0` of bus `0.0.0.2` ends the transaction after receiving a synchronous acknowledgement by the router
- Device `0` of bus `0.0.0.1` receives the asynchronous acknowledgement packet forwarded by the router and responds with a synchronous acknowledgement
