
![PJON](http://www.gioblu.com/PJON/PJON-github-header-tiny.png)
## PJON v11.0
PJON® (Padded Jittering Operative Network) is an Arduino compatible, multi-master, multi-media network protocol. It proposes a Standard, it is designed as a framework and implements a totally software emulated network protocol stack that can be easily cross-compiled on many architectures like ATtiny, ATmega, ESP8266, ESP32, STM32, Teensy, Raspberry Pi, Linux and Windows x86 machines. It is a valid tool to quickly and comprehensibly build a network of devices. Visit [wiki](https://github.com/gioblu/PJON/wiki) and [documentation](documentation/README.md) to know more about the PJON Standard.

[![Get PJON bus id](https://img.shields.io/badge/GET-PJON%20bus%20id-lightgrey.svg)](http://www.pjon.org/get-bus-id.php)
[![Video introduction](https://img.shields.io/badge/PJON-video%20introduction-blue.svg)](https://www.youtube.com/watch?v=vjc4ZF5own8)
[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Donate](https://img.shields.io/badge/DONATE-Paypal-green.svg)](https://www.paypal.me/PJON)

PJON is used in thousands of devices and its community has spread worldwide because of the following 6 key factors:
- **New technology**: The [PJON](specification/PJON-protocol-specification-v3.0.md) network protocol stack is a new technology crafted in 7 years of research and experimentation. It was originally developed as an open-source alternative to i2c and 1-Wire but during development its scope and features have been extended to cover use cases where TCP-IP is generally applied although requiring slightly more than half of its overhead. PJON has been engineered to support feature modularity adding a variable footprint (3.5-8.2 kB program memory) and overhead (5-22 bytes per packet) depending on the features used.
- **Multi-media support**: PJON packets can be transmitted on a wide range of media and protocols like TCP, UDP, Serial, RS485 and LoRa. The PJON network protocol stack specifies and implements also the [PJDL](src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) wired data link able to communicate data through a single common conductive element shared by up to 255 devices, [PJDLR](src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) wireless data link compatible with many ASK/FSK/OOK radio modules, and also [PJDLS](src/strategies/AnalogSampling/specification/PJDLS-specification-v1.0.md) wireless data link able to communicate through light pulses using off the shelf LEDs and laser diodes easing integration and enabling a lot of applications.
- **Increased security**: Devices using Ethernet/WiFi are often vulnerable to ransomware, illegal cyber warfare and private data leak. PJON has been engineered to enhance security not necessarily implementing the standard network protocol stack together with its vulnerabilities where it is not absolutely necessary offering a set of alternatives for many use cases.
- **Increased reliability**: Many protocols massively applied worldwide like 1-Wire, i2c and CAN expose dangerous vulnerabilities, their error detection algorithms are weak and they are not resilient to interference. The PJON network protocol stack is based on years of analysis and study not to make the same repeated mistakes present in most alternatives and provide with a simpler and more efficient solution.
- **High flexibility**: PJON is totally "software emulated" or executed by software without the use of dedicated hardware, for this reason its implementation can run a wide range of architectures; this engineering choice and its simplicity makes it easy to be ported on any computer or microcontroller. Its implementation is designed to enable developers to port new architectures in minutes defining a short set of interfaces to system calls.
- **Low cost**: Without any additional hardware needed to operate, minimal network wiring requirements and direct pin-to-pin or LED-to-LED communication, PJON is extremely energy efficient, cheap to be implemented and maintained. This implementation is kept updated and meticulously tested thanks to the strong commitment of its growing community of end users, testers and developers.

#### Features
- Cross-compilation support with the [interfaces](src/interfaces) system calls abstraction   
- Multi-media support with the [strategies](src/strategies) data link layer abstraction
- Master-slave or multi-master [dynamic addressing](specification/PJON-dynamic-addressing-specification-v2.0.md)
- Hot-swap support, no need of system reset or shut down when replacing or adding devices
- Configurable synchronous and/or asynchronous [acknowledgement](specification/PJON-protocol-acknowledge-specification-v1.0.md)
- Configurable 2 level addressing (device and bus id) for scalable applications
- Configurable 1 or 2 bytes packet length (max 255 or 65535 bytes)
- Collision avoidance to enable multi-master capability
- Configurable CRC8 or CRC32 table-less cyclic redundancy check
- Packet manager to handle, track and if necessary retransmit a packet sending in background
- Error handling

#### PJON (Padded Jittering Operative Network) Protocol specifications
- [PJON v3.0](specification/PJON-protocol-specification-v3.0.md)
- [PJON Acknowledge v1.0](specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [PJON Dynamic addressing v2.0](specification/PJON-dynamic-addressing-specification-v2.0.md)
- [PJDL v2.0](src/strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md) - [PJDLR v2.0](src/strategies/OverSampling/specification/PJDLR-specification-v2.0.md) - [PJDLS v2.0](src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) - [TSDL v2.0](src/strategies/ThroughSerial/specification/TSDL-specification-v2.0.md)

#### Compliant tools
- [ModuleInterface](https://github.com/fredilarsen/ModuleInterface) - easy config and value sync between IoT modules by Fred Larsen
- [PJON-piper](https://github.com/Girgitt/PJON-piper) - command line wrapper by Zbigniew Zasieczny
- [PJON-python](https://github.com/Girgitt/PJON-python) - python interface by Zbigniew Zasieczny
- [PJON-gRPC](https://github.com/Galitskiy/PJON-gRPC) - gRPC server-client by Oleg Galitskiy

#### Contribute
Feel free to send a pull request sharing something you have made that could help. If you want to support us you can also try to solve an [issue](https://github.com/gioblu/PJON/issues). Thank you for your support.

#### Donate
The PJON project is entirely financed by contributions of people like you and its resources are solely invested to cover the development and maintenance costs, consider to make donation:
- Paypal: [PJON](https://www.paypal.me/PJON)
- Bitcoin: [1FupxAyDTuAMGz33PtwnhwBm4ppc7VLwpD](http://tny.im/btc/address.php?a=1FupxAyDTuAMGz33PtwnhwBm4ppc7VLwpD)
- Ethereum: [0xf34AEAF3B149454522019781668F9a2d1762559b](https://etherchain.org/account/0xf34AEAF3B149454522019781668F9a2d1762559b)

#### License
All the software included in this project is experimental and it is distributed "AS IS" without any warranty, use it at your own risk. [Licensed](https://github.com/gioblu/PJON/blob/master/LICENSE.md) under the Apache License, Version 2.0. PJON® and its brand are registered trademarks, property of Giovanni Blu Mitolo gioscarab@gmail.com
