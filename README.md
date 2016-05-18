
![PJON](http://www.gioblu.com/PJON/PJON-github-header-small.png)
##PJON v3.0 beta
PJON (Padded Jittering Operative Network) is an Arduino compatible, multi-master, multi-media communications bus system. It proposes a Standard and it is designed as a framework for digital communication in embedded systems. It is a valid alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. If you are interested to know more about the PJON standard, visit the [Wiki](https://github.com/gioblu/PJON/wiki). If you need help see the [documentation](https://github.com/gioblu/PJON/wiki/Documentation) or if something is not working visit the [Troubleshooting page](https://github.com/gioblu/PJON/wiki/Troubleshooting). If you own a Saleae Logic Analyzer see [saleae-pjon-protocol-analyzer](https://github.com/aperepel/saleae-pjon-protocol-analyzer) crafted by Andrew Grande.

[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

####Features
- Physical layer abstraction
  - SoftwareBitBang (software emulated non blocking implementation).
    - 1 wire (plus common ground) physical layer with up to 50 meters range.
  - OverSampling (pseudo over-sampling based on SoftwareBitBang).
    - 2 pins (rx/tx) radio ASK/FSK physical layer with up to 3km range.
- Multi media compatibility like wires, radio waves and light (still experimental).
- 2 pin compatibility to enable twisted pair / radio modules.
- Device id implementation to enable univocal communication up to 254 devices.  
- Optional auto-addressing with id collision avoidance (experimental).
- CRC8 table-less cyclic redundancy check implementation.
- Acknowledgement of correct packet sending.
- Collision avoidance to enable multi-master capability.
- Broadcast functionality to contact all connected devices.
- Packet manager to track and retransmit a failed packet sending in background.
- Error handling.

####Compatibility
- ATmega88/168/328 16Mhz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16Mhz (Arduino Mega)
- ATmega16u4/32u4 16Mhz (Arduino Leonardo)
- ATtiny45/85 8Mhz, see https://github.com/gioblu/PJON/wiki/ATtiny-interfacing
- SAMD (Arduino Zero)
- ESP8266 v.1-7 80Mhz "AI-THINKER AT" firmware, see https://github.com/esp8266/Arduino
- ESP8266 NodeMCU v0.9-1.0 80Mhz, see https://github.com/esp8266/Arduino
- MK20DX256 96Mhz (Teensy 3.1)

When including and using PJON, you have the complete access to the microntroller ready to be used, as usual, untouched. This happens because PJON is completely software emulated with a non blocking implementation, transforming a painfull walk to the hill in a nice flight.

Single wire simplicity let you to experiment quickly and with creativity. The first test I suggest, at your own risk, is to let two arduino boards communicate through your body touching with the left hand the digital port of the first board (5v 40ma, harmless) and with the right the port of the other one. Will be stunning to see high accuracy digital communication running inside a living biological body. This opens the mind to possible creative solutions.

![PJON - Michael Teeuw application example](http://33.media.tumblr.com/0065c3946a34191a2836c405224158c8/tumblr_inline_nvrbxkXo831s95p1z_500.gif)

PJON application example made by the user [Michael Teeuw](http://michaelteeuw.nl/post/130558526217/pjon-my-son)

####License

```cpp
/* Copyright 2012-2016 Giovanni Blu Mitolo

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
```
