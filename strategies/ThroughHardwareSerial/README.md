
**Medium:** Hardware Serial port |
**Pins used:** 2

With ThroughHardwareSerial data link layer strategy, PJON can run through the Serial port of your device. Both ports should be free from USB computer connection and data pins should be wired inverted, TX to RX and RX to TX.

####Why PJON over Serial?
Serial communication is an hardware integrated communication medium that can reach very fast communication speed but it is quite basic and lacks many of the necessary structures and entities for an easy setup of a network communication.

####Serial Downsides:
- No CRC, checksum or parity bit (ensure correct data transfert)
- No transmission certainty (lack of acknowledge from receiver)
- No packet handling (no retrasmission in case of error)
- No idea of id (no chance to connect more than 2 devices together)

####Adding PJON over Serial you get:
- Correct transmission certainty with the use of CRC 8-bit
- Acnowledge byte to inform transmitter of correct data reception
- Packet handling, i.e. schedule a repeated packet transmission.
- ThroughHardwareSerial can run a 2 device bus in multi-master mode or a n devices bus in master-slave mode. (n still unkown, tested with 1 master and 2 slaves)

####How to use ThroughHardwareSerial
Pass the `ThroughHardwareSerial` type as PJON template parameter to instantiate a PJON object ready to communicate in this Strategy.
```cpp  
  PJON<ThroughHardwareSerial> bus; // 2 pin over-sampled data link layer
```
Call the `begin` method on the `Serial` object you want to use for PJON communication and pass it to the `set_serial` method:
```cpp  
  void setup() {
    Serial.begin(9600);
    bus.strategy.set_serial(&Serial);
  }
```
All the other necessary information is present in the general [Documentation](https://github.com/gioblu/PJON/wiki/Documentation).
