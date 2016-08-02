
![PJON](http://www.gioblu.com/PJON/PJON-github-header-tiny.png)
##PJON v4.0
PJON (Padded Jittering Operative Network) is an Arduino compatible, multi-master, multi-media communications bus system. It proposes a Standard and it is designed as a framework for digital communication. It is a valid alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. If you are interested to know more about the PJON Standard, visit the [Wiki](https://github.com/gioblu/PJON/wiki). If you need help see the [documentation](https://github.com/gioblu/PJON/wiki/Documentation) or if something is not working visit the [Troubleshooting page](https://github.com/gioblu/PJON/wiki/Troubleshooting). If you own a Saleae Logic Analyzer see [saleae-pjon-protocol-analyzer](https://github.com/aperepel/saleae-pjon-protocol-analyzer) crafted by Andrew Grande.

[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Donate](http://img.shields.io/paypal/donate.png?color=blue)](https://www.paypal.me/PJON)

####Features
- [Device id](https://github.com/gioblu/PJON/wiki) implementation to enable univocal communication up to 254 devices  
- [Bus id](https://github.com/gioblu/PJON/wiki) implementation to optionally enable shared medium communication
- [Multi media](https://github.com/gioblu/PJON/wiki/OverSampling) compatibility (wires up to 50m, radio up to 5km)
- Physical layer abstraction and [communication strategy framework](https://github.com/gioblu/PJON/wiki/Strategies)
- PJON communication through a Serial port with `ThroughHardwareSerial` 
- 2 pin compatibility to enable twisted pair / radio modules
- Optional auto-addressing with id collision avoidance (experimental)
- CRC8 table-less cyclic redundancy check implementation
- Acknowledgement of correct packet sending
- Collision avoidance to enable multi-master capability
- Broadcast functionality to contact all connected devices
- Packet manager to track and retransmit a failed packet sending in background
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
