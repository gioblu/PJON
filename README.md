
![PJON](http://www.gioblu.com/PJON/PJON-github-header-tiny.png)
## PJON 12.1
PJON® (Padded Jittering Operative Network) is an arduino-compatible, multi-master, multi-media network protocol. It proposes a new Open Standard, it is designed as a framework and implements a totally software-defined network protocol stack that can be easily cross-compiled on many MCUs and architectures like ATtiny, ATmega, SAMD, ESP8266, ESP32, STM32, Teensy, Raspberry Pi, Linux, Windows x86, Apple and Android. PJON operates on a wide range of media and protocols like TCP, UDP, MQTT, ESPNOW, USB, Serial, RS485, LoRa, PJDL, PJDLR and PJDLS.

[![Get PJON bus id](https://img.shields.io/badge/get-PJON%20bus%20id-lightgrey.svg)](http://www.pjon.org/get-bus-id.php)
[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

PJON is used in thousands of devices and its community has spread worldwide because of the following 5 key factors:
- **New technology**: [PJON](specification/PJON-protocol-specification-v3.2.md) is an experimental network protocol stack crafted in 10 years of research and experimentation. It was originally developed as an open-source alternative to i2c and 1-Wire but during development its scope and features have been extended to cover use cases where IP is generally applied. PJON has been engineered to have a variable footprint (4.2-8.2 kB program memory) and overhead (5-22 bytes per packet) depending on its configuration.
- **Multi-media support**: PJON operates on a wide range of media and protocols like TCP, UDP, MQTT, ESPNOW, USB, Serial, RS485 and LoRa. The PJON network protocol stack specifies and implements also [PJDL](src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.1.md) able to communicate data over a single wire shared by up to 255 devices, [PJDLR](src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md) able to operate with many ASK/FSK/OOK radio modules and also [PJDLS](src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md), able to communicate wirelessly with light pulses using off the shelf LEDs and laser diodes.
- **Increased reliability**: Many protocols massively applied worldwide expose dangerous vulnerabilities, have weak error detection algorithms and are not resilient to interference. PJON is based on years of analysis and study not to make the same mistakes present in most alternatives and provide with a set of simpler and more efficient solutions.
- **High flexibility**: PJON is totally software-defined and its implementation is designed to be easily extensible. it builds out-of-the-box in all supported devices and operates transparently on top of any supported protocol or medium.
- **Low cost**: Without any additional hardware needed to operate, minimal network wiring requirements and direct pin-to-pin or LED-to-LED communication, PJON is extremely energy efficient, cheap to be implemented and maintained. This implementation is kept updated and meticulously tested thanks to the strong commitment of its growing community of end users, testers and developers.

### Features
- Cross-compilation support with the [interfaces](src/interfaces) system calls abstraction   
- Multi-media support with the [strategies](src/strategies) data link layer abstraction
- Hot-swap support, no need of system reset or shut down when replacing or adding devices
- Configurable synchronous and/or asynchronous [acknowledgement](specification/PJON-protocol-acknowledge-specification-v1.0.md)
- Configurable 2 level addressing (device and bus id) for scalable applications
- Configurable 1 or 2 bytes packet length (max 255 or 65535 bytes)
- Configurable CRC8 or CRC32 table-less cyclic redundancy check
- Packet manager to handle, track and if necessary retransmit a packet sending in background
- Error handling

### Specifications
- [PJON v3.2](specification/PJON-protocol-specification-v3.2.md)
- [PJON Acknowledge v1.0](specification/PJON-protocol-acknowledge-specification-v1.0.md)
- [PJDL v4.1](src/strategies/SoftwareBitBang/specification/PJDL-specification-v4.1.md) - [PJDLR v3.0](src/strategies/OverSampling/specification/PJDLR-specification-v3.0.md) - [PJDLS v2.0](src/strategies/AnalogSampling/specification/PJDLS-specification-v2.0.md) - [TSDL v2.1](src/strategies/ThroughSerial/specification/TSDL-specification-v2.1.md)

### Publications
- [PJON protocol handbook](https://www.pjon-technologies.com/collections/frontpage/products/pjon-protocol-hand-book) by Giovanni Blu Mitolo - Distributed by [PJON Technologies srl](https://www.pjon-technologies.com)
- [PJON 12.0 big box](https://www.pjon-technologies.com/collections/frontpage/products/pjon-protocol-12-0-big-box) by Giovanni Blu Mitolo - Distributed by [PJON Technologies srl](https://www.pjon-technologies.com)

### Academic studies
Researchers are active in many universities worldwide using PJON in different environments. The following list contains all the known published academic studies about PJON:
- [Definition and Application of PJON-PLC for sensor networks](https://repositorio.unican.es/xmlui/bitstream/handle/10902/14012/408952.pdf?sequence=1) by Jorge Gómez Segurola, Ingeniería de Tecnologías de
Telecomunicación - [Universidad de Cantabria](https://web.unican.es/) (ES)
- [Biomimetic electronics](http://c.harl.ie/biomimetic.html) by Charlie Williams with scientific input from researchers Vítor Martins dos Santos, Diana Machado de Sousa and Sabine Vreeburg - Artist in Residency at [Wageningen University](https://www.wur.nl/en.htm) (NL)

### Contribute
Feel free to send a pull request sharing something you have made that could help, if you want to support this project you can also try to solve an [issue](https://github.com/gioblu/PJON/issues). Thanks to support, expertise, kindness and talent of the following contributors, the protocol's documentation, specification and implementation have been strongly tested, enhanced and verified:

[Fred Larsen](https://github.com/fredilarsen), [Zbigniew Zasieczny](https://github.com/girgitt), [Matheus Garbelini](https://github.com/Matheus-Garbelini), [sticilface](https://github.com/sticilface), [Felix Barbalet](https://github.com/xlfe), [Oleh Halitskiy](https://github.com/Halytskyi), [fabpolli](https://github.com/fabpolli), [Adrian Sławiński](https://github.com/4ib3r), [Osman Selçuk Aktepe](https://github.com/osman-aktepe), [Jorgen-VikingGod](https://github.com/Jorgen-VikingGod), [drtrigon](https://github.com/drtrigon), [Endre Karlson](https://github.com/ekarlso), [Wilfried Klaas](https://github.com/willie68), [budaics](https://github.com/budaics), [ibantxo](https://github.com/ibantxo), [gonnavis](https://github.com/gonnavis), [maxidroms83](https://github.com/maxidroms83), [Evgeny Dontsov](https://github.com/dontsovcmc), [zcattacz](https://github.com/zcattacz), [Valerii Koval](https://github.com/valeros), [Ivan Kravets](https://github.com/ivankravets), [Esben Soeltoft](https://github.com/EsbenSoeltoft), [Alex Grishin](https://github.com/240974a), [Andrew Grande](https://github.com/aperepel), [Michael Teeww](https://github.com/MichMich), [Paolo Paolucci](https://github.com/PaoloP74), [per1234](https://github.com/per1234), [Santiago Castro](https://github.com/bryant1410), [pacproduct](https://github.com/pacproduct), [elusive-code](https://github.com/elusive-code), [Emanuele Iannone](https://github.com/eiannone), [Christian Pointner](https://github.com/equinox0815), [Fabian Gärtner](https://github.com/TeeTrizZz), [Mauro Mombelli](https://github.com/MauroMombelli), [Remo Kallio](https://github.com/shacal), [hyndruide](https://github.com/hyndruide), [sigmaeo](https://github.com/sigmaeo), [filogranaf](https://github.com/filogranaf), [Maximiliano Duarte](https://github.com/domonetic), [Viktor Szépe](https://github.com/szepeviktor), [Shachar Limor](), [Pantovich](), [Mauro Zancarlin](), [Franketto](), [jzobac](), [DanRoad](), [fcastrovilli](https://github.com/fcastrovilli), [Andrei Volkau](https://github.com/andrei-volkau), [maniekq](https://github.com/maniekq), [DetAtHome](https://github.com/DetAtHome), [Michael Branson](https://github.com/mxbranson), [chestwood96](https://github.com/chestwood96), [Mattze96](https://github.com/Mattze96), [Steven Bense](https://github.com/justoke) and [Jack Anderson](https://github.com/jdaandersj).

### Compliant tools
- [ModuleInterface](https://github.com/fredilarsen/ModuleInterface) - easy config and value sync between IoT modules by Fred Larsen
- [PJON-cython](https://github.com/xlfe/PJON-cython) - cython PJON wrapper by xlfe github user
- [PJON-piper](https://github.com/Girgitt/PJON-piper) - command line wrapper by Zbigniew Zasieczny
- [PJON-python](https://github.com/Girgitt/PJON-python) - python interface by Zbigniew Zasieczny
- [PJON-gRPC](https://github.com/Halytskyi/PJON-gRPC) - gRPC server-client by Oleh Halytskyi

### License
All the software included in this project is experimental and it is distributed "AS IS" without any warranty, use it at your own risk. [Licensed](https://github.com/gioblu/PJON/blob/master/LICENSE.md) under the Apache License, Version 2.0. PJON® and its brand are registered trademarks, property of Giovanni Blu Mitolo gioscarab@gmail.com

### Safety warning
When installing or maintaining a PJON network, extreme care must be taken to avoid any danger. If devices are connected to AC power you are exposed to a high chance of being electrocuted if hardware is not installed carefully and properly. If you are not experienced enough ask the support of a skilled technician and consider that many countries prohibit uncertified installations. When a [SoftwareBitBang](/src/strategies/SoftwareBitBang) bus is installed [interference mitigation](https://github.com/gioblu/PJON/wiki/Mitigate-interference) and [protective circuitry](https://github.com/gioblu/PJON/wiki/Protective-circuitry) guidelines must be followed. When working with an [AnalogSampling](/src/strategies/AnalogSampling) LED or laser based setup safety glasses must be worn and transceivers must be operated cautiously to avoid potential eye injuries. Before any practical test or a hardware purchase for a wireless [OverSampling](/src/strategies/OverSampling), [ThroughSerial](/src/strategies/ThroughSerial) or [ThroughLoRa](/src/strategies/ThroughLoRa) radio setup, compliance with government requirements and regulations must be ensured. When connecting a local bus to the internet all devices must be considered potentially compromised, manipulated or remotely actuated against your will. It should be considered a good practice not to connect to the internet systems that may create a damage (fire, flood, data-leak) if hacked.
