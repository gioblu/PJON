
### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v3.0](/specification/PJON-protocol-specification-v3.0.md)
- [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [Dynamic addressing specification v2.0](/specification/PJON-dynamic-addressing-specification-v2.0.md)
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md)
- **[PJDLR (Padded Jittering Data Link over Radio) v2.0](/src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md)**
- [PJDLS (Padded Jittering Data Link Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJDLR v2.0
```
Invented by Giovanni Blu Mitolo, preamble feature proposed by Fred Larsen, released into the public domain
Originally published: 10/04/2010, latest revision: 31/10/2018
Related implementation: /src/strategies/OverSampling/
Compliant versions: PJON v9.0 and following
```
PJDLR (Padded Jittering Data Link over Radio) is an asynchronous serial data link for low-data-rate applications that supports one or many to many communication optimized to obtain long range and high reliability using ASK, FSK or OOK radio transceivers. PJDLR can be easily implemented on limited microcontrollers with low clock accuracy and can operate directly using one or two input-output pins.

### Communication modes
The proposed communication mode is the result of years of testing and optimization for ASK/FSK radio transceivers and have been selected to be easily supported also by low quality hardware.  

| MODE | Bit timing | Sync bit timing | Pad-data ratio | Speed               |
| ---- | ---------- | --------------- | -------------- | ------------------- |
| 1    | 512        | 328             | 0.64           | 202B/s - 1620Bb     |

Binary timing durations are expressed in microseconds.

### Medium access control
PJDLR specifies a contention based random multiple access method that supports multi-master communication. Collisions can only occur when 2 or more devices start to transmit at the same time because devices can securely detect an ongoing transmission. When a collision occurs it can be detected by the receiver because of synchronization loss. In multi-master operation the maximum data throughput effectively available is 36.8% of the bandwidth (the same as slotted ALOHA). In master-slave operation the maximum data throughput is equal to the bandwidth.

### Byte transmission
PJDLR byte transmission is composed by 10 bits, the first two are called synchronization pad and are used to obtain sampling synchronization. The synchronization pad is composed by a logic 1 padding bit shorter than data bits and a logic 0 data bit. The following 8 data bits contain information in LSB-first (least significant bit first) order.

The reception technique is based on 3 steps:
1. Find a logic 1 which duration is equal or shorter than the expected padding bit duration
2. Synchronize to its falling edge
3. Ensure it is followed by a logic 0 data bit

If this pattern is detected data reception starts, if not, interference, synchronization loss or simply absence of communication is detected.
```cpp  
 _____ ___________________________
| Pad | Byte                      |
|_    |___       ___     _____    |
| |   |   |     |   |   |     |   |
|1| 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|_|___|___|_____|___|___|_____|___|
```

### Frame transmission
Before a frame transmission, the communication medium is analysed, if logic 1 is present communication is detected and collision is avoided, If the medium is free for a duration longer than a byte plus a small random time, frame transmission starts with an optional preamble designed to enable signal gain tuning and a frame initializer composed by 3 consequent synchronization pads followed by data bytes. The presence of the synchronization pad between each byte ensures that also a frame composed of a series of bytes with decimal value 0 can be transmitted safely without risk of collision.

```cpp     
           INITIALIZER  DATA
 _________ ___________ __________ _______________ ______________
|Preamble |Pad|Pad|Pad| Byte     |Pad| Byte      |Pad| Byte     |
|_____    |_  |_  |_  |     __   |_  |      _   _|_  |      _   |
|     |   | | | | | | |    |  |  | | |     | | | | | |     | |  |
|  1  | 0 |1|0|1|0|1|0|0000|11|00|1|0|00000|1|0|1|1|0|00000|1|00|
|_____|___|_|_|_|_|_|_|____|__|__|_|_|_____|_|_|_|_|_|_____|_|__|
```
When a frame is received a low performance microcontroller with an inaccurate clock can correctly synchronize with transmitter during the frame initializer and consequently each byte is received. The frame initializer is detected if 3 synchronization pads occurred and if their duration is coherent with its expected duration. Frame initialization is 100% reliable, false positives can only occur because of externally induced interference.      

### Synchronous response
A frame transmission can be optionally followed by a synchronous response of its recipient.
```cpp  
Transmission                                              Response
 ________ ______  ______  ______                   ________ _____
|PREAMBLE| INIT || BYTE || BYTE | CRC COMPUTATION |PREAMBLE| ACK |
|____    |------||------||------|-----------------|____    |     |
|    |   |      ||      ||      | LATENCY         |    |   |  6  |
|____|___|______||______||______|                 |____|___|_____|
```

The maximum time dedicated to potential acknowledgement reception for a given application is estimated by adding the CRC computation time of the longest supported frame to the maximum latency and to the optional preamble duration.
