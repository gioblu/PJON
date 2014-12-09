PJON 
====

PJON **(Padded Jittering Operative network)** is a multi-masterm, single wire, addressed and checked communication protocol for Arduino.
Every connected device has an ID (one byte address) and can communicate with any other device on the network with package correctness acknowledge. Every package sent is checked by a one byte Cyclic Redundancy Check and is addresses to a particular device, for this reason is practically impossible to miss a package (if the receiver will not receive the sender will know) or to receive uncorrect data.




