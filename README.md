##PJON v1.1 stable
PJON (Padded Jittering Operative Network) is an Arduino compatible single wire, multi-master communication bus system implemented in  270 lines of C++ and Wiring (not considering comments). It is designed as an alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. If you are interested to know more about the PJON standard, visit the [Wiki](https://github.com/gioblu/PJON/wiki). If you need a wireless multimaster implementation check [PJON_ASK](https://github.com/gioblu/PJON_ASK). If you need help or something is not working visit the [Troubleshooting Wiki page](https://github.com/gioblu/PJON/wiki/Troubleshooting).

```cpp  
#include <PJON.h>     // Transmitter board code
PJON network(12, 45); // Bus connection to pin 12, device id 45

void setup() {
  network.send(44, "B", 1, 1000000);
  // Send to device 44, "B" content of 1 byte length every 1000000 microseconds (1 second)
}

void loop() {
  network.update();
}

/* ---------------------------------------------------------------------------- */

#include <PJON.h>     // Receiver board code
PJON network(12, 44); // Bus connection to pin 12, device id 44

void setup() {
  network.set_receiver(receiver_function); // Set the function used to receive messages
};

void receiver_function(uint8_t length, uint8_t *payload) {
  if(payload[0] == 'B') { // If the first letter of the received message is B
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
}

void loop() {
  network.receive(1000);
}
```

####Features
- Software emulated non blocking implementation (based on `micros` and `delayMicroseconds`)
- Single wire (plus common ground) physical layer with up to 50 meters range.
- Device id implementation to enable univocal communication up to 254 devices.  
- Lightweight 1 byte XOR based error detection.
- Acknowledgement of correct packet sending.
- Collision avoidance to enable multi-master capability.
- Broadcast functionality to contact all connected devices.
- Packet manager to track and retransmit failed packet sendings in background.
- Error handling.

####Performance
- Transfer speed: 42372Bd or 5.29kB/s
- Data throughput: 2.68kB/s
- Accuracy: 99.995%

####Compatibility
- ATmega88/168/328 8/16Mhz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16Mhz (Arduino Mega)
- ATtiny45/85 8/16Mhz (Trinket, other ATtiny 85 boards), see [wiki](https://github.com/gioblu/PJON/wiki/ATtiny-interfacing)
- ESP8266 v.1-7 80Mhz "AI-THINKER AT" firmware, see [ESP8266 Arduino IDE](https://github.com/esp8266/Arduino) and [wiki](https://github.com/gioblu/PJON/wiki/ESP8266-interfacing)   
- ESP8266 NodeMCU v0.9-1.0 80Mhz, see [ESP8266 Arduino IDE](https://github.com/esp8266/Arduino) and [wiki](https://github.com/gioblu/PJON/wiki/ESP8266-interfacing)    

PJON by default runs in `COMPATIBILITY_MODE` at 1.78 kB/s or 14240 baud to offer reliable communication between every supported device; some are clocked to 8Mhz and so not able to run PJON at full speed,  `COMPATIBILITY_MODE` timing values are now really conservative and probably will be changed during development allowing a faster standard cross-architecture communication speed. If you have a network made of 16Mhz devices with the same architecture / processor you can set `COMPATIBILITY_MODE` to false in `PJON.h` and run your network at full speed (5.29 kB/s or 42372 baud).

When including and using PJON, you have the complete access to the microntroller ready to be used, as usual, untouched. This happens because PJON is completely software emulated with a non blocking implementation, transforming a painfull walk to the hill in a nice flight.

Single wire simplicity let you to experiment quickly and with creativity. The first test I suggest, at your own risk, is to let two arduino boards communicate through your body touching with the left hand the digital port of the first board (5v 40ma, harmless) and with the right the port of the other one. Will be stunning to see high accuracy digital communication running inside a living biological body. This opens the mind to possible creative solutions.

####Why not I2c?
I2C is a bus system engineered to work with short wires to connect devices and it needs at least 2 wires, for those reasons is not feasible for home automation applications. If one of the connections to the bus fails, even briefly, one or both devices may freeze. For this reason i2c is not practical for high vibration scenarios such as automotive or robotic applications.

####Why not 1-Wire?
1-Wire is almost what I needed for a lot of projects but has its downsides: it is propietary, in my opinion is over-engineered and Arduino implementations are slow, chaotic and not reliable.


![PJON - Michael Teeuw application example](http://33.media.tumblr.com/0065c3946a34191a2836c405224158c8/tumblr_inline_nvrbxkXo831s95p1z_500.gif)

PJON application example made by the user [Michael Teeuw](http://michaelteeuw.nl/post/130558526217/pjon-my-son)

====

#### How to start
The first step is the physical layer: connect with a wire two boards using a digital pin on both boards. After this you should have both arduino boards connected by the wire on the same pin. The selected pins are the same only for simplicity and to avoid mistakes, PJON works fine on every Arduino digital or analog (used as digital) I/O pin.

Lets start coding, instantiate the `PJON` object that in the example is called network. To initialize a network based on PJON you need only to define the communication pin (any free digital pin on your board) and a unique ID (0 - 255):

```cpp  
  PJON network(12);
  network.set_id(123); // Set id later

  // or

  PJON network(12, 123);
```

====

#### Transmit data
Data transmission is handled by a packet manager, the `update()` function has to be called at least once per loop cycle. Consider that this is not an interrupt driven system, all the time dedicated to delays or executing other tasks is postponing the sending of all the packets are scheduled to be sent:

```cpp  
  network.update();
```

To send a string to another device connected to the bus simply call `send()` function passing the ID you want to contact, the string you want to send and its length:

```cpp
network.send(100, "Ciao, this is a test!", 21);
```
I know that the packet length is boring to fill but is there to prevent buffer overflow. If sending arbitrary values `NULL` terminator strategy based on `strlen()` is not safe to detect the end of a string.

To send a value repeatedly simply add as last parameter the interval in microseconds you want between every sending:

```cpp
int one_second_delay_test = network.send(100, "Test sent every second!", 23, 1000000);
```

`one_second_delay_test` contains the id of the packet. If you want to remove this repeated task simply:

```cpp
network.remove(one_second_delay_test);
```

To broadcast a message to all connected devices, use the `BROADCAST` constant as recipient ID.

```cpp
int broadcastTest = network.send(BROADCAST, "Message for all connected devices.", 34);
```

====

#### Receive data
Now define a `void function` that will be called if a correct message is received. This function receives 2 parameters: the message length and its content.

```cpp
void receiver_function(uint8_t length, uint8_t *payload) {
  Serial.print("Message content: ");

  for(int i = 0; i < length; i++)
    Serial.print((char)payload[i]);

  Serial.print(" | Message length: ");
  Serial.println(length, DEC);
};
```

Inform the network to call `receiver_function` when a correct message is received:

```cpp
network.set_receiver(receiver_function);
```

To correctly receive data call `receive()` function at least once per loop cycle passing as a parameter, the maximum reception time in microseconds:
```cpp
int response = network.receive(1000);
```

Consider that this is not an interrupt driven system and so all the time passed in delay or executing something a certain amount of packets will be potentially lost unheard. Structure intelligently your loop cycle to avoid huge blind timeframes.

====

####Error handling
PJON is designed to inform the user if an error is detected. A `void function` has to be defined as the error handler, it receives 2 parameters the first is the error code and the second is 1 byte additional data related to the error.

Error types:
- `CONNECTION_LOST` (value 101), `data` parameter contains lost device's id.
- `PACKETS_BUFFER_FULL` (value 102), `data` parameter contains buffer length.
- `MEMORY_FULL` (value 103), `data` parameter contains `FAIL`.
- `CONTENT_TOO_LONG` (value 104), `data` parameter contains content length.

```cpp
void error_handler(uint8_t code, uint8_t data) {
  if(code == CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(data, DEC);
    Serial.println(" is lost.");
  }
  if(code == PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data, DEC);
    Serial.println("Possible wrong network configuration!");
    Serial.println("For high complexity networks higher MAX_PACKETS over 10.");
    Serial.println("See in PJON.h");
  }
  if(code == MEMORY_FULL) {
    Serial.println("Packet memory allocation failed. Memory is full.");
  }
  if(code == CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data, DEC);
  }
}
```


Now inform the network to call the error handler function in case of error:
```cpp
network.set_error(error_handler);
```

====

####License

```cpp
/* Copyright (c) 2012-2016, Giovanni Blu Mitolo All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
- Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

-  Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

-  All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes PJON software developed by Giovanni Blu Mitolo.

-  Neither the name of PJON, PJON_ASK nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

This software is provided by the copyright holders and contributors"as is"
and any express or implied warranties, including, but not limited to, the
implied warranties of merchantability and fitness for a particular purpose
are disclaimed. In no event shall the copyright holder or contributors be
liable for any direct, indirect, incidental, special, exemplary, or consequential
damages (including, but not limited to, procurement of substitute goods or services;
loss of use, data, or profits; or business interruption) however caused and on any
theory of liability, whether in contract, strict liability, or tort (including
negligence or otherwise) arising in any way out of the use of this software, even if
advised of the possibility of such damage. */
```
