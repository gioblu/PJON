
**Medium:** Hardware/Software Serial port |
**Pins used:** 1 or 2

With ThroughSerial data link layer strategy, PJON can run through a software emulated or hardware Serial port. Thanks to this choice it is possible to leverage of virtually all the arduino compatible serial transceivers, like RS485, radio or infrared modules, still having PJON unchanged on top.

####Why PJON over Serial?
Serial communication is an hardware integrated or software emulated data communication that can reach very fast communication speed but it includes only the data link layer; adding PJON on top of the Serial data link layer it is possible to leverage of the PJON protocol layer features like acknowledge, addressing, multiplexing, packet handling, 8-bit CRC and traffic control.

####How to use ThroughSerial
Pass the `ThroughSerial` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
  PJON<ThroughSerial> bus; // 2 pin over-sampled data link layer
```
Call the `begin` method on the `Serial` or `SoftwareSerial`  object you want to use for PJON communication and pass it to the `set_serial` method:
```cpp  
  void setup() {
    Serial.begin(9600);
    bus.strategy.set_serial(&Serial);
  }
```
For a simple use with RS485 serial modules a transmission enable pin setter has been added:
```cpp  
  bus.set_enable_RS485_pin(11);
```

All the other necessary information is present in the general [Documentation](https://github.com/gioblu/PJON/wiki/Documentation).
