PJON 
====

PJON **(Padded Jittering Operative Network)** is a multi-master, single wire, addressed and checked communication protocol and is designed to be an alternative to i2c, 1Wire, Serial and the other Arduino compatible protocols. Practically this library let you connect up to 255 Arduino together using only one common wire and obviously ground if not common. Every board is programmed with its own ID and can be reached by every other connected device. The library contains also a packet manager / scheduler to track and retrasmit failed sendings in background, a reaction manager able to automatically call functions associated with user defined symbols and string encryption capability. Communication is checked by a one byte Cyclic Redundancy Check or CRC and sender wait for ackowledge by receiver. This architecture gives you really high communication reliability and acceptable speed for the vast majority of embedded projects. PJON can be efficiently used from home automation to automotive applications.

* **Slow mode** Absolute bandwidth: **3.0kb/s** | Practical bandwidth: **2.38kb/s** | Accuracy: **99.25%**
* **Standard mode** Absolute bandwidth: **3.44kb/s** | Practical bandwidth: **2.71kb/s** | Accuracy: **95.0%**
