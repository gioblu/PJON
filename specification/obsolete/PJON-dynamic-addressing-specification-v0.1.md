
```cpp
/*
Milan, Italy - 02/10/2016
The PJON® dynamic addressing specification is an invention
and intellectual property of Giovanni Blu Mitolo
Copyright 2010-2025 All rights reserved

Related work: https://github.com/gioblu/PJON/
Compliant implementation versions: PJON 5.2 and following

Inspired by the thesis of Thomas Snaidero:
"Modular components for eye tracking, in the interest of
helping persons with severely impaired motor skills."
Master Thesis, IT University of Copenhagen
Denmark, September 2016
*/
```

### PJON® dynamic addressing specification v0.1
This draft defines the dynamic addressing procedure used by a device in multi-master configuration or the one imposed and regulated by master in a master-slave configuration.

### Master-slave dynamic addressing
```cpp  
 _________     _________     _________     _________
| ID    1 |   | ID    2 |   | ID    3 |   | ID    4 |
| RID 101 |   | RID  41 |   | RID 945 |   | RID  22 |  __________
|_________|   |_________|   |_________|   |_________| |  MASTER  |
____|_____________|_____________|_____________|_______| ID   254 |
 ___|_____     ___|_____     ___|_____     ___|_____  |__________|
| ID    5 |   | ID    6 |   | ID    7 |   | ID    8 |
| RID 723 |   | RID  35 |   | RID 585 |   | RID  66 |
|_________|   |_________|   |_________|   |_________|
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
Slave sends a `PJON_ID_REQUEST` to get a new device id:
```cpp  
 _________ ________ ______ __________ ___ ___ ___ ___ ___  ___
|         | HEADER |      |          |RID|RID|RID|RID|   ||   |
|MASTER_ID|00010100|LENGTH|ID_REQUEST| 1 | 2 | 3 | 4 |CRC||ACK|
|_________|________|______|__________|___|___|___|___|___||___|
```
If master detects a device rid collision, sends a `PJON_ID_NEGATE` request to `PJON_NOT_ASSIGNED` device id to force
the collided device still not approved to regenerate a device rid:
```cpp  
 ____________ ________ ______ _________ ___ ___ ___ ___ ___  ___
|            | HEADER |      |         |RID|RID|RID|RID|   ||   |
|NOT_ASSIGNED|00010100|LENGTH|ID_NEGATE| 1 | 2 | 3 | 4 |CRC||ACK|
|____________|________|______|_________|___|___|___|___|___||___|
```  
Master broadcasts response containing the new id reserved for the device rid who requested:
```cpp  
 _________ ________ ______ __________ ___ ___ ___ ___ __ ___
|         | HEADER |      |          |RID|RID|RID|RID|  |   |
|BROADCAST|00010000|LENGTH|ID_REQUEST| 1 | 2 | 3 | 4 |ID|CRC|
|_________|________|______|__________|___|___|___|___|__|___|
```
Slave device id acquisition confirmation:
```cpp  
 _________ ________ ______ __________ ___ ___ ___ ___ __ ___  ___
|         | HEADER |      |          |RID|RID|RID|RID|  |   ||   |
|MASTER_ID|00010100|LENGTH|ID_CONFIRM| 1 | 2 | 3 | 4 |ID|CRC||ACK|
|_________|________|______|__________|___|___|___|___|__|___||___|
```
If master detects reference a requester's rid already present in its reference, sends a `PJON_ID_NEGATE` request to the slave id requesting `ID_CONFIRM` to force it to regenerate a rid and try again:
```cpp  
 __ ________ ______ _________ ___ ___ ___ ___ ___  ___
|  | HEADER |      |         |RID|RID|RID|RID|   ||   |
|ID|00010100|LENGTH|ID_NEGATE| 1 | 2 | 3 | 4 |CRC||ACK|
|__|________|______|_________|___|___|___|___|___||___|
```
If master experiences temporary disconnection or reboot, on startup sends a `PJON_ID_LIST` broadcast request.
Slaves will receive the broadcast and dispatch a `PJON_ID_REFRESH` request for master:

Master broadcast `PJON_ID_LIST` request:
```cpp  
 _________ ________ ______ _______ ___
|         | HEADER |      |       |   |
|BROADCAST|00010000|LENGTH|ID_LIST|CRC|
|_________|________|______|_______|___|
```
Slave device `PJON_ID_REFRESH` request:
```cpp  
 _________ ________ ______ __________ ___ ___ ___ ___ __ ___  ___
|         | HEADER |      |          |RID|RID|RID|RID|  |   ||   |
|MASTER_ID|00010100|LENGTH|ID_REFRESH| 1 | 2 | 3 | 4 |ID|CRC||ACK|
|_________|________|______|__________|___|___|___|___|__|___||___|
```
If the id requested by the slave is free in master's reference, id is approved and the exchange ends.
If the id is found already in use, master sends a `PJON_ID_NEGATE` request forcing the slave to
acquire a new id through a `PJON_ID_REQUEST`:

Master sends `PJON_ID_NEGATE` request to slave:
```cpp  
 _____ ________ ______ _________ ___ ___ ___ ___ ___  ___
|SLAVE| HEADER |      |         |RID|RID|RID|RID|   ||   |
| ID  |00010100|LENGTH|ID_NEGATE| 1 | 2 | 3 | 4 |CRC||ACK|
|_____|________|______|_________|___|___|___|___|___||___|
```
If slave disconnect from the bus must send a `PJON_ID_NEGATE` request to the master:
```cpp  
 _________ ________ ______ _________ ___ ___ ___ ___ __ ___  ___
|         | HEADER |      |         |RID|RID|RID|RID|  |   ||   |
|MASTER_ID|00010100|LENGTH|ID_NEGATE| 1 | 2 | 3 | 4 |ID|CRC||ACK|
|_________|________|______|_________|___|___|___|___|__|___||___|
```

### Multi-master dynamic addressing
```cpp  
 _______     _______     _______     _______     _______
|       |   |       |   |       |   |       |   |       |
| ID 0  |   | ID 1  |   | ID 2  |   | ID 3  |   | ID 4  |
|_______|   |_______|   |_______|   |_______|   |_______|
_____|___________|___________|___________|___________|_____
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
