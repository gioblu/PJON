
![PJON](http://www.gioblu.com/PJON/PJON-github-header-tiny.png)
##PJON v5.0
PJON (Padded Jittering Operative Network) is an Arduino compatible, multi-master, multi-media communications bus system. It proposes a Standard and it is designed as a framework for digital communication. It is a valid alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. Visit the [Wiki](https://github.com/gioblu/PJON/wiki), [Documentation](https://github.com/gioblu/PJON/wiki/Documentation) and [Troubleshooting](https://github.com/gioblu/PJON/wiki/Troubleshooting) wiki pages to know more about the PJON Standard.

[![Video introduction](https://img.shields.io/badge/PJON-video%20introduction-blue.svg)](https://www.youtube.com/watch?v=vjc4ZF5own8)
[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Donate](http://img.shields.io/paypal/donate.png?color=brightgreen)](https://www.paypal.me/PJON)

####Features
- Multi-media compatible thanks to the physical layer abstraction or [Strategy framework](https://github.com/gioblu/PJON/wiki/Strategies)
- [Device id](https://github.com/gioblu/PJON/wiki) implementation to contact a certain device in the bus  
- [Bus id](https://github.com/gioblu/PJON/wiki) implementation to contact a device in a certain bus
- CRC8 table-less cyclic redundancy check implementation
- Acknowledgement of correct packet sending
- Collision avoidance to enable multi-master capability
- Broadcast functionality to contact all connected devices
- Packet manager handle, track and if necessary retransmit a packet sending in background
- Error handling

####Compatibility
- ATmega88/168/328 16Mhz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16Mhz (Arduino Mega)
- ATmega16u4/32u4 16Mhz (Arduino Leonardo)
- ATtiny45/85 8Mhz, see https://github.com/gioblu/PJON/wiki/ATtiny-interfacing
- SAMD (Arduino Zero)
- ESP8266 v.1-7 80Mhz "AI-THINKER AT" firmware, see https://github.com/esp8266/Arduino
- ESP8266 NodeMCU v0.9-1.0 80Mhz, see https://github.com/esp8266/Arduino
- MK20DX256 96Mhz (Teensy 3.1)

####Compliant tools
-[saleae-pjon-protocol-analyzer](https://github.com/aperepel/saleae-pjon-protocol-analyzer) by Andrew Grande.
-[PJON-python](https://github.com/Girgitt/PJON-python) by Zbigniew Zasieczny.

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
