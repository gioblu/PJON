
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \|
   |y"s§+`\     Giovanni Blu Mitolo 2012 - 2015
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
   |/:%&-a3f.:/\     PJON is a device communications bus system that connects up to 255
    \+//u/+gosv//\    arduino boards over a single wire and provides up to 5KB/s data communication
     \o+&/osw+odss\\   with acknowledge, collision detection, CRC and encpryption all done
       \:/+-.§°-:+oss\  with micros() and delayMicroseconds(), without using interrupt or timers.
        | |       \oy\\   Pull down resistor on the bus is generally used to reduce interference.
        > <
       -| |-

DEVICE ID: 255 different adresses can be assigned
CRC: XOR Cyclic Redundancy Check ensures almost errorless data communication
ACKNOLEDGE: Acknowledge byte sent by receiver ensures packet delivery
COLLISION DETECTION: Collision avoidance is ensured analyzing network bus before starting */

#include "PJON.h"

/* Initiate PJON passing pin number and the selected device_id 

   PJON network(13, 46);
*/

PJON::PJON(int input_pin, uint8_t device_id) {
  _input_pin = input_pin;
  _device_id = device_id;

  for(int i = 0; i < MAX_PACKETS; i++) {
    packets[i].state = NULL;
    packets[i].timing = 0;
  }

  this->set_error(dummy_error_handler);
}

/* Pass as a parameter a static void function you previously defined in your code. 
   This will be called when a correct message will be received. 
   Inside there you can code how to react when data is received. 
  
   static void receiver_function(uint8_t sender_id, uint8_t length, uint8_t *payload) {
    Serial.print(sender_id);
    Serial.print(" ");

    for(int i = 0; i < length; i++) 
      Serial.print((char)payload[i]);
      
    Serial.print(" ");
    Serial.println(length);
  };

  network.set_receiver(receiver_function);

*/

void PJON::set_receiver(receiver r) {
  _receiver = r;
}


/* Pass as a parameter a static void function you previously defined in your code.
   This will be called when an error in communication occurs 

static void error_handler(uint8_t code, uint8_t data) {
  Serial.print(code);
  Serial.print(" ");
  Serial.println(data);
};

network.set_error(error_handler); */


void PJON::set_error(error e) {
  _error = e;
}

/* Check if the channel if free for transmission:
 If an entire byte received contains no 1s it means
 that there is no active transmission */

boolean PJON::can_start() {
  pinModeFast(_input_pin, INPUT);
  this->send_bit(0, 2);
  if(!this->read_byte())
    return true;

  return false;
}


/* Transmitter side functions --------------------------------------------------------------------------- */

/* Send a bit to the pin
 digitalWriteFast is used instead of standard digitalWrite
 function to optimize transmission time */

void PJON::send_bit(uint8_t VALUE, int duration) {
  digitalWriteFast(_input_pin, VALUE);
  delayMicroseconds(duration);
}


/* Send a byte to the pin:
  ________ _______________________
 |  Init  |         Byte          |
 |--------|-----------------------|
 |_____   |__       __    __ __   |
 |     |  |  |     |  |  |     |  |
 |1    |0 |1 |0  0 |1 |0 |1  1 |0 |
 |_____|__|__|__ __|__|__|_____|__|

 Init is a long 1 with a BIT_SPACER duration and a standard bit 0
 after that comes before the raw byte */

void PJON::send_byte(uint8_t b) {
  digitalWriteFast(_input_pin, HIGH);
  delayMicroseconds(BIT_SPACER);
  digitalWriteFast(_input_pin, LOW);
  delayMicroseconds(BIT_WIDTH);

  for(uint8_t mask = 0x01; mask; mask <<= 1)
    this->send_bit(b & mask, BIT_WIDTH);
}


/* Send a string to the pin:
 An Example of how the string "HI" is formatted and sent:
  _____    _________________________________________       _____
 | C-A |  | ID | LENGTH | byte0 | byte1  | IV | CRC |     | ACK |
 |-----|->|----|--------|-------|--------|----|-----|-> <-|-----|
 |  0  |  | 12 |   6    |   H   |   I    | 43 | 134 |     |  6  |
 |_____|  |____|________|_______|________|____|_____|     |_____|

 C-A: Collision avoidance - receive a byte, if no 1s channel is free    - 1 byte
 ID: Receiver ID                                                        - 1 byte
 LENGTH: Length of the string (max 255 characters)                      - 1 byte
 IV: Initialization vector, present if encryption activated             - 1 byte
 CRC: Cyclic redundancy check                                           - 1 byte
 ACK: Acknowledge sent from receiver, present if acknowledge activated  - 1 byte */

int PJON::send_string(uint8_t ID, char *string) {

  if (!*string) return FAIL;

  uint8_t package_length = strlen(string) + 3;
  uint8_t CRC = 0;

  if(!this->can_start())
    return BUSY;

  pinModeFast(_input_pin, OUTPUT);
  this->send_byte(ID);
  CRC ^= ID;
  this->send_byte(package_length);
  CRC ^= package_length;

  for(uint8_t i = 0; string[i] != NULL; i++) {
    this->send_byte(string[i]);
    CRC ^= string[i];
  }

  this->send_byte(CRC);
  digitalWriteFast(_input_pin, LOW);

  if(ID == BROADCAST)
    return ACK;
  
  unsigned long time = micros();
  int response = FAIL;

  while(response == FAIL && micros() - time <= BIT_SPACER + BIT_WIDTH)
    response = this->receive_byte();

  if (response == ACK || response == NAK)
    return response;
  
  return FAIL;
};


/* Insert a packet in the send list:
 The added packet will be sent in the next update() call. 
 Using the variable timing is possible to set the delay between every 
 transmission cyclically sending the packet (use remove() function stop it) 
 
 int hi = network.send(99, "HI!", 1000000); // Send hi every second  */ 

int PJON::send(uint8_t ID, char *packet, unsigned long timing) {
  uint8_t package_length = strlen(packet) + 1;
  char *str;
  str = (char *) malloc(package_length);
  memcpy(str, packet, package_length);

  for(uint8_t i = 0; i < MAX_PACKETS; i++)
    if(packets[i].state == NULL) {
      packets[i].content = str;
      packets[i].device_id = ID;
      packets[i].length = package_length;
      packets[i].state = TO_BE_SENT;
      if(timing > 0) {
        packets[i].registration = micros();
        packets[i].timing = timing;
      }
      return i;
    }
  this->_error(PACKETS_BUFFER_FULL, MAX_PACKETS);
  return FAIL;
}


/* Update the state of the send list:
 Check if there are packets to send, erase the correctly delivered */ 

void PJON::update() {
  for(uint8_t i = 0; i < MAX_PACKETS; i++) {
    if(packets[i].state != NULL)
      if(micros() - packets[i].registration > packets[i].timing + pow(packets[i].attempts, 2)) 
        packets[i].state = send_string(packets[i].device_id, packets[i].content); 

    if(packets[i].state == ACK) {
      if(!packets[i].timing)
        this->remove(i);
      else {
        packets[i].attempts = 0;
        packets[i].registration = micros();
        packets[i].state = TO_BE_SENT;
      }
    }

    if(packets[i].state == FAIL) { 
      packets[i].attempts++;
  
      if(packets[i].attempts > MAX_ATTEMPTS) {
        this->_error(CONNECTION_LOST, packets[i].device_id);
        if(!packets[i].timing)
          this->remove(i);
        else {
          packets[i].attempts = 0;
          packets[i].registration = micros();
          packets[i].state = TO_BE_SENT;
        }
      }
    }
  }
}

/* Remove a packet from the send list: */

void PJON::remove(int id) {
  free(packets[id].content);
  packets[id].attempts = 0;
  packets[id].device_id = NULL;
  packets[id].length = NULL;
  packets[id].state = NULL;
  packets[id].registration = NULL;
}


/* Receiver side functions ------------------------------------------------------------------------------- */

/* Syncronize with transmitter:
 This function is used only in byte syncronization.
 READ_DELAY has to be tuned to correctly send and
 receive transmissions because this variable shifts
 in which portion of the bit, reading will be
 executed by the next read_byte function */

uint8_t PJON::syncronization_bit() {
  delayMicroseconds((BIT_WIDTH / 2) - READ_DELAY);
  uint8_t bit_value = digitalReadFast(_input_pin);
  delayMicroseconds(BIT_WIDTH / 2);
  return bit_value;
}


/* Check if a byte is coming from the pin:
 If there is a 1 and is longer then ACCEPTANCE
 and after that comes a 0 probably a byte is coming */

int PJON::receive_byte() {
  pinModeFast(_input_pin, INPUT);
  digitalWriteFast(_input_pin, LOW);

  unsigned long time = micros();
  while (digitalReadFast(_input_pin) && micros() - time <= BIT_SPACER);
  time = micros() - time;

  if(time > ACCEPTANCE && !this->syncronization_bit())
    return (int)this->read_byte();

  return FAIL;
}


/* Read a byte from the pin */

uint8_t PJON::read_byte() {
  uint8_t byte_value = B00000000;
  delayMicroseconds(BIT_WIDTH / 2);
  for (uint8_t i = 0; i < 8; i++) {
    byte_value += digitalReadFast(_input_pin) << i;
    delayMicroseconds(BIT_WIDTH);
  }
  return byte_value;
}


/* Try to receive a string from the pin: */

int PJON::receive() {
  int package_length = PACKET_MAX_LENGTH;
  uint8_t CRC = 0;

  for (uint8_t i = 0; i <= package_length; i++) {
    data[i] = this->receive_byte();

    if (data[i] == FAIL) return FAIL;

    if(i == 0 && data[i] != _device_id && data[i] != BROADCAST)
      return BUSY;

    if(i == 1)
      if(data[i] > 0 && data[i] < PACKET_MAX_LENGTH)
        package_length = data[i];
      else return FAIL;

    CRC ^= data[i];
  }

  if (!CRC) {
    if(data[0] != BROADCAST) {
      pinModeFast(_input_pin, OUTPUT);
      this->send_byte(ACK);
      digitalWriteFast(_input_pin, LOW);
    }
    return ACK;
  } else {
    if(data[0] != BROADCAST) {
      pinModeFast(_input_pin, OUTPUT);
      this->send_byte(NAK);
      digitalWriteFast(_input_pin, LOW);
    }
    return NAK;
  }
}


/* Try to receive a string from the pin repeatedly:
 receive() is executed in cycle with a for because is
 not possible to use micros() as condition (too long to be executed).
 micros() is then used in while as condition approximately every
 10 milliseconds (3706 value in for determines duration) */

int PJON::receive(unsigned long duration) {
  int response;
  long time = micros();
  while(micros() - time <= duration) {
    for(int i = 0; i < 3706; i++) {
      response = this->receive();
      if(response == ACK) {
        this->_receiver(data[1] - 3, data + 2); 
        return ACK;
      }
    }
  }
  return response;
}