### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v4.0](/specification/PJON-protocol-specification-v4.0.md)
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v4.1](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.1.md)
- [PJDLR (Padded Jittering Data Link over Radio) v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md)
- **[PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)**
- [TSDL (Tardy Serial Data Link) v3.0](/src/strategies/ThroughSerial/specification/TSDL-specification-v3.0.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJDLS v2.0
```
Invented by Giovanni Blu Mitolo
Originally published: 20/11/2017
Latest revision: 31/10/2018
Related implementation: /src/strategies/AnalogSampling/
Compliant versions: PJON v10.0 and following
Released into the public domain
```

PJDLS (Padded Jittering Data Link byte Stuffed) is an asynchronous serial data link for low-data-rate applications that supports one or many to many communication optimized for optical wireless communication. Frame separation is obtained with the use of [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md). PJDLS can be easily implemented on limited microcontrollers with low clock accuracy and can operate directly using one or two input-output pins.

### Communication modes
The proposed communication modes are the result of years of testing and optimization for light pulses communication and have been selected to be easily supported also by limited microcontrollers.  

| MODE | Bit timing | Sync bit timing | Pad-bit ratio | Speed               |
| ---- | ---------- | --------------- | ------------- | ------------------- |
| 1    | 750        | 1050            | 1.4           | 128B/s  -  1024Bd   |
| 2    | 572        | 728             | 1.2727        | 170B/s  -  1361Bd   |
| 3    | 188        | 428             | 2.2765        | 471B/s  -  3773Bd   |
| 4    | 128        | 290             | 2.2656        | 639B/s  -  5547Bd   |
| 5    | 56         | 128             | 2.2857        | 1582B/s - 12658Bd   |

Binary timing durations are expressed in microseconds.

### Medium access control
PJDLS specifies a variation of the carrier-sense, non-persistent random multiple access method (non-persistent CSMA). Devices can detect an ongoing transmission for this reason collisions can only occur in multi-master mode when 2 or more devices start to transmit at the same time. When a collision occurs it can be detected by the receiver because of synchronization loss.

### Byte transmission
Byte transmission is composed by 10 bits, the first two are called synchronization pad and are used to obtain sampling synchronization. The synchronization pad is composed by a logic 1 padding bit longer than data bits and a logic 0 data bit. The following 8 data bits contain information in LSB-first (least significant bit first) order.

The reception technique is based on 3 steps:
1. Find a high bit which duration is equal to or acceptably shorter than a high padding bit
2. Synchronize to its falling edge
3. Ensure it is followed by a logic 0 data bit

If this pattern is detected data reception starts, if not, interference, synchronization loss or simply absence of communication is detected.
```cpp  
 ___________ ___________________________
| SYNC-PAD  | DATA                      |
|_______    |___       ___     _____    |
|  |    |   |   |     |   |   |     |   |
|  | 1  | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|__|____|___|___|_____|___|___|_____|___|
   |
Minimum acceptable padding bit duration
```

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
