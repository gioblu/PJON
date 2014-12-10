PJON 
====

PJON **(Padded Jittering Operative network)** is a multi-master, single wire, addressed and checked communication protocol for Arduino. Every Connected device has an ID (one byte address) and is able to communicate with any other device on the network. 
Communication is checked by a one byte Cyclic Redundancy Check or CRC and sender wait for ackowledge by receiver.


This architecture gives you really high communication reliability and acceptable speed for the vast majority of embedded projects. PJON can be efficiently used from home automation to automotive applications.

