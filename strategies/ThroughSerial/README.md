
**Medium:** Hardware/Software Serial port |
**Pins used:** 1 or 2

With ThroughSerial data link layer strategy, PJON can run through a software emulated or hardware Serial port. Thanks to this choice it is possible to leverage of virtually all the arduino compatible serial transceivers, like RS485, radio or infrared modules, still having PJON unchanged on top. Take a look at the [video introduction](https://www.youtube.com/watch?v=H4jUsgvM-lw) for a brief showcase of its features.


#### Why PJON over Serial?
Serial communication is an hardware integrated or software emulated data communication that can reach very fast communication speed but it includes only the data link layer; adding PJON on top of the Serial data link layer it is possible to leverage of the PJON protocol layer features like acknowledge, addressing, multiplexing, packet handling, 8-bit CRC and traffic control.

#### How to use ThroughSerial
Pass the `ThroughSerial` type as PJON template parameter to instantiate a PJON object ready to communicate through this Strategy.
```cpp  
  PJON<ThroughSerial> bus; // 2 pin over-sampled data link layer
```
Call the `begin` method on the `Serial` or `SoftwareSerial`  object you want to use for PJON communication and pass it to the `set_serial` method:
```cpp  
  /* Set 5 milliseconds (default) as the maximum timeframe for byte reception.
     This value depends on the latency, baud rate and computation time.
     Always set TX_MAX_RESPONSE_TIME > (byte transmission time + latency) */
  #define TS_BYTE_TIME_OUT      5000

  /* Set 10 milliseconds as the maximum timeframe between transmission and
     synchronous acknowledgement response. This value is strictly related to
     the maximum time needed by receiver to receive and compute the packet. */
  #define TS_RESPONSE_TIME_OUT 10000
  /* If it leads to unacceptable loop time duration consider to use asynchronous
     acknowledgement instead, set TS_RESPONSE_TIME_OUT to 0 and call the
     following methods in the setup:
       bus.set_synchronous_acknowledge(false);
       bus.set_asynchronous_acknowledge(true); */

  #include <PJON.h>

  PJON<ThroughSerial> bus;

  void setup() {
    Serial.begin(9600);
    bus.strategy.set_serial(&Serial);
  }
```
For a simple use with RS485 serial modules a transmission enable pin setter has been added:
```cpp  
  bus.strategy.set_enable_RS485_pin(11);
```

All the other necessary information is present in the general [Documentation](/documentation).

#### Known issues
- Being PJON not an interrupt driven, its communication can be affected and potentially disrupted by long delays added in the user's sketch. Try to reduce as possible the interval between every `receive` call. A delay between every receive call higher than 100 millisseconds can disurpt the synchronous acknowledment transmission phase, higher `TS_MAX_BYTE_TIME` in `ThroughSerial.h` if necessary.
