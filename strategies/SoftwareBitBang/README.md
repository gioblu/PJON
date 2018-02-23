
**Medium:** Wire |
**Pins used:** 1 / 2

`SoftwareBitBang` is a strategy or data link that complies with [PJDL v2.0](/src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md), requires 1 or optionally 2 wires and no additional hardware to handle one or many to many communication on a single channel or bus. It can be run on low performance microcontrollers sharing a common direct pin-to-pin connection. It is a valid alternative to 1-Wire because of its flexibility and reliability. Bus maximum length is mostly limited by resistance of the common conductive element used and by externally induced interference. It has been tested with up to 50m long insulated wires, results demonstrate the same high performance achieved with shorter lengths. Take a look at the [video introduction](https://www.youtube.com/watch?v=Vg5aSlD-VCU) for a brief showcase of its features.
```cpp
PJDL SINGLE WIRE BUS                            ______
 ______    ______    ______    ______          |      |
|      |  |      |  |      |  |      |         |DEVICE|
|DEVICE|  |DEVICE|  |DEVICE|  |DEVICE|         |______|
|______|  |______|  |______|  |______|             |
___|__________|________|___________|_______/\/\/\__| IO PIN
 ___|__    __|___    ___|__    ___|__   |  110-180 Ω
|      |  |      |  |      |  |      |  |  
|DEVICE|  |DEVICE|  |DEVICE|  |DEVICE|  |__/\/\/\__  GND
|______|  |______|  |______|  |______|     1-5 MΩ    
```
It is suggested to add 1-5 MΩ pull-down resistor as shown in the graph above to reduce externally induced interference. Pins can be optionally protected against overload adding a current limiting resistor to each connected pin. The resistor value can be obtained solving the following equation `R = (operating voltage / pin max current drain)`, for example to obtain the current limiting resistor value for an Arduino Uno simply substitute its characteristics: `R = (5v / 0.030A) = 166.66Ω`.

#### Compatibility
- ATmega88/168/328 16MHz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16MHz (Arduino Mega)
- ATmega16u4/32u4 16MHz (Arduino Leonardo)
- ATtiny84/84A 16MHz external oscillator
- ATtiny85 16MHz external oscillator
- SAMD (Arduino Zero)
- ESP8266 v.1-7 80MHz AI-THINKER Modules
- ESP8266 NodeMCU v0.9-1.0 80MHz
- MK20DX256 96MHz (Teensy 3.1)

#### Performance
`SWBB_MODE` can be configured in 3 different modes, `1`, `2` and `3`:
- `1` runs at 16944Bd or 2.12kB/s cross-architecture, promiscuous clock/architecture compatible.
- `2` runs at 21504Bd or 2.68kB/s cross-architecture, promiscuous clock/architecture compatible.
- `3` runs a specific architecture at its maximum limits (non cross-architecture compatible). Every architecture has its own limits, Arduino Duemilanove for example runs at 33472Bd or 4184B/s, Arduino Zero can reach 48000Bd or 6000B/s.

When including and using `SoftwareBitBang`, as data link layer of a PJON bus, you have the complete access to the microcontroller ready to be used, as usual, untouched. This happens because `SoftwareBitBang` runs a completely software emulated implementation, transforming a painful walk in a nice flight.

Single wire simplicity let you to experiment quickly and with creativity. The first suggested test, at the tester's risk, is to let two arduino boards communicate [through a living body](https://www.youtube.com/watch?v=caMit7nzJsM) touching with the left hand the digital port of the first board (5v 40ma, harmless) and with the right the port of the other one. It is stunning to see highly accurate digital communication running through a living biological body. This opens the mind to possible creative solutions.

#### How to use SoftwareBitBang
Before including `PJON.h` it is possible to configure `SoftwareBitBang` using predefined constants:
```cpp  
/* SoftwareBitBang default SWBB_MODE: 1
   (Transfer speed: 16.949kBb or 2.11kB/s) */

// Set SWBB_MODE 2 before PJON.h inclusion
// (Speed: 21.505kBd or 2.68kB/s)
#define SWBB_MODE 2

// Set SWBB_MODE 3 before PJON.h inclusion
// (Architecture / Toolchain dependant)
#define SWBB_MODE 3

/* Synchronous acknowledgement response timeout
   (1.5 milliseconds by default) If latency + CRC
   computation > SWBB_RESPONSE_TIMEOUT
   synchronous acknowledgement reliability could
   be affected or disrupted higher
   SWBB_RESPONSE_TIMEOUT if necessary. */
#define SWBB_RESPONSE_TIMEOUT 1500

// Set the back-off exponential degree (default 4)
#define SWBB_BACK_OFF_DEGREE     4

// Set the maximum sending attempts (default 20)
#define SWBB_MAX_ATTEMPTS       20

#include <PJON.h>

PJON<SoftwareBitBang> bus;

void setup() {
  // Set the pin 12 as the communication pin
  bus.strategy.set_pin(12);
  // Set pin 11 as input pin and pin 12 as output pin
  bus.strategy.set_pins(11, 12);
}
```
After the PJON object is defined with its strategy it is possible to set the communication pin accessing to the strategy present in the PJON instance. All the other necessary information is present in the general [Documentation](/documentation).

#### Why not interrupts?
In the Arduino environment the use of libraries is really extensive and often the end user is not able to go over collisions. Very often a library is using hardware resources of the microcontroller as timers or interrupts, colliding or interrupting other libraries. This happens because in general Arduino boards have limited hardware resources. To have a universal and reliable communication medium in this sort of environment, software emulated bit-banging, is a good, stable and reliable solution that leads to "more predictable" results than interrupt driven systems coexisting on small microcontrollers without the original developer and the end user knowing about it.

![PJON - Michael Teeuw application example](http://33.media.tumblr.com/0065c3946a34191a2836c405224158c8/tumblr_inline_nvrbxkXo831s95p1z_500.gif)

PJON application example made by the user [Michael Teeuw](http://michaelteeuw.nl/post/130558526217/pjon-my-son)

#### Known issues
- A 1-5 MΩ pull down resistor could be necessary to reduce interference, see [deal with interference](https://github.com/gioblu/PJON/wiki/Deal-with-interference).
- Consider that this is not an interrupt driven system, during the time passed
in delay or executing other tasks a certain amount of packets could be potentially
lost, PJON does its job scheduling the packet
to be sent again in future but a certain amount of bandwidth can be wasted. Structure intelligently
your loop cycle and polling duration to avoid low transmission accuracy.
- `SoftwareBitBang` strategy can have compatibility issues with codebases that
are using interrupts, reliability or bandwidth loss can be experienced because of the cyclical
interruptions made by third party software to the PJON procedure.
