PJON 
==== 
 
PJON (Padded Jittering Operative Network) is a single wire, multi-master communication bus system. It is designed as an alternative to i2c, 1-Wire, Serial and other Arduino compatible protocols. 

```cpp 
// Instantiate PJON network passing the bus PIN and the device ID
PJON network(A0, 44);
 
// Configure both RX and TX devices in setup()
network.set_acknowledge(true);
network.set_collision_avoidance(true);

// Send temperature to device ID 44
byte temperature = 34;
network.send(44,  (char)temperature);

// Receive the temperature on the other side
network.receive(1000);
Serial.println(network.data[2]); // -> 34
```

####Features
- ID functionality to enable univocal communication up to 254 devices on the same wire.  
- Cyclic Redundancy Check (CRC).
- Acknowledgement of correct packet sending. 
- Collision avoidance to enable multi-master capability.
- Broadcast functionality to contact all connected devices.
- Packet manager to track and retransmit failed packet sendings in background.
- Reaction manager able to automatically call functions associated with user defined symbols.
- Optional encyption. 

####Performance
Absolute bandwidth: **3.0kB/s** - Practical bandwidth: **2.38kB/s** - Transfer speed: **39200 baud/s**  

This single wire simplicity enables you to experiment quickly and with creativity. For example, one of the first tests I suggest to try with PJON is to let two arduinos communicate through your body touching with the left hand the port of the first board and with the right the port of the other one. It's stunning to see more than 90% accuracy for this digital communication doing all that path inside a biological body. This opens the mind to possible creative solutions.

####Why not I2c?
I2C is a bus system engineered to work with short wires to connect devices and it needs at least 2 wires. No long wires can be used, this excludes home automation applications. If one of the connections to the bus fails, even briefly, one or both devices may freeze. For this reason i2c is not practical for high vibration scenarios such as automotive applications.

## How to start
The first step is the physical layer. Lets wire the two arduinos. Simply find a piece of wire and select a digital pin you want to use on both boards where to connect the wire. After this you should have both arduino boards connected by the wire on the same pin.

Lets start coding, instantiate the PJON object that in the example is called network. To initialize a network based on PJON you need only to define the communication pin (any free digital pin on your board) and a unique ID (0 - 255):

```cpp  
  PJON network(12, 123); 
```

After this is necessary to define wich type of communication we need. For example if you only need to send a value from a "slave" arduino to another one cyclically, you will probably not need collision_avoidance (to check if someone is speaking before starting a communication) and acknowledge or ACK (when a sender waits the receiver acknowledge of correct data receive after check of data integrity). Disabling this two functionalities let you spare loop time and so speed up communication. In the opposite case, if you have two or more boards communicating bidirectionally as my automated home scenario, you will need to have a system able to speak over the channel when is free to avoid to corrupt other communication going on and also to have every board sure that the message sent arrived correct to the receiver. There are some setters that let you configure the network as you need:

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
int one_second_delay_test = network.send(100, "Test sent every second!", 1000000);
```

If you want to remove this repeated task simply:

```cpp
network.remove(one_second_delay_test);
```

The state of your packet can be `TO_BE_SENT` that means is still in pending state, if NULL the packet was correctly sent and deleted. if you want to know the state of your packet:

```cpp
int state = network.packets[one_second_delay_test].state;
```

You can also send a command with a symbol to the sender that will trigger a function:

```cpp
char packet[2] = {CMD, 'B', NULL};
int blink_send = network.send(100, packet);
```

#### Broadcast
To broadcast a message to all connected devices, use the `BROADCAST` constant as the recipient ID. Every node will receive the message but will not answer ACK to avoid communication overlap.

```cpp
int broadcastTest = network.send(BROADCAST, "Message for all connected devices.");
```

## Receive data
To correctly receive data is necessary to call at least once per loop cycle the receive() function passing as a parameter the maximum reception time in microseconds:

```cpp
int response = network.receive(1000);
```
It is important to consider that this is not an interrupt driven system and so all the time Arduino will pass in delay or executing something a certain amount of packets will be potentially lost unheard by the busy receiver. In this case structure intelligently your loop cicle to avoid huge blind timeframes. The receive function returns a response that contains the result of the reception. If you use packet and reaction manager this is not strictly necessary but useful to know:

```cpp
int response = network.receive(1000);
if(response == ACK)
 Serial.println("Correctly received");
if(response == NAK)
 Serial.println("Received not correct");
if(response == BUSY)
 Serial.pritnln("Received packet for another device");
if(response == FAIL)
 Serial.println("No data");
```

To read the packet access to `network.data` array with max length of `max_package_length`:
```cpp
for(int i = 0; i < max_package_length; i++)
  Serial.println(network.data[i]);
```

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

Here is a working example of the blink_test you can find in the examples directory:

[![Alt text for your video](http://img.youtube.com/vi/JesqJ9_WJJs/0.jpg)](http://www.youtube.com/watch?v=JesqJ9_WJJs)

