- PJON Protocol specification:
[v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md) - [v0.2](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md) - [v0.3](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md) - [v1.0](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md)
- Acknowledge specification: [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Dynamic addressing specification: **[v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md)**

```cpp
/*
Milan, Italy - 02/10/2016
The PJON™ dynamic addressing specification is an invention and intellectual property
of Giovanni Blu Mitolo - Copyright 2010-2017 All rights reserved

Related work: https://github.com/gioblu/PJON/
Compliant implementation versions: PJON 5.2 and following

Inspired by the thesis of Thomas Snaidero:
"Modular components for eye tracking, in the interest of helping persons with severely impaired motor skills."
Master Thesis, IT University of Copenhagen, Denmark, September 2016
*/
```

###PJON™ dynamic addressing specification v0.1
This draft defines the dynamic addressing procedure used by a device in multi-master configuration or the one imposed and regulated by the master in a master-slave configuration.

###Master-slave dynamic addressing
```cpp  
  SLAVES
   _________     _________     _________     _________
  | ID    1 |   | ID    2 |   | ID    3 |   | ID    4 |     
  | RID 101 |   | RID  41 |   | RID 945 |   | RID  22 |      __________
  |_________|   |_________|   |_________|   |_________|     |  MASTER  |
______|_____________|_____________|_____________|___________| ID   254 |
      ___|_____     ___|_____     ___|_____     ___|_____   |__________|
     | ID    5 |   | ID    6 |   | ID    7 |   | ID    8 |             
     | RID 723 |   | RID  35 |   | RID 585 |   | RID  66 |               
     |_________|   |_________|   |_________|   |_________|                 
```

####Master features
* The master id is `PJON_MASTER_ID` (value 254)
* Master has a caducous internal device archive
* Broadcasts `ID_LIST` to get `ID_REFRESH` requests from already approved devices
* Handles `ID_REQUEST` requests from devices asking for device id assignment
* Sends `ID_NEGATE` request to colliding or inconsistent devices
* Handles `ID_NEGATE` requests from slaves who are leaving the bus  

####Slave features
* The slave initial device id is `NOT_ASSIGNED` (value 255)
* Slaves have a unique random generated 4 bytes id or rid
* Sends `ID_REFRESH` request to master if required by master `ID_LIST` broadcast
* Sends `ID_REQUEST` to master if device id assignment is necessary
* Regenerates rid and restarts the process if `ID_NEGATE` is received from master
* Sends `ID_NEGATE` before shut down / leaving the bus
* Fall back to multi-master procedure if no master is present

####Procedure
Slave sends an `ID_REQUEST` to get a new device id:
```cpp  
  ___________ __________ ________ ____________ _______ _______ _______ _______ _____     _____
 |           |  HEADER  |        |            |       |       |       |       |     |   |     |
>| MASTER_ID | 00010100 | LENGTH | ID_REQUEST | RID 1 | RID 2 | RID 3 | RID 4 | CRC |> <| ACK |
 |___________|__________|________|____________|_______|_______|_______|_______|_____|   |_____|
```
If master detects a device rid collision, sends a `ID_NEGATE` request to `NOT_ASSIGNED` device id to force
the collided device still not approved to regenerate a device rid:
```cpp  
  ______________ __________ ________ ___________ _______ _______ _______ _______ _____     _____
 |              |  HEADER  |        |           |       |       |       |       |     |   |     |
>| NOT_ASSIGNED | 00010100 | LENGTH | ID_NEGATE | RID 1 | RID 2 | RID 3 | RID 4 | CRC |> <| ACK |
 |______________|__________|________|___________|_______|_______|_______|_______|_____|   |_____|
```  
Master broadcasts response containing the new id reserved for the device rid who requested:
```cpp  
  ___________ __________ ________ ____________ _______ _______ _______ _______ ____ _____
 |           |  HEADER  |        |            |       |       |       |       |    |     |
>| BROADCAST | 00010000 | LENGTH | ID_REQUEST | RID 1 | RID 2 | RID 3 | RID 4 | ID | CRC |>
 |___________|__________|________|____________|_______|_______|_______|_______|____|_____|
```
Slave device id acquisition confirmation:
```cpp  
  ___________ __________ ________ ____________ _______ _______ _______ _______ ____ _____     _____
 |           |  HEADER  |        |            |       |       |       |       |    |     |   |     |
>| MASTER_ID | 00010100 | LENGTH | ID_CONFIRM | RID 1 | RID 2 | RID 3 | RID 4 | ID | CRC |> <| ACK |
 |___________|__________|________|____________|_______|_______|_______|_______|____|_____|   |_____|
```
If master detects reference inconsistencies at this stage, sends a `ID_NEGATE` request to the slave id
to force the device requesting `ID_CONFIRM` to regenerate a rid and try again:
```cpp  
  ____ __________ ________ ___________ _______ _______ _______ _______ _____     _____
 |    |  HEADER  |        |           |       |       |       |       |     |   |     |
>| ID | 00010100 | LENGTH | ID_NEGATE | RID 1 | RID 2 | RID 3 | RID 4 | CRC |> <| ACK |
 |____|__________|________|___________|_______|_______|_______|_______|_____|   |_____|
```
If master experience temporary disconnection / malfunction on startup sends a `ID_LIST` broadcast request.
Slaves will receive the broadcast and dispatch an `ID_REFRESH` request for the master:

Master broadcast `ID_LIST` request:
```cpp  
  ___________ __________ ________ _________ _____
 |           |  HEADER  |        |         |     |
>| BROADCAST | 00010000 | LENGTH | ID_LIST | CRC |>
 |___________|__________|________|_________|_____|
```
Slave device `ID_REFRESH` request:
```cpp  
  ___________ __________ ________ ____________ _______ _______ _______ _______ ____ _____     _____
 |           |  HEADER  |        |            |       |       |       |       |    |     |   |     |
>| MASTER_ID | 00010100 | LENGTH | ID_REFRESH | RID 1 | RID 2 | RID 3 | RID 4 | ID | CRC |> <| ACK |
 |___________|__________|________|____________|_______|_______|_______|_______|____|_____|   |_____|
```
If the id requested by the slave is free in the reference, id is approved by the master and the exchange ends.
If the id is found already in use, the master sends an `ID_NEGATE` request forcing the slave to
acquire a new id through an `ID_REQUEST`:

Master sends `ID_NEGATE` request to the slave:
```cpp  
  _______ __________ ________ ___________ _______ _______ _______ _______ _____     _____
 | SLAVE |  HEADER  |        |           |       |       |       |       |     |   |     |
>|  ID   | 00010100 | LENGTH | ID_NEGATE | RID 1 | RID 2 | RID 3 | RID 4 | CRC |> <| ACK |
 |_______|__________|________|___________|_______|_______|_______|_______|_____|   |_____|
```
If slave wants to leave the bus must send a `ID_NEGATE` request to the master:
```cpp  
  ___________ __________ ________ ___________ _______ _______ _______ _______ ____ _____     _____
 |           |  HEADER  |        |           |       |       |       |       |    |     |   |     |
>| MASTER_ID | 00010100 | LENGTH | ID_NEGATE | RID 1 | RID 2 | RID 3 | RID 4 | ID | CRC |> <| ACK |
 |___________|__________|________|___________|_______|_______|_______|_______|____|_____|   |_____|
```

###Multi-master dynamic addressing
```cpp  
    _______     _______     _______     _______     _______
   |       |   |       |   |       |   |       |   |       |  
   | ID 0  |   | ID 1  |   | ID 2  |   | ID 3  |   | ID 4  |  
   |_______|   |_______|   |_______|   |_______|   |_______|    
 ______|___________|___________|___________|___________|______
          ___|___     ___|___     ___|___     ___|___
         |       |   |       |   |       |   |       |   
         | ID 5  |   | ID 6  |   | ID 7  |   | ID 8  |
         |_______|   |_______|   |_______|   |_______|    
```

####Procedure
In a multi-master scenario, the device actively looks for a free device id and make no use of its rid for this procedure:

1. The device extracts a random device id and tries to contact that device
2. If an answer is received, it adds one to the id and tries again
3. If any answer is obtained from a device id, that is reserved
4. The device receives for a random time to be able to answer to other devices interested in that device id
5. The device tries to contact itself to probe collision, if no answer is received the device id is taken.
