
### Specifications index

#### Network layer
- [PJON (Padded Jittering Operative Network) v3.1](/specification/PJON-protocol-specification-v3.1.md)
- [Acknowledge specification v1.0](/specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [Dynamic addressing specification v3.0](/specification/PJON-dynamic-addressing-specification-v3.0.md)
- [Network services list](/specification/PJON-network-services-list.md)
#### Data link layer
- [PJDL (Padded Jittering Data Link) v3.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v3.0.md)
- **[PJDLR (Padded Jittering Data Link over Radio) v3.0](/src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md)**
- [PJDLS (Padded Jittering Data Link byte Stuffed) v2.0](/src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md)
- [TSDL (Tardy Serial Data Link) v2.1](/src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md)
- [SFSP (Secure Frame Separation Protocol) v1.0](/specification/SFSP-frame-separation-specification-v1.0.md)

---

## PJDLR v3.0
```
Invented by Giovanni Blu Mitolo
Originally published: 10/04/2010, latest revision: 26/06/2019
Related implementation: /src/strategies/OverSampling/
Compliant versions: PJON v12.0 and following
Released into the public domain
```
PJDLR (Padded Jittering Data Link over Radio) is an asynchronous serial data link for low-data-rate applications that supports one or many to many communication optimized to obtain long range and high reliability using ASK, FSK or OOK radio transceivers. PJDLR can be easily implemented on limited microcontrollers with low clock accuracy and can operate directly using one or two input-output pins.

### Communication modes
The proposed communication mode is the result of years of testing and optimization for ASK/FSK radio transceivers and have been selected to be easily supported also by low quality hardware.  

| MODE | Bit timing | Sync bit timing | Pad-data ratio | Speed               |
| ---- | ---------- | --------------- | -------------- | ------------------- |
| 1    | 512        | 328             | 0.64           | 202B/s - 1620Bd     |

Binary timing durations are expressed in microseconds.

### Medium access control
PJDLR specifies a variation of the carrier-sense, non-persistent random multiple access method (non-persistent CSMA). Devices can detect an ongoing transmission for this reason collisions can only occur in multi-master mode when 2 or more devices start to transmit at the same time. When a collision occurs it can be detected by the receiver because of synchronization loss.

### Byte transmission
Byte transmission is composed by 10 bits, the first two are called synchronization pad and are used to obtain sampling synchronization. The synchronization pad is composed by a high padding bit shorter than data bits and a low data bit. The following 8 data bits contain information in LSB-first (least significant bit first) order.

The reception technique is based on 3 steps:
1. Find a high bit which duration is equal to or acceptably shorter than a high padding bit
2. Synchronize to its falling edge
3. Ensure it is followed by a low data bit
4. If so reception starts, if not, interference, synchronization loss or simply absence of communication is detected
```cpp  
 _____ ___________________________
| Pad | Byte                      |
|_    |___       ___     _____    |
| |   |   |     |   |   |     |   |
|1| 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
|_|___|___|_____|___|___|_____|___|
```

### Frame transmission
Before a frame transmission, the communication medium's state is analysed, if high communication is detected and collision is avoided, if low for a duration that is longer than the response time-out plus a small random time, frame transmission starts with a frame initializer composed by 3 consequent synchronization pads followed by data bytes. The presence of the synchronization pad between each byte ensures that also a frame composed of a series of bytes with decimal value 0 can be transmitted safely without risk of collision.

```cpp     
 INITIALIZER  DATA
 ___________ __________ _______________ ______________
|Pad|Pad|Pad| Byte     |Pad| Byte      |Pad| Byte     |
|_  |_  |_  |     __   |_  |      _   _|_  |      _   |
| | | | | | |    |  |  | | |     | | | | | |     | |  |
|1|0|1|0|1|0|0000|11|00|1|0|00000|1|0|1|1|0|00000|1|00|
|_|_|_|_|_|_|____|__|__|_|_|_____|_|_|_|_|_|_____|_|__|
```
When a frame is received a low performance microcontroller with an inaccurate clock can correctly synchronize with transmitter during the frame initializer and consequently each byte is received. The frame initializer is detected if 3 synchronization pads occurred and if their duration is coherent with its expected duration. Frame initialization is 100% reliable, false positives can only occur because of externally induced interference.      

### Synchronous response
Between frame transmission and a synchronous response there is a variable interval which duration is influenced by latency. In order to avoid other devices to detect the medium free for use and disrupt an ongoing exchange, the sender cyclically transmits a short high bit (1/2 high padding bit) and consequently attempts to receive a response. On the other side the receiver can safely transmit its response as soon as possible without caring about the state of the medium. If the acknowledgement is not transmitted or not received the transmitter continues to keep busy the medium up to the maximum acceptable time between transmission and response.
```cpp  
Transmission                                      Response
 ______  ______  ______  ______   _   _   _   _   _ _____
| INIT || BYTE || BYTE || BYTE | | | | | | | | | | | ACK |
|------||------||------||------| | | | | | | | | | |-----|
|      ||      ||      ||      | | | | | | | | | | |  6  |
|______||______||______||______|_| |_| |_| |_| |_| |_____|

```

The maximum time dedicated to potential acknowledgement reception for a given application can be determined practically transmitting the longest supported frame with the farthest physical distance between the two devices. The highest interval between packet transmission and acknowledgement measured, plus a small margin, is the correct timeout that should exclude acknowledgement losses. Consider that the longer this timeout is, the more bandwidth is wasted if the transmission is not successful.
