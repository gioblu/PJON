PJON 
====

PJON **(Padded Jittering Operative network)** is a multi-master, single wire, addressed and checked communication protocol for Arduino. Every Connected device has an ID (one byte address) and is able to communicate with any other device on the network. 
Communication is checked by a one byte Cyclic Redundancy Check or CRC and sender wait for ackowledge by receiver.
