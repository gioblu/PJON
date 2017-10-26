
![PJON](http://www.gioblu.com/PJON/PJON-github-header-tiny.png)
## PJON v8.2
PJON™ (Padded Jittering Operative Network) is an Arduino compatible, multi-master, multi-media communications bus system. It proposes a Standard, it is designed as a framework and implements a totally software emulated network protocol stack that can be easily cross-compiled on many architectures like ATtiny, ATmega, ESP8266, Teensy, Raspberry Pi, Linux and Windows x86 machines. It is a valid tool to quickly and comprehensibly build a network of devices. Visit [wiki](https://github.com/gioblu/PJON/wiki) and [documentation](https://github.com/gioblu/PJON/wiki/Documentation) to know more about the PJON Standard.

[![Get PJON bus id](https://img.shields.io/badge/GET-PJON%20bus%20id-lightgrey.svg)](http://www.pjon.org/get-bus-id.php)
[![Video introduction](https://img.shields.io/badge/PJON-video%20introduction-blue.svg)](https://www.youtube.com/watch?v=vjc4ZF5own8)
[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Donate](https://img.shields.io/badge/DONATE-Paypal-green.svg)](https://www.paypal.me/PJON)

PJON is used in thousands of devices and its community has spread worldwide because of the following 5 key factors:
- **New technology**: The [PJON](specification/PJON-protocol-specification-v2.0.md) network protocol layer is a new technology crafted in years of experimentation and testing, that, while supporting TCP, UDP, Serial and RS485, it implements the [PJDL](specification/PJON-protocol-specification-v2.0.md) data link able to communicate data through a single common conductive element shared by up to 255 devices with up to 50 meters range, [PJDLR](strategies/OverSampling/specification/PJDLR-specification-v2.0.md) data link supporting many ASK/FSK/OOK radio modules available on the market, and also, [PJDLS](strategies/AnalogSampling/specification/PJDLS-specification-v1.0.md) data link able to communicate wirelessly through a single LED or a couple of emitters and receivers, easing integration and enabling a lot of applications.
- **Increased security**: Ethernet and WiFi are exposing many smart things to ransomware, illegal cyber-warfare activities and putting privacy at risk. PJON has been engineered to enhance security not necessarily implementing the standard network protocol stack together with its vulnerabilities where it is not absolutely necessary, offering a set of alternatives covering many use cases.
- **Increased reliability**: Many protocols applied worldwide, also in critical use cases, like 1-Wire, i2c and CAN exposes dangerous vulnerabilities, their error detection algorithms are weak and they are not resilient in case of interference. The [PJON](specification/PJON-protocol-specification-v2.0.md) network protocol specification is based on years of analysis and study not to make the same repeated mistakes present in most alternatives and provide with a more efficient a simpler solution.
- **High flexibility**: The PJON network protocol stack is modular enabling the use of the same network protocol implementation on different data links or media and on different architectures, simply changing its configuration.
- **Fast portability**: PJON is totally "software emulated" or executed by software without the use of dedicated hardware; this engineering choice and its simplicity makes it easy to be ported on any computer or microcontroller. Its implementation is designed to enable developers to port new architectures in minutes defining a short set of interfaces to system calls.
- **Low cost**: Without any additional hardware needed to operate, minimal network wiring requirements and direct pin-to-pin or LED-to-LED low current communication, PJON is extremely energy efficient, cheap to be implemented and maintained. This implementation is kept updated and meticulously tested thanks to the strong commitment of the PJON foundation and the huge support of its growing community of end users, testers and developers.

#### Features
- Cross-compilation support with the [interfaces](interfaces) system calls abstraction   
- Multi-media support with the [strategies](strategies) data link layer abstraction
- Master-slave or multi-master [dynamic addressing](specification/PJON-dynamic-addressing-specification-v0.1.md)
- Hot-swap support, no need of system reset or shut down when replacing or adding devices
- Configurable synchronous and/or asynchronous [acknowledgement](specification/PJON-protocol-acknowledge-specification-v0.1.md)
- Configurable 2 level addressing (device and bus id) for scalable applications
- Configurable 1 or 2 bytes packet length (max 255 or 65535 bytes)
- Collision avoidance to enable multi-master capability
- Configurable CRC8 or CRC32 table-less cyclic redundancy check
- Packet manager to handle, track and if necessary retransmit a packet sending in background
- Error handling

#### PJON (Padded Jittering Operative Network) Protocol specification
- [PJON v2.0](specification/PJON-protocol-specification-v2.0.md)
- [PJON Acknowledge v1.0](specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [PJON Dynamic addressing v1.0](specification/PJON-dynamic-addressing-specification-v1.0.md)

#### Data links specification
- [PJDL v2.0](strategies/SoftwareBitBang/specification/PJDL-specification-v2.0.md)
- [PJDLR v2.0](strategies/OverSampling/specification/PJDLR-specification-v2.0.md)
- [PJDLS v1.0](strategies/AnalogSampling/specification/PJDLS-specification-v1.0.md)
- [TSDL v1.0](strategies/ThroughSerial/specification/TSDL-specification-v1.0.md)

#### Compliant tools
- [ModuleInterface](https://github.com/fredilarsen/ModuleInterface) by Fred Larsen
- [PJON-piper](https://github.com/Girgitt/PJON-piper) by Zbigniew Zasieczny
- [PJON-python](https://github.com/Girgitt/PJON-python) by Zbigniew Zasieczny

PJON™ is a self-funded, no-profit open-source project created in 2010 and maintained by Giovanni Blu Mitolo with the support of the internet community. If you want to see the PJON project growing with a faster pace, consider a donation at the following link: https://www.paypal.me/PJON

PJON™ and its brand are unregistered trademarks, property of Giovanni Blu Mitolo gioscarab@gmail.com
