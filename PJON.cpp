
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \|
   |y"s§+`\     Giovanni Blu Mitolo 2012 - 2014
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
   |/:%&-a3f.:/\     Simple library to send data from an Arduino board
    \+//u/+gosv//\    to another one up to 4Kb/s with no trouble and
     \o+&/osw+odss\\   on a single wire (plus GND obviously :P)
       \:/+-.§°-:+oss\
        | |       \oy\\
        > <               Pull down resistor on the bus is generally used to reduce interference
       -| |-

ADDRESS: A byte is dedicated to the id of the board, so 255 different ids are selectable
ACKNOLEDGE: Feedback to communication (receiver answer to sender ACK or NACK)
COLLISION DETECTION: Transmissions collision avoidance
CRC: XOR Cyclic Redundancy Check to ensure correct data communication
ENCRYPTION: Custom private key encryption algorithm

PJON Slow mode
Absolute bandwidth 3.0 kb/s | Practical bandwidth 2.38 kb/s | Accuracy: 99.25%
bit_width 20 - bit_spacer 68 - acceptance 16 - read_delay 9

PJON Medium mode
Absolute bandwidth 3.51 kb/b | Practical bandwidth 2.75 kb/s | Accuracy: 96.6%
bit_width 18 - bit_spacer 36 - acceptance 16 - read_delay 7 */

#include "WProgram.h"
#include "WConstants.h"
#include "includes/digitalWriteFast.h"
#include "PJON.h"

/* Initiate PJON passing pin number and the selected device id */
PJON::PJON(int input_pin, byte device_id) {
  _input_pin = input_pin;
  _device_id = device_id;
}

/* Avoid to transmit over other transmitting devices:
 Every device to speak together without transmissions collision.
 On top of this can be developed a multimaster network of Arduino boards
 that arbitrally decides to communicate (slight reduction of bandwidth) */
void PJON::set_collision_avoidance(boolean state) {
  _collision_avoidance = state;
}

/* Sender waits for a feedback by receiver after transmission:
 After the string is sent, receiver answer with an ACK if
 CRC is ok or NAK if wrong (slight reduction of bandwidth) */
void PJON::set_acknowledge(boolean state) {
  _acknowledge = state;
}

/* Encrypt transmitted data */
void PJON::set_encryption(boolean state) {
  _encryption = state;
}

/* Encrypt string with a custom made private key algorithm */
void PJON::crypt(char *data) {
 int i, j = 0;

 for (i = 0; i < encryption_strength; i++)
  _s_box[i] = i;

 for (i = 0; i < encryption_strength; i++) {
  j = (j + _s_box[i] + encryption_key[ i % strlen(encryption_key)]) % encryption_strength;
  swap(_s_box[i], _s_box[j]);
 }

 i = j = 0;
 for (int k = 0; k < strlen(data); k++) {
  i = (i + 1) % encryption_strength;
  j = (j + _s_box[i]) % encryption_strength;
  swap(_s_box[i], _s_box[j]);
  hash[k] = (data[k] ^ _s_box[ (_s_box[i] + _s_box[j]) % encryption_strength]) ^ encryption_key[3];
  hash[k] = hash[k] ^ ((encryption_key[0] + i + encryption_key[1] + k + encryption_key[2]) % 255);
 }

 hash[strlen(data) + 1] = '\0';
}

/* Send a bit to the pin */
void PJON::send_bit(byte VALUE, int duration) {
  digitalWriteFast(_input_pin, VALUE);
  delayMicroseconds(duration);
}

/* Send a byte to the pin */
void PJON::send_byte(byte b) {
  pinModeFast(_input_pin, OUTPUT);
  this->send_bit(HIGH, bit_spacer);
  this->send_bit(LOW, bit_width);

  for(byte mask = 0x01; mask; mask <<= 1)
    this->send_bit(b & mask, bit_width);
}

/* Send a string to the pin */
int PJON::send_string(byte ID, char *string) {

  int package_length = strlen(string) + 3;
  byte CRC = 0;

  if(_collision_avoidance && !this->can_start())
      return BUSY;

  if(_encryption)
    this->crypt(string);

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
    response = this->start();

  if(response == NAK) return NAK;
  if(response == ACK) return ACK;
  return FAIL;

};

/* Send a command to the pin */
int PJON::send_command(byte ID, byte command_type, unsigned int value) {
  char bytes_to_send[3] = { command_type, value >> 8, value & 0xFF };
  return this->send_string(ID, bytes_to_send);
}

/* Receive a bit from the pin */
int PJON::receive_bit() {
  delayMicroseconds((bit_width / 2) - read_delay);
  int bit_value = digitalReadFast(_input_pin);
  delayMicroseconds(bit_width / 2);
  return bit_value;
}

/* Receive a byte from the pin */
byte PJON::receive_byte() {
  byte byte_value = B00000000;
  delayMicroseconds(bit_width / 2);
  for (int i = 0; i < 8; i++) {
    byte_value += digitalReadFast(_input_pin) << i;
    delayMicroseconds(bit_width);
  }

  return byte_value;
}

/* Check if the channel if free for transmission */
boolean PJON::can_start() {
  pinModeFast(_input_pin, INPUT);
  this->send_bit(0, 8);
  if(!this->receive_byte())
    return true;

  return false;
}

/* Check if a byte is coming from the pin */
int PJON::start() {
  pinModeFast(_input_pin, INPUT);
  digitalWriteFast(_input_pin, LOW);

  unsigned long time = micros();
  while (digitalReadFast(_input_pin) && micros() - time <= bit_spacer);
  time = micros() - time;

  if(time > acceptance && !this->receive_bit())
    return (int)this->receive_byte();

  return FAIL;
}

/* Receive a string from the pin */
int PJON::receive() {
  int package_length = max_package_length;
  byte CRC = 0;

  for (int i = 0; i < package_length; i++) {
    data[i] = this->start();

    if (data[i] == FAIL)
      return FAIL;

    if(i == 0 && data[i] != _device_id)
      return BUSY;

    if(i == 1)
      package_length = data[i];

    if (i < package_length - 1)
      CRC ^= data[i];
  }

  if (data[package_length - 1] == CRC) {
    if(_acknowledge) {
      this->send_byte(ACK);
      digitalWriteFast(_input_pin, LOW);
    }
    if(_encryption) {
      for(int i = 0; i < package_length - 1; i++)
        if(i > 1) data[i - 2] = data[i];

      this->crypt((char*)data);
    }
    return ACK;

  } else {
    if(_acknowledge) {
      this->send_byte(NAK);
      digitalWriteFast(_input_pin, LOW);
    }
    return NAK;
  }
}

