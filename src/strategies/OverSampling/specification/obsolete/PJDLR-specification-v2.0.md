
## PJDLR v2.0
```
Invented by Giovanni Blu Mitolo
Originally published: 10/04/2010, latest revision: 31/10/2018
Related implementation: /src/strategies/OverSampling/
Compliant versions: PJON v9.0 and following
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
A frame transmission in both master-slave and multi-master modes can be optionally followed by a synchronous response of its recipient, all devices must use the same response time-out to avoid collisions. The acknowledgment reception phase must be shorter than the response time-out to be successful.
```cpp  
Transmission                             Response
 ______  ______  ______                   _____
| INIT || BYTE || BYTE | CRC COMPUTATION | ACK |
|------||------||------|-----------------|     |
|      ||      ||      | LATENCY         |  6  |
|______||______||______|                 |_____|
```

The required response time-out for a given application can be determined practically transmitting the longest supported frame with the farthest physical distance between the two devices. The highest interval between packet transmission and acknowledgement measured plus a small margin is the correct time-out that should exclude acknowledgement losses.
