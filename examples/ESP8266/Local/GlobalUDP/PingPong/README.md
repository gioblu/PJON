# ESP8266 PingPong example

This setup consists of a Transmitter and a Receiver. The Transmitter will regularly send a small packet to the Receiver, and the Receiver will reply back every time.

This setup demonstrates how only one device has a fixed IP address while the other is using DHCP to get the network setup and IP address assigned dynamically. Additional DHCP based Transmitters can be added to the setup, requiring only the PJON id to be different for each one.

The Receiver has a fixed IP address. Please edit Receiver.ino to insert your actual network setup and an unused IP address, preferrably outside of your DHCP pool.
Both sketches should be edited to contain the SSID and password of your WiFi network.

The Transmitter is set up to use DHCP to request a dynamic IP address. When the first packet from the Transmitter reaches the Receiver, the Transmitter's assigned IP address will be registrered in the Receiver's node table so that the Receiver can send to it by PJON id.

If the Transmitter should be restarted and get a new IP address, the first packet that reaches the Receiver after this will update the registration in the Receiver so that the communication continues.
