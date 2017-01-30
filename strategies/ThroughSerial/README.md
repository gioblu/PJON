
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

  /* Set 100 milliseconds is the maximum timeframe between every receive call
     in any of the connected devices. If this timeframe is in average exceeded
     by some of the connected devices, communication reliability could drop
     or be disrupted. (before PJON.h inclusion) */
  #define TS_MAX_BYTE_TIME          1000000

  /* Set 0.5 milliseconds as the minimum timeframe of free port before transmitting
     (before PJON.h inclusion) */
  #define TS_FREE_TIME_BEFORE_START     500

  /* This timing configuration is ok for a master-slave setup, but could lead to
     collisions if used in a multi-master setup.

  If using ThroughSerial in a multi-master setup with synchronous acknowledgment
  NEVER set TS_FREE_TIME_BEFORE_START < TS_MAX_BYTE_TIME or a device could start
  transmitting while a couple is still exchanging an acknowledge */
  #define TS_MAX_BYTE_TIME           100000

  #define TS_FREE_TIME_BEFORE_START  110000

  /* Above is shown multi-master compatible setup able to receive a synchronous
     acknowledgment with a maximum delay 100 milliseconds. Channel analysis before
     transmission is set to 110 milliseconds to avoid collisions.

     Which is the correct value for your setup depends on the maximum average time
     interval between every receive call in your system. TS_MAX_BYTE_TIME
     should be around the same duration. So in a sketch where there is only a
     delay(10) between every receive call at least 10000 should be the correct
     value for TS_MAX_BYTE_TIME.

     If your tasks timing are long and a satisfactory setup can't be reached
     consider to drop the use of the synchronous acknowledge and start using the
     asynchronous acknowledgment instead. */

  /* Set the back-off exponential degree */
  #define TS_BACK_OFF_DEGREE              4

  /* Set the maximum sending attempts */
  #define TS_MAX_ATTEMPTS                20

  /* The values set above are the default producing a 3.2 seconds
     back-off timeout with 20 attempts. Higher TS_MAX_ATTEMPTS to higher
     the back-off timeout, higher TS_BACK_OFF_DEGREE to higher the interval
     between every attempt. */

  #include <PJON.h>

  PJON<SoftwareBitBang> bus;

  void setup() {
    Serial.begin(9600);
    bus.strategy.set_serial(&Serial);
  }
```
For a simple use with RS485 serial modules a transmission enable pin setter has been added:
```cpp  
  bus.strategy.set_enable_RS485_pin(11);
```

All the other necessary information is present in the general [Documentation](https://github.com/gioblu/PJON/wiki/Documentation).

####Known issues
- Being PJON not an interrupt driven, its communication can be affected and potentially disrupted by long delays added in the user's sketch. Try to reduce as possible the interval between every `receive` call. A delay between every receive call higher than 100 millisseconds can disurpt the synchronous acknowledment transmission phase, higher `TS_MAX_BYTE_TIME` in `ThroughSerial.h` if necessary.
