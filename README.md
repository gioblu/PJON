PJON v1.0 stable
==== 
 
PJON (Padded Jittering Operative Network) is a single wire, multi-master communication bus system. It is designed as an alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. If you are interested to know more about the PJON standard, visit the [wiki](https://github.com/gioblu/PJON/wiki). If you need a wireless multimaster implementation check [PJON_ASK](https://github.com/gioblu/PJON_ASK).

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

static void receiver_function(uint8_t length, uint8_t *payload) {
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
- Cyclic Redundancy Check (lightweight one byte XOR based CRC).
- Acknowledgement of correct packet sending. 
- Collision avoidance to enable multi-master capability.
- Broadcast functionality to contact all connected devices.
- Packet manager to track and retransmit failed packet sendings in background.
- Error handling.

####Performance
- Transfer speed: **42372Bd** or **5.29kB/s** 
- Data throughput: **2.68kB/s** 
- Accuracy: **99.995%**

####Compatibility
- Arduino Diecimila / Duemilanove
- Arduino Mini
- Arduino Uno
- Arduino Nano
- Arduino Mega

Single wire simplicity let you to experiment quickly and with creativity. For example, one of the first tests I suggest to try with PJON is to let two arduinos communicate through your body touching with the left hand the port of the first board and with the right the port of the other one. It's stunning to see more than 90% accuracy for this digital communication doing all that path inside a biological body. This opens the mind to possible creative solutions.

####Why not I2c?
I2C is a bus system engineered to work with short wires to connect devices and it needs at least 2 wires. No long wires can be used, this excludes home automation applications. If one of the connections to the bus fails, even briefly, one or both devices may freeze. For this reason i2c is not practical for high vibration scenarios such as automotive applications.

####Why not 1-Wire?
1-Wire is almost what I needed for a lot of projects but has its downsides: first it is propietary and closed source code and second, for me, the implementation is a littlebit chaotic.

## How to start
The first step is the physical layer. Lets wire the two arduinos. Simply find a piece of wire and select a digital pin you want to use on both boards where to connect the wire. After this you should have both arduino boards connected by the wire on the same pin.

Lets start coding, instantiate the `PJON` object that in the example is called network. To initialize a network based on PJON you need only to define the communication pin (any free digital pin on your board) and a unique ID (0 - 255):

```cpp  
  PJON network(12, 123); 
```

## Transmit data
Data transmission is handled by a packet manager, the `update()` function has to be called at least once per loop cycle. Consider that this is not an interrupt driven system, all the time dedicated to delays or executing other tasks is postponing the sending of all the packets are scheduled to be sent:

```cpp  
  network.update(); 
```

To send a string to another device connected to the bus simply call `send()` function passing the ID you want to contact, the string you want to send and its length:

```cpp
network.send(100, "Ciao, this is a test!", 21);
```
Packet length is there to prevent unwanted buffer overflows. If sending arbitrary values  `NULL` terminator strategy (strlen) is not safe to detect the end of a string. 

To send a value repeatedly simply add as last parameter the interval in microseconds you want between every sending:

```cpp
int one_second_delay_test = network.send(100, "Test sent every second!", 23, 1000000);
```

`one_second_delay_test` contains the id of the packet. If you want to remove this repeated task simply:

```cpp
network.remove(one_second_delay_test);
```

To broadcast a message to all connected devices, use the `BROADCAST` constant as the recipient ID. Every node will receive the message but will not answer `ACK` to avoid communication overlap.

```cpp
int broadcastTest = network.send(BROADCAST, "Message for all connected devices.", 34);
```

## Receive data
Now define a `static void function` that will be called if a correct message is received. This function receives 2 parameters: the message length and its content.

```cpp
static void receiver_function(uint8_t length, uint8_t *payload) {
  Serial.print("Message content: ");

  for(int i = 0; i < length; i++) 
    Serial.print((char)payload[i]);
  
  Serial.print(" | Message length: ");
  Serial.println(length);
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


##Error handling
PJON is designed to inform the user if the communication link to a certain device is lost or if the packtes buffer is full. A `static void function` has to be defined as the error handler, it receives 2 parameters the first is the error code and the second is 1 byte additional data related to the error.

Error types:
- `CONNECTION_LOST` (value 101), `data` parameter contains lost device's id.
- `PACKETS_BUFFER_FULL` (value 102), `data` parameter contains buffer length.
- `MEMORY_FULL` (103), `data` parameter contains `FAIL`.


```cpp
static void error_handler(uint8_t code, uint8_t data) {
  if(code == CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(data);
    Serial.println(" is lost.");
  }
  if(code == PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data);
    Serial.println("Possible wrong network configuration!");
    Serial.println("For high complexity networks higher MAX_PACKETS over 10.");
    Serial.println("See in PJON.h");
  }
  if(code == MEMORY_FULL) {
    Serial.println("Packet memory allocation failed. Memory is full.");
  }
}
```


Now inform the network to call the error handler function in case of error:
```cpp
network.set_error(error_handler);
```
---

Copyright (c) 2012-2015, Giovanni Blu Mitolo
All rights reserved.

This software is provided by the copyright holders and contributors "as is" and any express or implied warranties, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose are disclaimed. In no event shall the copyright holder or contributors be liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, procurement of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on any theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out of the use of this software, even if advised of the possibility of such damage.
