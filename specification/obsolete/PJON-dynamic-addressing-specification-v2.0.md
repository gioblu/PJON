
## PJON® dynamic addressing v2.0
```
Invented by Giovanni Blu Mitolo
Originally published: 02/10/2016, latest revision: 31/10/2018
Related implementation: https://github.com/gioblu/PJON/
Compliant versions: PJON v9.0 and following
Released into the public domain
```
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
* Master's id is `PJON_MASTER_ID` (decimal 254)
* Master has a caducous internal slave ids archive
* Broadcasts `PJON_ID_LIST` to get `PJON_ID_REFRESH` requests from already approved slaves
* Handles `PJON_ID_REQUEST` requests from slaves requiring id assignment
* Sends `PJON_ID_NEGATE` request to slaves if identification collision occurs
* Handles `PJON_ID_NEGATE` requests from slaves who are leaving the bus  

#### Slave features
* Slave's initial id is `PJON_NOT_ASSIGNED` (decimal 255)
* Slaves have a unique random generated 4 bytes rid or randomly generated identifier
* Sends `PJON_ID_REFRESH` request to master if required by master `PJON_ID_LIST` broadcast
* Sends `PJON_ID_REQUEST` to master if id assignment is required
* Regenerates rid and restarts the process if `PJON_ID_NEGATE` is received from master
* Sends `PJON_ID_NEGATE` before shut down or leaving the bus
* Fall back to multi-master procedure if no master is present

#### Procedure
All communication to dynamically assign or request ids must be transmitted using CRC32 on the `PJON_DYNAMIC_ADDRESSING` port (see the [network services list v1.0](/specification/PJON-network-services-list.md)).

Slave sends a `PJON_ID_REQUEST` to get a new id:
```cpp  
 ______ ________ ______ ___ ________ ____ __________ ___ ___  ___
|MASTER| HEADER |      |   |  NOT   |PORT|          |RID|   ||   |
|  ID  |00110110|LENGTH|CRC|ASSIGNED| 1  |ID_REQUEST|   |CRC||ACK|
|______|________|______|___|________|____|__________|___|___||___|
```
If master detects a rid collision, sends a `PJON_ID_NEGATE` request to `PJON_NOT_ASSIGNED` device id to force the still not approved slave to regenerate a rid:
```cpp  
 ________ ________ ______ ___ ______ ____ _________ ___ ___  ___
|  NOT   | HEADER |      |   |MASTER|PORT|         |RID|   ||   |
|ASSIGNED|00110110|LENGTH|CRC|  ID  | 1  |ID_NEGATE|   |CRC||ACK|
|________|________|______|___|______|____|_________|___|___||___|
```  
Master broadcasts a response containing the rid and the new device id reserved for the requester:
```cpp  
 _________ ________ ______ ___ ______ ____ __________ ___ __ ___
|         | HEADER |      |   |MASTER|PORT|          |RID|  |   |
|BROADCAST|00110010|LENGTH|CRC|  ID  | 1  |ID_REQUEST|   |ID|CRC|
|_________|________|______|___|______|____|__________|___|__|___|
```
Slave confirms the id acquisition:
```cpp  
 ______ ________ ______ ___ __ ____ __________ ___ __ ___  ___
|MASTER| HEADER |      |   |  |PORT|          |RID|  |   ||   |
|  ID  |00110110|LENGTH|CRC|ID| 1  |ID_CONFIRM|   |ID|CRC||ACK|
|______|________|______|___|__|____|__________|___|__|___||___|
```
If master detects a slave's rid already present in its reference sends a `PJON_ID_NEGATE` request to the slave who transmitted the `ID_CONFIRM` request to force it to regenerate a rid and try again:
```cpp  
 __ ________ ______ ___ _________ ____ _________ ___ ___  ___
|  | HEADER |      |   |         |PORT|         |RID|   ||   |
|ID|00110110|LENGTH|CRC|MASTER_ID| 1  |ID_NEGATE|   |CRC||ACK|
|__|________|______|___|_________|____|_________|___|___||___|
```
If master experiences temporary disconnection or reboot, at start up sends a `PJON_ID_LIST` broadcast request:
```cpp  
 _________ ________ ______ ___ _________ ____ _______ ___
|         | HEADER |      |   |         |PORT|       |   |
|BROADCAST|00110010|LENGTH|CRC|MASTER_ID| 1  |ID_LIST|CRC|
|_________|________|______|___|_________|____|_______|___|
```
Each slave answers to a `PJON_ID_LIST` broadcast request dispatching a `PJON_ID_REFRESH` request for master:
```cpp  
 ______ ________ ______ ___ __ ____ __________ ___ __ ___  ___
|MASTER| HEADER |      |   |  |PORT|          |RID|  |   ||   |
|  ID  |00110110|LENGTH|CRC|ID| 1  |ID_REFRESH|   |ID|CRC||ACK|
|______|________|______|___|__|____|__________|___|__|___||___|
```
If the id requested is free in the master's reference, id is approved and the exchange ends.
If the id is already in use, master sends a `PJON_ID_NEGATE` request forcing the slave to
acquire a new id through a `PJON_ID_REQUEST`:

Master sends `PJON_ID_NEGATE` request to slave:
```cpp  
 _____ ________ ______ ___ _________ ____ _________ ___ ___  ___
|SLAVE| HEADER |      |   |         |PORT|         |RID|   ||   |
| ID  |00110110|LENGTH|CRC|MASTER_ID| 1  |ID_NEGATE|   |CRC||ACK|
|_____|________|______|___|_________|____|_________|___|___||___|
```
Slaves must send a `PJON_ID_NEGATE` request to the master to free the id before leaving the bus:
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
In a multi-master setup, all devices are actively looking for a free device id:

1. The device extracts a random device id and tries to contact that device
2. If an answer is received, it adds one to the id and tries again
3. If no answer is obtained from a device id, that is reserved
4. The device receives for a random time to be able to answer to other devices interested in that device id
5. The device tries to contact itself to probe collision, if no answer is received the device id is taken.
