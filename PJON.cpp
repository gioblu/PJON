
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \|
   |y"s§+`\     Giovanni Blu Mitolo 2012 - 2014
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
   |/:%&-a3f.:/\     PJON is a device communications bus system that connects up to 255
    \+//u/+gosv//\    arduino boards over a single wire and provides up to 5KB/s data communication
     \o+&/osw+odss\\   with acknowledge, collision detection, CRC and encpryption all done
       \:/+-.§°-:+oss\  with micros() and delayMicroseconds(), without using interrupt or timers.
        | |       \oy\\   Pull down resistor on the bus is generally used to reduce interference.
        > <
       -| |-

ADDRESS: 255 different adresses can be assigned
CRC: XOR Cyclic Redundancy Check ensures almost errorless data communication
ACKNOLEDGE:  Acknowledge byte sent by receiver ensures packet delivery
COLLISION DETECTION: collision avoidance is ensured analyzing network bus before starting
ENCRYPTION: Private key encryption + initialization vector to ensure almost random data stream */

#include "PJON.h"

/* Initiate PJON passing pin number and the selected device_id */

PJON::PJON(int input_pin, uint8_t device_id) {
  _input_pin = input_pin;
  _device_id = device_id;

  for(int i = 0; i < max_reactions; i++)
    reactions[i].empty = true;

  for(int i = 0; i < max_packets; i++) {
    packets[i].state = NULL;
    packets[i].timing = 0;
  }
}


/* Set collision detection:
 If true Avoids to transmit over other transmitting devices:
 on top of this can be developed a multimaster network of Arduino boards
 that arbitrally decides to communicate (slight reduction of bandwidth) */

void PJON::set_collision_avoidance(boolean state) {
  _collision_avoidance = state;
}


/* Set acknowledge state:
 If true sender waits for feedback by receiver after transmission:
 After the string is sent, receiver answers with an ACK if
 CRC is ok or NAK if wrong (slight reduction of bandwidth)
 If false, if you call send_string repeatedly, at least 14 or more
 microseconds delay is necessary bewteen every send_string. */

void PJON::set_acknowledge(boolean state) {
  _acknowledge = state;
}


/* Set Encryption state to transmitted data */

void PJON::set_encryption(boolean state) {
  _encryption = state;
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


/* Encrypt string with a custom made private key algorithm + initialization vector ----------------------- */


/* RC4 cpp implementation with a 1 byte initialization vector */

void PJON::crypt(char *content, boolean initialization_vector, boolean side) {
  uint8_t i, j = 0;
  uint8_t string_length = strlen(content);
  uint8_t encryption_key_length = strlen(encryption_key);

  if(initialization_vector && side)
    for(i = 0; i < string_length; i++)
      content[i] ^= content[string_length - 1];

  for (i = 0; i < encryption_strength; i++)
    _s_box[i] = i;
  
  for (i = 0; i < encryption_strength; i++) {
    j = (j + _s_box[i] + encryption_key[i % encryption_key_length]) % encryption_strength;
    swap(_s_box[i], _s_box[j]);
  }

  i = j = 0;
  for (int k = 0; k < string_length; k++) {
    i = (i + 1) % encryption_strength;
    j = (j + _s_box[i]) % encryption_strength;
    swap(_s_box[i], _s_box[j]);
    hash[k] = content[k] ^ _s_box[ (_s_box[i] + _s_box[j]) % encryption_strength];
  }

  if(initialization_vector && !side) {
    hash[string_length] = this->generate_IV(string_length);
    for(i = 0; i < string_length; i++)
      hash[i] ^= hash[string_length];
  }

  hash[string_length + 1] = '\0';
}

uint8_t PJON::generate_IV(uint8_t string_length) {
  uint8_t IV = (micros() % 254) + 1;
  for(uint8_t i = 0; i < string_length; i++)
    if(hash[i] == IV)
      return this->generate_IV(string_length);
  return IV;
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

 Init is a long 1 with a bit_spacer duration and a standard bit 0
 after that comes before the raw byte */

void PJON::send_byte(uint8_t b) {
  digitalWriteFast(_input_pin, HIGH);
  delayMicroseconds(bit_spacer);
  digitalWriteFast(_input_pin, LOW);
  delayMicroseconds(bit_width);

  for(uint8_t mask = 0x01; mask; mask <<= 1)
    this->send_bit(b & mask, bit_width);
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

  uint8_t package_length = strlen(string) + 4;
  uint8_t CRC = 0;

  if(_collision_avoidance && !this->can_start())
      return BUSY;

  if(_encryption)
    this->crypt(string, true, 0);

  pinModeFast(_input_pin, OUTPUT);
  this->send_byte(ID);
  CRC ^= ID;
  this->send_byte(package_length);
  CRC ^= package_length;

  char *string_pointer = (_encryption) ? hash : string;
  while(*string_pointer) {
    this->send_byte(*string_pointer);
    CRC ^= *string_pointer;
    *string_pointer++;
  }

  this->send_byte(CRC);
  digitalWriteFast(_input_pin, LOW);

  if(!_acknowledge) return ACK;
  unsigned long time = micros();
  int response = FAIL;

  while(response == FAIL && micros() - time <= bit_spacer + bit_width)
    response = this->receive_byte();

  if(response == NAK) return NAK;
  if(response == ACK) return ACK;
  return FAIL;

};

int PJON::send_string(uint8_t ID, char *string, int count) {
  int response;
  for(int i = 0; i < count && response != ACK; i++)
    response = this->send_string(ID, string);

  return response;
}


/* Send a command to the pin:
 An Example of how a command is formatted and sent:
  _____    __________________________________________________________       _____
 | C-A |  |  ID   | LENGTH |  CMD  |  type  |  value |  IV   |  CRC  |     | ACK |
 |-----|->|-------|--------|-------|--------|--------|-------|-------|-> <-|-----|
 |  0  |  | 0-255 |   7    |   88  |  0-255 |  0-255 | 0-255 | 0-255 |     |  6  |
 |_____|  |_______|________|_______|________|________|_______|_______|     |_____|

 C-A: Collision avoidance - receive a byte, if no 1s channel is free           - 1 byte
 ID: Receiver ID                                                               - 1 byte
 LENGTH: Length of the string (max 255 characters)                             - 1 byte

 CMD: 88 is the command symbol                                                 - 1 byte
 type: 0-255 byte value that has to be equal to its related reaction's id      - 1 byte
 value: 1 or 2 bytes dedicated to command additional data                      - 1 byte

 IV: Initialization vector, present if encryption activated                    - 1 byte
 CRC: Cyclic redundancy check                                                  - 1 byte
 ACK: Acknowledge sent from receiver, present if acknowledge activated         - 1 byte */

int PJON::send_command(byte ID, byte command_type, uint8_t value, unsigned long timing) {
  char bytes_to_send[3] = { CMD, command_type, value };
  return this->send(ID, bytes_to_send, timing);
}

int PJON::send_command(byte ID, byte command_type, unsigned int value, unsigned long timing) {
  char bytes_to_send[4] = { CMD, command_type, value >> 8, value & 0xFF };
  return this->send(ID, bytes_to_send, timing);
}


/* Insert a packet in the send list:
 The added packet will be sent in the next update() call. 
 Using the variable timing is possible to set the delay between every 
 transmission cyclically sending the packet (use remove() function stop it) */ 

int PJON::send(uint8_t ID, char *packet, unsigned long timing) {
  for(uint8_t i = 0; i < max_packets; i++)
    if(packets[i].state == NULL) {
      packets[i].state = TO_BE_SENT;
      packets[i].content = packet;
      packets[i].device_id = ID;
      if(timing > 0) {
        packets[i].registration = micros();
        packets[i].timing = timing;
      }
      return i;
    }

  return -1;
}


/* Update the state of the send list:
 Check if there are packets to send, erase the correctly delivered */ 

void PJON::update() {
  for(uint8_t i = 0; i < max_packets; i++) {

    if(packets[i].state != NULL)
      if(!packets[i].timing || packets[i].timing && micros() - packets[i].registration > packets[i].timing) 
        packets[i].state = send_string(packets[i].device_id, packets[i].content); 
    
    if(packets[i].state == ACK) {
      if(packets[i].timing == 0)
        this->remove(i);
      else {
        packets[i].state = TO_BE_SENT;
        packets[i].registration = micros();
      }
    }
  }
}


/* Remove a packet from the send list: */

void PJON::remove(int packet_id) {
  packets[packet_id].content = NULL;
  packets[packet_id].state = NULL;
  packets[packet_id].device_id = NULL;
  packets[packet_id].registration = NULL;
}

/* Receiver side functions ------------------------------------------------------------------------------- */

/* Syncronize with transmitter:
 This function is used only in byte syncronization.
 read_delay has to be tuned to correctly send and
 receive transmissions because this variable shifts
 in which portion of the bit, reading will be
 executed by the next read_byte function */

uint8_t PJON::syncronization_bit() {
  delayMicroseconds((bit_width / 2) - read_delay);
  uint8_t bit_value = digitalReadFast(_input_pin);
  delayMicroseconds(bit_width / 2);
  return bit_value;
}


/* Check if a byte is coming from the pin:
 If there is a 1 and is longer then acceptance
 and after that comes a 0 probably a byte is coming */

int PJON::receive_byte() {
  pinModeFast(_input_pin, INPUT);
  digitalWriteFast(_input_pin, LOW);

  unsigned long time = micros();
  while (digitalReadFast(_input_pin) && micros() - time <= bit_spacer);
  time = micros() - time;

  if(time > acceptance && !this->syncronization_bit())
    return (int)this->read_byte();

  return FAIL;
}


/* Read a byte from the pin */

uint8_t PJON::read_byte() {
  uint8_t byte_value = B00000000;
  delayMicroseconds(bit_width / 2);
  for (uint8_t i = 0; i < 8; i++) {
    byte_value += digitalReadFast(_input_pin) << i;
    delayMicroseconds(bit_width);
  }
  return byte_value;
}


/* Try to receive a string from the pin: */

int PJON::receive() {
  int package_length = max_package_length;
  uint8_t CRC = 0;

  for (uint8_t i = 0; i < package_length; i++) {
    data[i] = this->receive_byte();

    if (data[i] == FAIL)
      return FAIL;

    if(i == 0 && data[i] != _device_id && data[i] != BROADCAST)
      return BUSY;

    if(i == 1)
      package_length = data[i];

    CRC ^= data[i];
  }

  if (!CRC) {
    if(_acknowledge && data[0] != BROADCAST) {
      pinModeFast(_input_pin, OUTPUT);
      this->send_byte(ACK);
      digitalWriteFast(_input_pin, LOW);
    }
    if(_encryption) {
      for(int i = 0; i < package_length - 1; i++)
        if(i > 1) data[i - 2] = data[i];

      this->crypt((char*)data, true, 1);
    }
    return ACK;

  } else {
    if(_acknowledge && data[0] != BROADCAST) {
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
  unsigned long time = micros();
  while(micros() - time <= duration) {
    for(unsigned long i = 0; i < 3706; i++) {
      response = this->receive();
      if(response == ACK) { 
        if(data[2] == CMD)
          this->process_reaction();
        break;
      }
    }
  }
  return response;
}

/* Reaction to command functions ------------------------------------------------------------------------- */


/* Insert a reaction to a command in the reaction's list: 
 Returns the ID of the reaction you added, this could be 
 useful in future to remove or deactivate it */

uint8_t PJON::insert_reaction(char command_type, void (*task)(void), boolean once) {
  for(uint8_t i = 0; i < max_reactions; i++)
    if(reactions[i].empty) {
      reactions[i].active = true;
      reactions[i].execution = *task;
      reactions[i].command_type = command_type;
      reactions[i].once = once;
      reactions[i].empty = false;
      return i;
    }
  return -1;
}


/* Process reaction to a command:
 Find in the list of reactions if there 
 is a reaction's command that has the same
 content of the one you have now received, if
 a candidate is found, execute the related function */

void PJON::process_reaction() {
  for(uint8_t i = 0; i < max_reactions; i++)
    if(this->compare_reaction(reactions[i].command_type))
      reactions[i].execution();
}

/* Compare if the last received command content
 matches one of the reaction's command */ 

boolean PJON::compare_reaction(char command_type) {

  if(_encryption)
    if(hash[1] != command_type)
      return false;
  else
    if(data[4] != command_type)
      return false;

  return true;
}


/* Activate a reaction you previously added: */

void PJON::activate_reaction(uint8_t id) {
  reactions[id].active = true;
}


/* Deactivate a reaction you previously added: */

void PJON::deactivate_reaction(uint8_t id) {
  reactions[id].active = false;
}


/* Remove from the reactions list a reaction you previously added: 
 Pass to the function the ID of the reaction you want to remove
 from the list */

void PJON::remove_reaction(uint8_t id) {
  reactions[id].active = NULL;
  reactions[id].execution = NULL;
  reactions[id].once = NULL;
  reactions[id].empty = true;
}

