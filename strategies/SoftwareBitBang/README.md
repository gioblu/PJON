
**Medium:** Wire |
**Pins used:** 1 / 2

SoftwareBitBang is the default data link layer used by PJON and it is based on `micros` and `delayMicroseconds`. It needs 1 or optionally 2 wires and no additional hardware to handle communication with a software emulated implementation. It supports a star configuration network with many devices transmitting and receiving over the same 1 or 2 wires. It is designed to work with most of the Arduino compatible boards, have a small memory footprint and to be extremely resilient to interference. Thanks to the use of a dedicated `PJON_IO` library, can be achieved fast and reliable cross-architecture communication through one or two pins. It complies with [PJDL v1.0](/strategies/SoftwareBitBang/specification/PJDL-specification-v1.0.md) Data link layer specification. Take a look at the [video introduction](https://www.youtube.com/watch?v=Vg5aSlD-VCU) for a brief showcase of its features.

#### Compatibility
- ATmega88/168/328 16Mhz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16Mhz (Arduino Mega)
- ATmega16u4/32u4 16Mhz (Arduino Leonardo)
- ATtiny45/85 8Mhz, see https://github.com/gioblu/PJON/wiki/ATtiny-interfacing
- SAMD (Arduino Zero)
- ESP8266 v.1-7 80Mhz "AI-THINKER AT" firmware, see https://github.com/esp8266/Arduino
- ESP8266 NodeMCU v0.9-1.0 80Mhz, see https://github.com/esp8266/Arduino
- MK20DX256 96Mhz (Teensy 3.1)

#### Performance
PJON works in 3 different communication modes, `STANDARD`, `FAST` and `OVERDRIVE`:
- `STANDARD` runs at 16944Bd or 2.12kB/s cross-architecture, promiscuous clock/architecture compatible.
- `FAST` runs at 25157Bd or 3.15kB/s cross-architecture, promiscuous clock/architecture compatible.
- `OVERDRIVE` runs a specific architecture at its maximum limits (non cross-architecture compatible). Every architecture has its own limits, Arduino Duemilanove for example runs at 33898Bd or 4.23kB/s, Arduino Zero can reach 48000Bd or 6.00kB/s.

When including and using SoftwareBitBang, as data link layer of a PJON bus, you have the complete access to the microntroller ready to be used, as usual, untouched. This happens because SoftwareBitBang is completely software emulated strategy with a non blocking implementation, transforming a painfull walk to the hill in a nice flight.

Single wire simplicity let you to experiment quickly and with creativity. The first suggested test, at the tester's risk, is to let two arduino boards communicate [through a living body](https://www.youtube.com/watch?v=caMit7nzJsM) touching with the left hand the digital port of the first board (5v 40ma, harmless) and with the right the port of the other one. It is stunning to see highly accurate digital communication running inside a living biological body. This opens the mind to possible creative solutions.

#### How to use SoftwareBitBang
Pass the `SoftwareBitBang` type as PJON template parameter to instantiate a PJON object ready to communicate in this Strategy. All the other necessary information is present in the general [Documentation](/documentation).
```cpp  
  /* The default SoftwareBitBang mode is _SWBB_STANDARD
     (Transfer speed: 16.944kBb or 2.12kB/s) */

  /* Set SoftwareBitBang mode to _SWBB_FAST before PJON.h inclusion
     (Transfer speed: 25.157kBd or 3.15kB/s) */
  #define SWBB_MODE 2

  /* Set SoftwareBitBang mode to _SWBB_OVERDRIVE before PJON.h inclusion
     (Architecture / Toolchain dependant) */
  #define SWBB_MODE 3

  /* Acknowledge latency maximum duration (1000 microseconds default).
     Could be necessary to higher SWBB_LATENCY if sending long packets because
     of the CRC computation time needed by receiver before transmitting its acknowledge  */
  #define SWBB_LATENCY 1000

  /* Set the back-off exponential degree */
  #define SWBB_BACK_OFF_DEGREE 4

  /* Set the maximum sending attempts */
  #define SWBB_MAX_ATTEMPTS 20

  /* The values set above are the default producing a 3.2 seconds
     back-off timeout with 20 attempts. Higher SWBB_MAX_ATTEMPTS to higher
     the back-off timeout, higher SWBB_BACK_OFF_DEGREE to higher the interval
     between every attempt. */

  #include <PJON.h>

  PJON<SoftwareBitBang> bus;

  void setup() {
    bus.strategy.set_pin(12);       // Set the pin 12 as the communication pin
                                    // or
    bus.strategy.set_pins(11, 12);  // Set pin 11 as input pin and pin 12 as output pin  
  }

```
After the PJON object is defined with its strategy it is possible to set the communication pin accessing to the strategy present in the PJON instance.

#### Why not interrupts?
The use of libraries is really extensive in the Arduino environment and often the end user is not able to go over collisions. Very often a library is using hardware resources of the microcontroller as timers or interrupts, colliding or interrupting other libraries. This happens because in general Arduino boards have limited hardware resources. To have a universal and reliable communication medium in this sort of environment, software emulated bit-banging, is a good, stable and reliable solution that leads to "more predictable" results than interrupt driven systems coexisting on small microcontrollers without the original developer and the end user knowing about it.

![PJON - Michael Teeuw application example](http://33.media.tumblr.com/0065c3946a34191a2836c405224158c8/tumblr_inline_nvrbxkXo831s95p1z_500.gif)

PJON application example made by the user [Michael Teeuw](http://michaelteeuw.nl/post/130558526217/pjon-my-son)

#### Known issues
- A pull down resistor in the order of mega ohms could be necessary on the bus to reduce interference, see [deal with interference](https://github.com/gioblu/PJON/wiki/Deal-with-interference). In late november 2016 a bug has been discovered, it was on many devices creating a slight inconsistency in the channel state during transitions, most of the times
disappearing with the use of a pull-down resistor ([120b2c](https://github.com/gioblu/PJON/commit/120b2c72f1435519e7712adfd2c3f1eecc38557c)), with this bugfix the channel is much more reliable and in most cases there is no more need of a pull-down resistor to have nominal communication speed.
- Consider that this is not an interrupt driven system and so all the time passed
in delay or executing something a certain amount of packets could be potentially
lost not received, the packet manager of PJON will do its job scheduling the packet
to be sent again in future until is received or `PJON_MAX_ATTEMPTS` sending attempts is
reached, but a certain amount of bandwidth can be wasted. Structure intelligently
your loop cycle to avoid huge blind timeframes.
- `SoftwareBitBang` strategy can have compatibility issues with codebases that
are using interrupts, reliability or bandwidth loss can be experienced because of the cyclical
interruptions made by third party software to the PJON procedure.
