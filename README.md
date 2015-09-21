PJON 
==== 
 
PJON (Padded Jittering Operative Network) is a single wire, multi-master communication bus system. It is designed as an alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. 

```cpp  
#include <PJON.h>     // Transmitter board code
PJON network(12, 45); // Bus connection to pin 12, device id 45

void setup() {
  network.send(44, "B", 1000000); // Send B to device 44 every second
}

void loop() {
  network.update();
}

/* ---------------------------------------------------------------------------- */

#include <PJON.h>     // Receiver board code
PJON network(12, 44); // Bus connection to pin 12, device id 45

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
- Single wire physical layer with up to 50 meters range.
- Device id implementation to enable univocal communication up to 254 devices.  
- Cyclic Redundancy Check (CRC).
- Acknowledgement of correct packet sending. 
- Collision avoidance to enable multi-master capability.
- Broadcast functionality to contact all connected devices.
- Packet manager to track and retransmit failed packet sendings in background.
- Exception handling.

####Performance
- Transfer speed: **32256 baud/s** or **4.32kB/s** 
- Absolute bandwidth: **2.8kB/s** 
- Practical bandwidth: **2.38kB/s**
- Accuracy: **99.45-99.95%**

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

To send a string to another device connected to the bus simply call `send()` function passing the ID you want to contact and the string you want to send:

```cpp
network.send(100, "Ciao, this is a test!");
```

To send a value repeatedly simply add as last parameter the interval in microseconds you want between every sending:

```cpp
int one_second_delay_test = network.send(100, "Test sent every second!", 1000000);
```

`one_second_delay_test` contains the id of the packet. If you want to remove this repeated task simply:

```cpp
network.remove(one_second_delay_test);
```

To broadcast a message to all connected devices, use the `BROADCAST` constant as the recipient ID. Every node will receive the message but will not answer `ACK` to avoid communication overlap.

```cpp
int broadcastTest = network.send(BROADCAST, "Message for all connected devices.");
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


##Exception handling
PJON is designed to inform you if the communication link to a certain device is lost or if the packtes buffer is full. A `static void function` has to be defined as the exception handler, it receives 2 parameters the first is the motivation of the exception and the second is 1 byte additional data related to the error.

```cpp
static void exception_handler(uint8_t motivation, uint8_t data) {
  if(motivation == CONNECTION_LOST) {
    Serial.print("Connection with device ID ");
    Serial.print(data);
    Serial.println(" is lost.");
  }
  if(motivation == PACKETS_BUFFER_FULL) {
    Serial.print("Packet buffer is full, has now a length of ");
    Serial.println(data);
    Serial.println("Possible wrong network configuration!");
    Serial.println("For high complexity networks higher MAX_PACKETS over 10.");
    Serial.println("See in PJON.h");
  }
}
```


Now inform the network to call the exception handler function in case of error:
```cpp
network.set_exception(exception_handler);
```

