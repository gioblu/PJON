PJON 
====

PJON **(Padded Jittering Operative network)** is a multi-master, single wire, addressed and checked communication protocol and is designed to be an alternative to i2c, 1Wire, Serial and the other Arduino compatible protocols.

Every Connected device has an ID (one byte address) and is able to communicate with any other device on the network. 
Communication is checked by a one byte Cyclic Redundancy Check or CRC and sender wait for ackowledge by receiver.

This architecture gives you really high communication reliability and acceptable speed for the vast majority of embedded projects. PJON can be efficiently used from home automation to automotive applications.

* **Slow mode** Absolute bandwidth: **3.0kb/s** | Practical bandwidth: **2.38kb/s** | Accuracy: **99.25%**
* **Standard mode** Absolute bandwidth: **3.44kb/s** | Practical bandwidth: **2.71kb/s** | Accuracy: **95.0%**
