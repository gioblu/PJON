PJON 
====

PJON **(Padded Jittering Operative network)** is a multi-master, single wire, addressed and checked communication protocol for Arduino. Every Connected device has an ID (one byte address) and is able to communicate with any other device on the network. 
Communication is checked by a one byte Cyclic Redundancy Check or CRC and sender wait for ackowledge by receiver.


This architecture gives you really high communication reliability and acceptable speed for the vast majority of embedded projects. PJON can be efficiently used from home automation to automotive applications.

**PJON Slow mode**
Absolute bandwidth: 3.0 kb/s | Practical bandwidth: 2.38 kb/s | Accuracy: 99.25%\r\n
bit_width: 20 - bit_spacer: 68 - acceptance: 16 - read_delay: 9

**PJON Standard mode**
Absolute bandwidth: 3.44 kb/b | Practical bandwidth: 2.71 kb/s | Accuracy: 95.0%\r\n
bit_width 18: - bit_spacer: 36 - acceptance: 16 - read_delay: 7
