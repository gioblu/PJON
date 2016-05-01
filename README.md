##PJON v2.0 
PJON (Padded Jittering Operative Network) is an Arduino compatible single wire, multi-master communications bus system implemented in  270 lines of C++ and Wiring (not considering comments). It is designed as an alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. If you are interested to know more about the PJON standard, visit the [Wiki](https://github.com/gioblu/PJON/wiki). If you need a wireless multimaster implementation check [PJON_ASK](https://github.com/gioblu/PJON_ASK). If you need help or something is not working visit the [Troubleshooting Wiki page](https://github.com/gioblu/PJON/wiki/Troubleshooting). If you own a Saleae Logic Analyzer you can scope communication with [saleae-pjon-protocol-analyzer](https://github.com/aperepel/saleae-pjon-protocol-analyzer) crafted by Andrew Grande.

[![Join the chat at https://gitter.im/gioblu/PJON](https://badges.gitter.im/gioblu/PJON.svg)](https://gitter.im/gioblu/PJON?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

```cpp  
#include <PJON.h> // Transmitter board code
PJON bus(12, 45); // Bus connection to pin 12, device id 45

void setup() {
  bus.begin(); // Initialize PJON bus
  bus.send(44, "B", 1, 1000000); // Send to 44 "B" of length 1 every second
}

void loop() {
  bus.update();
}

/* ---------------------------------------------------------------------------- */

#include <PJON.h> // Receiver board code
PJON bus(12, 44); // Bus connection to pin 12, device id 44

void setup() {
  bus.begin(); // Initialize PJON bus
  bus.set_receiver(receiver_function); // Set the function used to receive messages
};

void receiver_function(uint8_t length, uint8_t *payload) {
  if(payload[0] == 'B') { // If the first letter of the received message is B
    digitalWrite(13, HIGH);
    delay(30);
    digitalWrite(13, LOW);
  }
}

void loop() {
  bus.receive(1000);
}
```

####Features
- Software emulated non blocking implementation (based on `micros` and `delayMicroseconds`)
- Single wire (plus common ground) physical layer with up to 50 meters range.
- Device id implementation to enable univocal communication up to 254 devices.  
- Optional auto-addressing with id collision avoidance.
- CRC8 table-less cyclic redundancy check implementation.
- Acknowledgement of correct packet sending.
- Collision avoidance to enable multi-master capability.
- Broadcast functionality to contact all connected devices.
- Packet manager to track and retransmit failed packet sendings in background.
- Error handling.

####Compatibility
- ATmega88/168/328 8/16Mhz (Diecimila, Duemilanove, Uno, Nano, Mini, Lillypad)
- ATmega2560 16Mhz (Arduino Mega)
- ATmega16u4/32u4 (Arduino Leonardo)
- ATtiny45/85 8/16Mhz (Trinket, other ATtiny 85 boards), see [ATtiny Wiki page](https://github.com/gioblu/PJON/wiki/ATtiny-interfacing)
- SAMD (Arduino Zero)
- ESP8266 v.1-7 80Mhz "AI-THINKER AT" firmware, see [ESP8266 Arduino IDE](https://github.com/esp8266/Arduino)  
- ESP8266 NodeMCU v0.9-1.0 80Mhz, see [ESP8266 Arduino IDE](https://github.com/esp8266/Arduino)  

####Performance
PJON works in 3 different communication modes, `STANDARD`, `FAST` and `OVERDRIVE`:
- `STANDARD` mode runs at 16.944kBd or 2.12kB/s full cross-architecture / promiscuous clock compatible.
- `FAST` mode runs at 25.157kBd or 3.15kB/s full cross-architecture / promiscuous clock compatible.
- `OVERDRIVE` mode runs a specific architecture at its maximum limits (non cross-architecture compatible). Every architecture has its own limits, Arduino Duemilanove for example runs at 33.898kBd, Arduino Zero can reach 48.000kBd.

When including and using PJON, you have the complete access to the microntroller ready to be used, as usual, untouched. This happens because PJON is completely software emulated with a non blocking implementation, transforming a painfull walk to the hill in a nice flight.

Single wire simplicity let you to experiment quickly and with creativity. The first test I suggest, at your own risk, is to let two arduino boards communicate through your body touching with the left hand the digital port of the first board (5v 40ma, harmless) and with the right the port of the other one. Will be stunning to see high accuracy digital communication running inside a living biological body. This opens the mind to possible creative solutions.

####Why not I2c?
I2C is a bus system engineered to work with short wires to connect devices and it needs at least 2 wires, for those reasons is not feasible for home automation applications. If one of the connections to the bus fails, even briefly, one or both devices may freeze. For this reason i2c is not practical for high vibration scenarios such as automotive or robotic applications.

####Why not 1-Wire?
1-Wire is almost what I needed for a lot of projects but has its downsides: it is propietary, in my opinion is over-engineered and Arduino implementations are slow, chaotic and not reliable.

####Why not interrupts?
Usage of libraries is really extensive in the Arduino environment and often the end user is not able to go over collisions or redefinitions. Very often a library is using hardware resources of the microcontroller as timers or interrupts, colliding or interrupting other libraries. This happens because in general Arduino boards have limited hardware resources. To have a universal and reliable communication medium in this sort of environment, software emulated bit-banging, I think, is a good, stable and reliable solution that leads to "more predictable" results than interrupt driven systems coexisting on small microcontrollers without the original developer and the end user knowing about it.

![PJON - Michael Teeuw application example](http://33.media.tumblr.com/0065c3946a34191a2836c405224158c8/tumblr_inline_nvrbxkXo831s95p1z_500.gif)

PJON application example made by the user [Michael Teeuw](http://michaelteeuw.nl/post/130558526217/pjon-my-son)

====

#### How to start
The first step is the physical layer: connect with a wire two boards using a digital pin on both boards. After this you should have both arduino boards connected by the wire on the same pin. The selected pins are the same only for simplicity and to avoid mistakes, PJON works fine on every Arduino digital or analog (used as digital) I/O pin.

Lets start coding, instantiate the `PJON` object that in the example is called bus. To initialize a bus based on PJON you need only to define the communication pin (any free digital pin on your board) and a unique ID (0 - 255):

```cpp  
  PJON bus(12);
  bus.begin();     // initialize PJON bus
  bus.set_id(123); // Set id later

  // or

  PJON bus(12, 123);
  bus.begin();     // initialize PJON bus
```

If you are interested auto-addressing is really easy to use but still experimental:
```cpp  
  PJON bus(12);
  bus.begin();     // initialize PJON bus
  bus.acquire_id();
  Serial.println(bus.device_id()); // Device id found with scan
```
All ids are scanned sending a packet containing the `ACQUIRE_ID` constant. If no answer is received from an id, it is considered free.
If auto-addressing approach is your choice, you should never have a blind timeframe longer than 1.5 seconds (i.e. `delay(2000)`) between every `receive` function call. This constrain is imposed by the necessity of having the device able to receive incoming packets (as `ACQUIRE_ID` used to determine if a device id is free or not). If a device is executing something for too much time while not reading the bus for incoming packets, its device id could be stolen by another device. There is still no device id collision detection / correction, but respecting the described rules, collision should not happen.

====

#### Transmit data
Data transmission is handled by a packet manager, the `update()` function has to be called at least once per loop cycle. Consider that this is not an interrupt driven system, all the time dedicated to delays or executing other tasks is postponing the sending of all the packets are scheduled to be sent:

```cpp  
  bus.update();
```

To send a string to another device connected to the bus simply call `send()` function passing the ID you want to contact, the string you want to send and its length:

```cpp
bus.send(100, "Ciao, this is a test!", 21);
```
I know that the packet length is boring to fill but is there to prevent buffer overflow. If sending arbitrary values `NULL` terminator strategy based on `strlen()` is not safe to detect the end of a string.

To send a value repeatedly simply add as last parameter the interval in microseconds you want between every sending:

```cpp
int one_second_delay_test = bus.send(100, "Test sent every second!", 23, 1000000);
```

`one_second_delay_test` contains the id of the packet. If you want to remove this repeated task simply:

```cpp
bus.remove(one_second_delay_test);
```

To broadcast a message to all connected devices, use the `BROADCAST` constant as recipient ID.

```cpp
int broadcastTest = bus.send(BROADCAST, "Message for all connected devices.", 34);
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

Inform the bus to call `receiver_function` when a correct message is received:

```cpp
bus.set_receiver(receiver_function);
```

To correctly receive data call `receive()` function at least once per loop cycle passing as a parameter, the maximum reception time in microseconds:
```cpp
int response = bus.receive(1000);
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
- `ID_ACQUISITION_FAIL` (value 105), `data` parameter contains actual device id.

```cpp
void error_handler(uint8_t code, uint8_t data) {
  if(code == CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(data);
    Serial.println(" is lost.");
  }
  if(code == PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data, DEC);
    Serial.println("Possible wrong bus configuration!");
    Serial.println("higher MAX_PACKETS in PJON.h if necessary.");
  }
  if(code == MEMORY_FULL) {
    Serial.println("Packet memory allocation failed. Memory is full.");
  }
  if(code == CONTENT_TOO_LONG) {
    Serial.print("Content is too long, length: ");
    Serial.println(data);
  }
  if(code == ID_ACQUISITION_FAIL) {
    Serial.print("Can't acquire a free id ");
    Serial.println(data);
  }
}
```


Now inform the bus to call the error handler function in case of error:
```cpp
bus.set_error(error_handler);
```

====

####License

```cpp
/* Copyright 2012-2016 Giovanni Blu Mitolo

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
```
