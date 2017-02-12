
![PJON](http://www.gioblu.com/PJON/PJON-github-header-tiny.png)
##PJON v6.2
PJON™ (Padded Jittering Operative Network) is an Arduino compatible, multi-master, multi-media communications bus system. It proposes a Standard, it is designed as a framework and implements a totally software-emulated network protocol stack. It is a valid alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. Visit the [Wiki](https://github.com/gioblu/PJON/wiki), [Documentation](https://github.com/gioblu/PJON/wiki/Documentation) and [Troubleshooting](https://github.com/gioblu/PJON/wiki/Troubleshooting) wiki pages to know more about the PJON Standard.

[![Get PJON bus id](https://img.shields.io/badge/GET-PJON%20bus%20id-lightgrey.svg)](http://www.pjon.org/get-bus-id.php)
[![Video introduction](https://img.shields.io/badge/PJON-video%20introduction-blue.svg)](https://www.youtube.com/watch?v=vjc4ZF5own8)
[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Donate](http://img.shields.io/paypal/donate.png?color=brightgreen)](https://www.paypal.me/PJON)

####Features
- Configurable 2 level addressing (device and bus id) for scalable applications
- Multi-media support with the data link layer abstraction or [Strategy](https://github.com/gioblu/PJON/tree/master/strategies) framework
- Configurable strategies inclusion (for memory optimization)
- Configurable 1 or 2 bytes packet length (max 255 or 65535 bytes)
- Master-slave or multi-master [dynamic addressing](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md)
- Configurable synchronous and/or asynchronous [acknowledgement](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md) of correct packet sending
- Collision avoidance to enable multi-master capability
- Selectable CRC8 or CRC32 table-less cyclic redundancy check
- Packet manager to handle, track and if necessary retransmit a packet sending in background
- Optional ordered packet sending
- Error handling

####PJDL (Padded Jittering Data Link) specification:
- [PJDL v0.1](https://github.com/gioblu/PJON/blob/master/strategies/SoftwareBitBang/specification/padded-jittering-protocol-specification-v0.1.md)
- [PJDL v1.0](https://github.com/gioblu/PJON/blob/master/strategies/SoftwareBitBang/specification/PJDL-specification-v1.0.md)
- [PJDLR v1.0](https://github.com/gioblu/PJON/blob/master/strategies/OverSampling/specification/PJDLR-specification-v1.0.md)

####PJON (Padded Jittering Operative Network) Protocol specification:
- [v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md)
- [v0.2](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md)
- [v0.3](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md)
- [v1.0](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md)
- [Acknowledge v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md)
- [Dynamic addressing v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md)

####Compliant tools
- [saleae-pjon-protocol-analyzer](https://github.com/aperepel/saleae-pjon-protocol-analyzer) by Andrew Grande
- [PJON-python](https://github.com/Girgitt/PJON-python) by Zbigniew Zasieczny
- [ModuleInterface](https://github.com/fredilarsen/ModuleInterface) by Fred Larsen

PJON™ is a self-funded, no-profit open-source project created (in 2010) and mantained by Giovanni Blu Mitolo with the support ot the internet community if you want to see the PJON project growing with a faster pace, consider a donation at the following link: https://www.paypal.me/PJON

PJON™ and its brand are unregistered trademarks, property of Giovanni Blu Mitolo gioscarab@gmail.com
