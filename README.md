
![PJON](http://www.gioblu.com/PJON/PJON-github-header-tiny.png)
##PJON v5.1
PJON (Padded Jittering Operative Network) is an Arduino compatible, multi-master, multi-media communications bus system. It proposes a Standard and it is designed as a framework for digital communication. It is a valid alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. Visit the [Wiki](https://github.com/gioblu/PJON/wiki), [Documentation](https://github.com/gioblu/PJON/wiki/Documentation) and [Troubleshooting](https://github.com/gioblu/PJON/wiki/Troubleshooting) wiki pages to know more about the PJON Standard.

[![Get PJON bus id](https://img.shields.io/badge/GET-PJON%20bus%20id-lightgrey.svg)](http://www.pjon.org/get-bus-id.php)
[![Video introduction](https://img.shields.io/badge/PJON-video%20introduction-blue.svg)](https://www.youtube.com/watch?v=vjc4ZF5own8)
[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![Donate](http://img.shields.io/paypal/donate.png?color=brightgreen)](https://www.paypal.me/PJON)

####Features
- Multi-media compatible data link layer or [Strategy](https://github.com/gioblu/PJON/wiki/Strategies)
- Configurable 2 level addressing (device and bus id) for scalable applications
- Acknowledgement of correct packet sending
- Collision avoidance to enable multi-master capability
- CRC8 table-less cyclic redundancy check implementation
- Packet manager to handle, track and if necessary retransmit a packet sending in background
- Error handling

####PJON Protocol layer specification
- [PJON protocol layer v0.1](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md)
- [PJON protocol layer v0.2](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md)
- [PJON protocol layer v0.3](https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md)

####Padded jittering data link layer specification
- [Padded jittering data link layer v0.1](https://github.com/gioblu/PJON/blob/master/strategies/SoftwareBitBang/specification/padded-jittering-protocol-specification-v0.1.md)

####Compliant tools
- [saleae-pjon-protocol-analyzer](https://github.com/aperepel/saleae-pjon-protocol-analyzer) by Andrew Grande.
- [PJON-python](https://github.com/Girgitt/PJON-python) by Zbigniew Zasieczny.
