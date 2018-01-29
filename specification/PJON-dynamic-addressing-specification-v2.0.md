#### Network layer
- PJON (Padded Jittering Operative Network) Protocol specification: [v3.0](/specification/PJON-protocol-specification-v3.0.md)
- Acknowledge specification: [v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- Dynamic addressing specification: **[v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)**
- PJON known protocols: [list](/specification/PJON-known-protocols-list.md)
#### Data link layer
- PJDL (Padded Jittering Data Link) specification:
[PJDL v2.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v2.0](/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- TSDL (Tardy Serial Data Link) specification: [TSDL v2.0](/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- SFSP (Secure Frame Separation Protocol) specification: [SFSP v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

```cpp
/*
Milan, Italy
Originally published: 02/10/2016
Latest revision: 14/12/2017
PJON™ dynamic addressing specification v2.0
Invented by Giovanni Blu Mitolo,
released into the public domain

Related implementation: https://github.com/gioblu/PJON/
Compliant versions: PJON v9.0 and following

Inspired by the thesis of Thomas Snaidero:
"Modular components for eye tracking, in the interest of
helping persons with severely impaired motor skills."
Master Thesis, IT University of Copenhagen
Denmark, September 2016

Changelog:
- Switched from PJON_ADDRESS_BIT to port identifier
*/
```

### PJON™ dynamic addressing specification v2.0
This document defines the dynamic addressing procedure used by a device in multi-master configuration or the one imposed and regulated by master in a master-slave configuration. All communication related to the addressing procedure must be transmitted on the `PJON_DYNAMIC_ADDRESSING_PORT` or port `1`.

### Master-slave dynamic addressing
```cpp  
 _________    _________    _________    _________
| ID    1 |  | ID    2 |  | ID    3 |  | ID    4 |
| RID 101 |  | RID  41 |  | RID 945 |  | RID  22 |  __________
|_________|  |_________|  |_________|  |_________| |  MASTER  |
____|____________|____________|____________|_______| ID   254 |
 ___|_____    ___|_____    ___|_____    ___|_____  |__________|
| ID    5 |  | ID    6 |  | ID    7 |  | ID    8 |
| RID 723 |  | RID  35 |  | RID 585 |  | RID  66 |
|_________|  |_________|  |_________|  |_________|
```

#### Master features
* Master's id is `PJON_MASTER_ID` (value 254)
* Master has a caducous internal device archive
* Broadcasts `PJON_ID_LIST` to get `PJON_ID_REFRESH` requests from already approved devices
* Handles `PJON_ID_REQUEST` requests from devices asking for device id assignment
* Sends `PJON_ID_NEGATE` request to colliding or inconsistent devices
* Handles `PJON_ID_NEGATE` requests from slaves who are leaving the bus  

#### Slave features
* Slave's initial device id is `PJON_NOT_ASSIGNED` (value 255)
* Slaves have a unique random generated 4 bytes id or rid
* Sends `PJON_ID_REFRESH` request to master if required by master `PJON_ID_LIST` broadcast
* Sends `PJON_ID_REQUEST` to master if device id assignment is necessary
* Regenerates rid and restarts the process if `PJON_ID_NEGATE` is received from master
* Sends `PJON_ID_NEGATE` before shut down / leaving the bus
* Fall back to multi-master procedure if no master is present

#### Procedure
All communication to dynamically assign or request ids must be transmitted using CRC32 and `PJON_DYNAMIC_ADDRESSING` port (see the [known protocols list v1.0](/strategies/ThroughSerial/specification/PJON-known-protocols-list-v1.0.md)).

Slave sends a `PJON_ID_REQUEST` to get a new device id:
```cpp  
 ______ ________ ______ ___ ________ ____ __________ ___ ___  ___
|MASTER| HEADER |      |   |  NOT   |PORT|          |RID|   ||   |
|  ID  |00110110|LENGTH|CRC|ASSIGNED| 1  |ID_REQUEST|   |CRC||ACK|
|______|________|______|___|________|____|__________|___|___||___|
```
If master detects a device rid collision, sends a `PJON_ID_NEGATE` request to `PJON_NOT_ASSIGNED` device id to force
the collided device still not approved to regenerate a device rid:
```cpp  
 ________ ________ ______ ___ ______ ____ _________ ___ ___  ___
|  NOT   | HEADER |      |   |MASTER|PORT|         |RID|   ||   |
|ASSIGNED|00110110|LENGTH|CRC|  ID  | 1  |ID_NEGATE|   |CRC||ACK|
|________|________|______|___|______|____|_________|___|___||___|
```  
Master broadcasts response containing the new id reserved for the device rid who requested:
```cpp  
 _________ ________ ______ ___ ______ ____ __________ ___ __ ___
|         | HEADER |      |   |MASTER|PORT|          |RID|  |   |
|BROADCAST|00110010|LENGTH|CRC|  ID  | 1  |ID_REQUEST|   |ID|CRC|
|_________|________|______|___|______|____|__________|___|__|___|
```
Slave device id acquisition confirmation:
```cpp  
 ______ ________ ______ ___ __ ____ __________ ___ __ ___  ___
|MASTER| HEADER |      |   |  |PORT|          |RID|  |   ||   |
|  ID  |00110110|LENGTH|CRC|ID| 1  |ID_CONFIRM|   |ID|CRC||ACK|
|______|________|______|___|__|____|__________|___|__|___||___|
```
If master detects a requester's rid already present in its reference, sends a `PJON_ID_NEGATE` request to the slave id requesting `ID_CONFIRM` to force it to regenerate a rid and try again:
```cpp  
 __ ________ ______ ___ _________ ____ _________ ___ ___  ___
|  | HEADER |      |   |         |PORT|         |RID|   ||   |
|ID|00110110|LENGTH|CRC|MASTER_ID| 1  |ID_NEGATE|   |CRC||ACK|
|__|________|______|___|_________|____|_________|___|___||___|
```
If master experiences temporary disconnection or reboot, on startup sends a `PJON_ID_LIST` broadcast request:
```cpp  
 _________ ________ ______ ___ _________ ____ _______ ___
|         | HEADER |      |   |         |PORT|       |   |
|BROADCAST|00110010|LENGTH|CRC|MASTER_ID| 1  |ID_LIST|CRC|
|_________|________|______|___|_________|____|_______|___|
```
Slaves receives and dispatches a `PJON_ID_REFRESH` request for master:
```cpp  
 ______ ________ ______ ___ __ ____ __________ ___ __ ___  ___
|MASTER| HEADER |      |   |  |PORT|          |RID|  |   ||   |
|  ID  |00110110|LENGTH|CRC|ID| 1  |ID_REFRESH|   |ID|CRC||ACK|
|______|________|______|___|__|____|__________|___|__|___||___|
```
If the id requested by the slave is free in master's reference, id is approved and the exchange ends.
If the id is found already in use, master sends a `PJON_ID_NEGATE` request forcing the slave to
acquire a new id through a `PJON_ID_REQUEST`:

Master sends `PJON_ID_NEGATE` request to slave:
```cpp  
 _____ ________ ______ ___ _________ ____ _________ ___ ___  ___
|SLAVE| HEADER |      |   |         |PORT|         |RID|   ||   |
| ID  |00110110|LENGTH|CRC|MASTER_ID| 1  |ID_NEGATE|   |CRC||ACK|
|_____|________|______|___|_________|____|_________|___|___||___|
```
If slave disconnect from the bus must send a `PJON_ID_NEGATE` request to the master:
```cpp  
 ______ ________ ______ ___ __ ____ _________ ___ __ ___  ___
|MASTER| HEADER |      |   |  |PORT|         |RID|  |   ||   |
|  ID  |00110110|LENGTH|CRC|ID| 1  |ID_NEGATE|   |ID|CRC||ACK|
|______|________|______|___|__|____|_________|___|__|___||___|
```

### Multi-master dynamic addressing
```cpp  
 _______     _______     _______     _______     _______
|       |   |       |   |       |   |       |   |       |
| ID 0  |   | ID 1  |   | ID 2  |   | ID 3  |   | ID 4  |
|_______|   |_______|   |_______|   |_______|   |_______|
_____|___________|___________|___________|___________|____
     ___|___     ___|___     ___|___     ___|___
    |       |   |       |   |       |   |       |
    | ID 5  |   | ID 6  |   | ID 7  |   | ID 8  |
    |_______|   |_______|   |_______|   |_______|   
```

#### Procedure
In a multi-master setup, all devices, are actively looking for a free device id:

1. The device extracts a random device id and tries to contact that device
2. If an answer is received, it adds one to the id and tries again
3. If no answer is obtained from a device id, that is reserved
4. The device receives for a random time to be able to answer to other devices interested in that device id
5. The device tries to contact itself to probe collision, if no answer is received the device id is taken.
