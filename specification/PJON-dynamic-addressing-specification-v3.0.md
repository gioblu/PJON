### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v3.1](/specification/PJON-protocol-specification-v3.1.md)
- [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- **[Dynamic addressing specification v3.0](/specification/PJON-dynamic-addressing-specification-v3.0.md)**
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v3.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v3.0.md)
- [PJDLR (Padded Jittering Data Link over Radio) v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md)
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJON® dynamic addressing v3.0
```
Invented by Giovanni Blu Mitolo
Originally published: 26/03/2019
Related implementation: https://github.com/gioblu/PJON/
Compliant versions: PJON v12.0 and following
Released into the public domain
```
Slaves are physically and uniquely identified by a 5 bytes device address able to cover the whole PJON network address space (2^32*254 or 1.090.921.693.180 devices). Masters use unique bus ids to isolate traffic.
```cpp  
 _____________    _____________
| DEVICE ID 1 |  | DEVICE ID 2 |
| D. ADDRESS  |  | D. ADDRESS  |
| 0.0.3.98.1  |  | 0.0.0.12.54 |     ________________
|_____________|  |_____________|    | MASTER ID  254 |
_______|________________|___________| BUS ID 0.0.0.1 |
 ______|______    ______|______     |________________|
| DEVICE ID 3 |  | DEVICE ID 4 |
| D. ADDRESS  |  | D. ADDRESS  |
| 0.0.8.1.1   |  | 0.0.8.121.1 |
|_____________|  |_____________|
```
Regardless of the master or the network addresses used by the network layer, slaves remain uniquely identified by their own device address. In an isolated scenario device addresses can be generated and assigned as required. If the application is connected to a shared medium where collisions with other systems may occur, it is strongly suggested to request a unique device address here: [https://www.pjon.org/get-device-address.php](https://www.pjon.org/get-device-address.php).

#### Procedure
All communication used to request and assign ids is transmitted using CRC32 on the `PJON_DYNAMIC_ADDRESSING_PORT` port (decimal 1). Masters routinely broadcast a `PJON_ID_DISCOVERY` (decimal 200) advertisement to let slaves be aware of their presence.  
```cpp  
 __________________________________________________
|ID| HEADER |LENGTH|CRC8|BUS ID| PORT | DATA |CRC32|
|--|--------|------|----|------|------|------|-----|
|0 |00010001|  14  |    | 0001 |  1   | 200  |     |
|__|________|______|____|______|______|______|_____|   
```

Slaves are initially configured in router mode using device id `PJON_NOT_ASSIGNED` (decimal 255). When a slave receives an advertisement from a reachable master, it can respond with `PJON_ID_REQUEST` (decimal 201) followed by its own 5 bytes unique device address.
```cpp  
 ____________________________________________________________  ___
|ID | HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID |PORT|DATA  |CRC32||ACK|
|---|--------|------|----|------|------|---|----|------|-----||---|
|254|00010111|  25  |    | 0001 | 0001 |255| 1  |201-DA|     || 6 |
|___|________|______|____|______|______|___|____|______|_____||___|
                         |RXINFO| TX INFO  |          

```
Master sends to `PJON_NOT_ASSIGNED` id (decimal 255) a response containing `PJON_ID_REQUEST` (decimal 201), the device address of the requester and the reserved device id:
```cpp  
 _______________________________________________________________ ___
|ID | HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID |PORT|  DATA  |CRC32||ACK|
|---|--------|------|----|------|------|---|----|--------|-----||---|
|255|00010111|  26  |    | 0001 | 0001 |254| 1  |201-DA-1|     || 6 |
|___|________|______|____|______|______|___|____|________|_____||___|
                         |RXINFO| TX INFO  |   
```
Many slaves could be using `PJON_NOT_ASSIGNED` id although only the slave with the device address contained in the packet responds with a synchronous acknowledgement. Then the slave acquires the device id transmitted by the master and confirms the id acquisition sending `PJON_ID_CONFIRM` (decimal 202):
```cpp  
 _______________________________________________________________ ___
|ID | HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID |PORT|  DATA  |CRC32||ACK|
|---|--------|------|----|------|------|---|----|--------|-----||---|
|254|00010111|  26  |    | 0001 | 0001 | 1 | 1  |  202   |     || 6 |
|___|________|______|____|______|______|___|____|________|_____||___|
                         |RXINFO| TX INFO  |   
```
From now on the slave is part of the bus of its master and receives packets only for that particular bus id. Masters can send a packet containing `PJON_ID_NEGATE` (decimal 203) to force a slave to waive its own device id.
```cpp  

 _____________________________________________________________ ___
|ID | HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID |PORT| DATA |CRC32||ACK|
|---|--------|------|----|------|------|---|----|------|-----||---|
|1  |00010111|  20  |    | 0001 | 0001 |254| 1  | 203  |     || 6 |
|___|________|______|____|______|______|___|____|______|_____||___|
                         |RXINFO| TX INFO  |   
```
All slaves must send a packet containing `PJON_ID_NEGATE` (decimal 203) to the master to waive their device id before effectively leaving the bus:
```cpp  
 ____________________________________________________________ ___
|ID | HEADER |LENGTH|CRC8|BUS ID|BUS ID|ID|PORT| DATA |CRC32||ACK|
|---|--------|------|----|------|------|--|----|------|-----||---|
|254|00010111|  25  |    | 0001 | 0001 |1 | 1  |203-DA|     || 6 |
|___|________|______|____|______|______|__|____|______|_____||___|
                         |RXINFO| TX INFO |   
```
