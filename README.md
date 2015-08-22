PJON 
==== 
 
PJON **(Padded Jittering Operative Network)** is a multi-master, single wire, addressed and checked communication protocol and is designed to be an alternative to i2c, 1Wire, Serial and the other Arduino compatible protocols. 
* **Slow mode** Absolute bandwidth: **3.0kb/s** | Practical bandwidth: **2.38kb/s** | Accuracy: **99.25%**
* **Standard mode** Absolute bandwidth: **3.44kb/s** | Practical bandwidth: **2.71kb/s** | Accuracy: **95.0%**
(PJON is not interrupt driven, this speed is obtained using only delayMicroseconds() and micros() as timing source)

This library let you connect up to 255 Arduino together using only one common wire and ground. Every board programmed with PJON and so with its own ID can be reached by every other connected device. 

PJON contains also a packet manager / scheduler to track and retrasmit failed sendings in background, a reaction manager able to automatically call functions associated with user defined symbols and string encryption capability. Communication is checked by a one byte Cyclic Redundancy Check or CRC and sender wait for ackowledge by receiver. 

This architecture gives you really high communication reliability and acceptable speed for the vast majority of embedded projects. PJON can be efficiently used from home automation to automotive applications.

This single wire simplicity let you experiment fastly and with creativity. For example one of the first tests I suggest to try with PJON is to let two arduinos communicate through your body touching with the left hand the port of the first and with the right the port of the other arduino. It's stunning to see more then 90% of accuracy for this digital communication doing all that path inside a biological body. This opens the mind to possible creative solutions; generally the average reaction is like: "lets use the car frame to let all the digital embedded systems to communicate together" and so on...

## How to start

The first step is the physical layer. Lets wire the two arduinos. Simply find a piece of wire and select a digital pin you want to use on both where to connect the wire. After this you should have both arduino boards connected by a piece of wire on the same port.

Lets start coding,  instantiate the PJON object that in the example is called network. To initialize a network based on PJON you need only to define the communication pin (any free digital pin on your board) and a unique ID (0 - 254):

```cpp  
  PJON network(12, 123); 
```

After this is necessary to define wich type of communication we need. For example if you only need to send a value from a "slave" arduino to another one cyclically, you will probably not need collision_avoidance (to check if someone is speaking before starting a communication) and acknowledge or ACK (when a sender waits the receiver acknowledge of correct data receive after CRC check of data integrity). Disabling this two functionalities let you spare loop time and so speed up communication. In the opposite case, if you have two or more boards communicating bidirectionally as my automated home scenario, you will need to have a system able to speak over the channel when is free to avoid to corrupt some other communication going on and also to have every board sure that the message sent arrived correct to the receiver. There are some setters that let you configure the network as you need:

```cpp
  // Master Slave capable setup
  void setup() {
   network.set_collision_avoidance(false);
   network.set_acknowledge(false);
  }; 
  
  // Multimaster capable setup
  void setup() {
   network.set_collision_avoidance(true);
   network.set_acknowledge(true);
  }; 
```

If you need to keep all the network communication inside the PJON bus secure from possible external listeners you can use the encryption function contained in the library that is pratically the [Cape](https://github.com/gioblu/Cape) string encryption library I wrote pasted inside (infact this is an issue I should fix):

```cpp
  void setup() {
   network.set_encryption(true);
  }; 
```


To let the network work correctly you need to call the update() function at least once per loop cycle. It's really important to consider that this is not an interrupt driven system, so all the time the Arduino is passing delaying time or executing other tasks is delaying the sending of all the packets are scheduled to be sent:

```cpp  
  network.update(); 
```

## Transmit data

To send a string to another device connected to the bus simply call send() function passing the ID you want to contact and the string you want to send:

```cpp
int test = network.send(100, "Ciao, this is a test!");
```

if you need to send a value repeatedly simply add after the first parameter the interval in microseconds you want between every sending:

```cpp
int one_second_delay_test = network.send(100, "Ciao, test sent every second!", 1000000);
```

If you want to remove this repeated task simply:

```cpp
network.remove(one_second_delay_test);
```

The state of your packet can be -1 that means is still in pending state, if NULL the packet was correctly sent and deleted. if you want to know the state of your packet:

```cpp
int state = network.packets[one_second_delay_test].state;
```

You can also send a command with a predefined symbol to the sender that will trigger a predefined function:

```cpp
void loop() {
 int blink_send = network.send_short_command(100, 'B');
 delay(1000);
}
```

### Broadcast

To broadcast a message to all connected devices, use the `BROADCAST` constant as the recipient ID. Every node will receive the message but will not answer ACK to avoid communication overlap.

```cpp
int multicastTest = network.send(BROADCAST, "Message for all connected devices.");
```

## Receive data

If you defined in another board a command that the receiver board you are programming should receive you can define a reaction to that command that will trigger a function that will be called as soon as the command will be received:

```cpp
void setup() {
 network.insert_reaction('B', led_blink);
};

void led_blink() {
 digitalWrite(13, HIGH);
 delay(30);
 digitalWrite(13, LOW);
};

void loop() {
 network.receive(1000);
};
```

To correctly receive data is necessary to call at least once per loop cycle the receive() function. Its important to consider that this is not an interrupt driven system and so all the time Arduino will pass in delay or executing something a certain amount of packets will be potentially lost unheard by the busy receiver. In this case structure intelligently your loop cicly to avoid huge blind timeframes. The receive function returns a response that contains the result of the reception. If you use packet and reaction manager this is not strictly necessary but useful to know:

```cpp
int response = network.receive();
if(response == ACK)
 Serial.println("Correctly received");
if(response == NAK)
 Serial.println("Received not correct");
if(response == BUSY)
 Serial.pritnln("Received packet for another device");
if(response == FAIL)
 Serial.println("No data");
```

If you want to dedicate exusively a determined amount of time only to receive data from the PJON bus use:

```cpp
int response = network.receive(1000000);
// Receive for one second
```

Here is a working example of the blink_test you can find in the examples directory:

[![Alt text for your video](http://img.youtube.com/vi/JesqJ9_WJJs/0.jpg)](http://www.youtube.com/watch?v=JesqJ9_WJJs)

