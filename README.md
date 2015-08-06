PJON 
==== 
**Slow mode** Absolute bandwidth: **3.0kb/s** | Practical bandwidth: **2.38kb/s** | Accuracy: **99.25%**
**Standard mode** Absolute bandwidth: **3.44kb/s** | Practical bandwidth: **2.71kb/s** | Accuracy: **95.0%**
 
PJON **(Padded Jittering Operative Network)** is a multi-master, single wire, addressed and checked communication protocol and is designed to be an alternative to i2c, 1Wire, Serial and the other Arduino compatible protocols. 

Practically this library let you connect up to 255 Arduino together using only one common wire and obviously ground if not common. Every board is programmed with its own ID and can be reached by every other connected device. 

PJON contains also a packet manager / scheduler to track and retrasmit failed sendings in background, a reaction manager able to automatically call functions associated with user defined symbols and string encryption capability. Communication is checked by a one byte Cyclic Redundancy Check or CRC and sender wait for ackowledge by receiver. 

This architecture gives you really high communication reliability and acceptable speed for the vast majority of embedded projects. PJON can be efficiently used from home automation to automotive applications.

This single wire simplicity let you experiment fastly and with creativity. For example one of the first tests I advice to do with PJON is to let two arduinos communicate through your body touching with the left hand the port of the first and with the right the port of the other arduino. It's stunning to see more then 90% of accuracy for this digital communication doing all that path inside a biological body. This opens the mind to possible creative solutions; generally the average reaction is like: "lets use the car frame to let all the digital embedded systems to communicate together" and so on...

I am not still free from work and so I don't have now the time to write a proper documentation but I sware they will appear in this summer vacation.


