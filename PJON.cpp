
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \| v1.2
   |y"s§+`\     Giovanni Blu Mitolo 2012 - 2016
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\     PJON is a device communications bus system that connects up to 255
  \+//u/+g%{osv,,\    arduino boards over one wire up to 5.29kB/s data communication speed.
    \=+&/osw+olds.\\   Contains acknowledge, collision and error detection, all done with
       \:/+-.-°-:+oss\  micros() and delayMicroseconds(), with no use of interrupts or timers.
        | |       \oy\\  Pull down resistor on the bus is generally used to reduce interference.
        > <
       -| |-

Copyright (c) 2012-2016, Giovanni Blu Mitolo All rights reserved.

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

/* modified 2016-03-09 by Esben Soeltoft

   - Added support for the Arduino Zero. 
     Able to achieve the following speeds between two Arduino Zero
       Absolute com speed: 5215.20B/s
       Practical bandwidth: 4346.00B/s
       Packets sent: 2173.00
       Mistakes (error found with CRC) 0.00
       Fail (no answer from receiver) 0
       Busy (Channel is busy or affected by interference) 0
       Accuracy: 100.00 %  
   
   - All comparison typecasted to preserve proper format between platforms. Comparisons
     on AVR defaults to 16 bit unless typecasted, while it defaults to 32 bits on SAMD
     This can give problems when comparison is made with timer functions such as millis(),
     micros(), delay() and delaymicroseconds().
     The function micros() and millis() both returns a unsigned long (uint32_t) on AVR and SAMD.
     Delay argument is typecasted (unsigned long) or (uint32_t) on AVR, and delaymicroseconds argument 
     is typecasted (unsigned int) or (uint16_t) on AVR, while on SAMD the delay argument is typecasted 
     (uint32_t), and delaymicroseconds argument is typecasted (uint32_t).

   - #endif belonging to #ifndef PJON_h moved to end of file

   - in function receive_byte, the functioncall digitalWriteFast(_input_pin,LOW) has been removed. It
     should not be necessary when the _input_pin is pulled low using a pulldown resistor. Having this
     second line only slows down.
     If the physical pulldown resistor has to be avoided, pinModeFast(_input_pin,INPUT_PULLDOWN) will
     be the correct function call to use, as the INPUT_PULLDOWN is done through pinMode and not
     digitalWriteFast.
     
   TO-DO
   - Reduce variable size to optimize memory footprint

*/




#include "PJON.h"

/* Initiate PJON passing pin number:
   Device's id has to be set through set_id()
   before transmitting on the PJON network.  */

PJON::PJON(uint8_t input_pin) {
  _input_pin = input_pin;
  this->initialize();
}


/* Initiate PJON passing pin number and the device's id: */

PJON::PJON(uint8_t input_pin, uint8_t device_id) {
  _input_pin = input_pin;
  _device_id = device_id;
  this->initialize();
}


/* Initialization tasks: */

void PJON::initialize() {
  /* Initial random delay to avoid startup collision */
  delayMicroseconds(random(0, INITIAL_MAX_DELAY));

  this->set_error(dummy_error_handler);
  this->set_receiver(dummy_receiver_handler);

  for(int i = 0; i < MAX_PACKETS; i++) {
    packets[i].state = NULL;
    packets[i].timing = 0;
    packets[i].attempts = 0;
  }
}


/* Set the device id, passing a single byte (watch out to id collision) */

void PJON::set_id(uint8_t id) {
  _device_id = id;
}


/* Get the device id, returning a single byte (watch out to id collision) */

uint8_t PJON::get_id() {
  return _device_id;
}


/* Pass as a parameter a void function you previously defined in your code.
   This will be called when a correct message will be received.
   Inside there you can code how to react when data is received.

   void receiver_function(uint8_t length, uint8_t *payload) {
    for(int i = 0; i < length; i++)
      Serial.print((char)payload[i]);

    Serial.print(" ");
    Serial.println(length);
  };

  network.set_receiver(receiver_function); */

void PJON::set_receiver(receiver r) {
  _receiver = r;
}


/* Pass as a parameter a void function you previously defined in your code.
   This will be called when an error in communication occurs

void error_handler(uint8_t code, uint8_t data) {
  Serial.print(code);
  Serial.print(" ");
  Serial.println(data);
};

network.set_error(error_handler); */

void PJON::set_error(error e) {
  _error = e;
}


/* Check if the channel is free for transmission:
 If receiving 10 bits no 1s are detected
 there is no active transmission */

boolean PJON::can_start() {
  pinModeFast(_input_pin, INPUT);

  for(uint8_t i = 0; i < 9; i++) {
    if(digitalReadFast(_input_pin))
      return false;
    delayMicroseconds(BIT_WIDTH);
  }

  if(digitalReadFast(_input_pin))
    return false;

  return true;
}


/* Send a bit to the pin:
 digitalWriteFast is used instead of standard digitalWrite
 function to optimize transmission time */

void PJON::send_bit(uint8_t VALUE, uint32_t duration) {
  digitalWriteFast(_input_pin, VALUE);
  delayMicroseconds(duration);
}


/* Every byte is prepended with 2 synchronization padding bits. The first
   is a longer than standard logic 1 followed by a standard logic 0.
   __________ ___________________________
  | SyncPad  | Byte                      |
  |______    |___       ___     _____    |
  | |    |   |   |     |   |   |     |   |
  | | 1  | 0 | 1 | 0 0 | 1 | 0 | 1 1 | 0 |
  |_|____|___|___|_____|___|___|_____|___|
    |
   ACCEPTANCE

The reception tecnique is based on finding a logic 1 as long as the
first padding bit within a certain threshold, synchronizing to its
falling edge and checking if it is followed by a logic 0. If this
pattern is recognised, reception starts, if not, interference,
synchronization loss or simply absence of communication is
detected at byte level. */

void PJON::send_byte(uint8_t b) {
  digitalWriteFast(_input_pin, HIGH);
  delayMicroseconds(BIT_SPACER);
  digitalWriteFast(_input_pin, LOW);
  delayMicroseconds(BIT_WIDTH);

  for(uint8_t mask = 0x01; mask; mask <<= 1) {
    digitalWriteFast(_input_pin, b & mask);
    delayMicroseconds(BIT_WIDTH);
  }
}


/* An Example of how the string "@" is formatted and sent:

 ID 12            LENGTH 4         CONTENT 64       CRC 130
 ________________ ________________ ________________ __________________
|Sync | Byte     |Sync | Byte     |Sync | Byte     |Sync | Byte       |
|___  |     __   |___  |      _   |___  |  _       |___  |  _      _  |
|   | |    |  |  |   | |     | |  |   | | | |      |   | | | |    | | |
| 1 |0|0000|11|00| 1 |0|00000|1|00| 1 |0|0|1|000000| 1 |0|0|1|0000|1|0|
|___|_|____|__|__|___|_|_____|_|__|___|_|_|_|______|___|_|_|_|____|_|_|

A standard packet transmission is a bidirectional communication between
two devices that can be divided in 3 different phases:

Channel analysis   Transmission                            Response
    _____           _____________________________           _____
   | C-A |         | ID | LENGTH | CONTENT | CRC |         | ACK |
<--|-----|---------|----|--------|---------|-----|--> <----|-----|
   |  0  |         | 12 |   4    |   64    | 130 |         |  6  |
   |_____|         |____|________|_________|_____|         |_____|  */

uint16_t PJON::send_string(uint8_t id, char *string, uint8_t length) {
  if(!*string) return FAIL;

  if(!this->can_start()) return BUSY;

  uint8_t CRC = 0;
  pinModeFast(_input_pin, OUTPUT);

  this->send_byte(id);
  CRC ^= id;
  this->send_byte(length + 3);
  CRC ^= length + 3;

  for(uint8_t i = 0; i < length; i++) {
    this->send_byte(string[i]);
    CRC ^= string[i];
  }

  this->send_byte(CRC);
  digitalWriteFast(_input_pin, LOW);

  if(id == BROADCAST) return ACK;

  uint32_t time = micros();
  uint16_t response = FAIL;

  /* Receive byte for an initial BIT_SPACER bit + standard bit total duration.
     (freak condition used to avoid micros() overflow bug) */
  while(response == FAIL && !((uint32_t)(micros() - time) >= BIT_SPACER + BIT_WIDTH))
    response = this->receive_byte();

  if(response == ACK) return response;

  /* Random delay if NAK, corrupted ACK/NAK or collision */
  if(response != FAIL)
    delayMicroseconds(random(0, COLLISION_MAX_DELAY));

  if(response == NAK) return response;

  return FAIL;
};


/* Insert a packet in the send list:
 The added packet will be sent in the next update() call.
 Using the timing parameter you can set the delay between every
 transmission cyclically sending the packet (use remove() function stop it)

 int hi = network.send(99, "HI!", 3, 1000000); // Send hi every second
   _________________________________________________________________________
  |           |        |         |       |          |        |              |
  | device_id | length | content | state | attempts | timing | registration |
  |___________|________|_________|_______|__________|________|______________| */

uint16_t PJON::send(uint8_t id, char *packet, uint8_t length, uint32_t timing) {

  if(length >= PACKET_MAX_LENGTH) {
    this->_error(CONTENT_TOO_LONG, length);
    return FAIL;
  }

  char *str = (char *) malloc(length);

  if(str == NULL) {
    this->_error(MEMORY_FULL, FAIL);
    return FAIL;
  }

  memcpy(str, packet, length);

  for(uint8_t i = 0; i < MAX_PACKETS; i++)
    if(packets[i].state == NULL) {
      packets[i].content = str;
      packets[i].device_id = id;
      packets[i].length = length;
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
   check if there are packets to be sent or to be erased
   if correctly delivered */

void PJON::update() {
  for(uint8_t i = 0; i < MAX_PACKETS; i++) {
    if(packets[i].state != NULL)
      if(micros() - packets[i].registration > packets[i].timing + pow(packets[i].attempts, 2))
        packets[i].state = send_string(packets[i].device_id, packets[i].content, packets[i].length);

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

void PJON::remove(uint16_t id) {
  free(packets[id].content);
  packets[id].attempts = 0;
  packets[id].device_id = NULL;
  packets[id].length = NULL;
  packets[id].state = NULL;
  packets[id].registration = NULL;
}


/* Syncronize with transmitter:
 This function is used only in byte syncronization.
 READ_DELAY has to be tuned to correctly send and
 receive transmissions because this variable shifts
 in which portion of the bit, the reading will be
 executed by the next read_byte function */

uint8_t PJON::syncronization_bit() {
  delayMicroseconds((BIT_WIDTH / 2) - READ_DELAY);
  uint8_t bit_value = digitalReadFast(_input_pin);
  delayMicroseconds(BIT_WIDTH / 2);
  return bit_value;
}


/* Check if a byte is coming from the pin:
 This function is looking for padding bits before a byte.
 If value is 1 for more than ACCEPTANCE and after
 that comes a 0 probably a byte is coming:
  ________
 |  Init  |
 |--------|
 |_____   |
 |  |  |  |
 |1 |  |0 |
 |__|__|__|
    |
  ACCEPTANCE */

uint16_t PJON::receive_byte() {
  /* Initialize the pin and set it to LOW to reduce interference */
  pinModeFast(_input_pin, INPUT);
  uint32_t time = micros();
  /* Do nothing until the pin stops to be HIGH or passed more time than
     BIT_SPACER duration (freak condition used to avoid micros() overflow bug) */
  while(digitalReadFast(_input_pin) && !( (uint32_t)( micros() - time >= BIT_SPACER ) ));
  /* Save how much time passed */
  time = micros() - time;
  /* is for sure less than BIT_SPACER, and if is more than ACCEPTANCE
     (a minimum HIGH duration) and what is coming after is a LOW bit
     probably a byte is coming so try to receive it. */
  if(time >= ACCEPTANCE && !this->syncronization_bit())
    return (uint8_t)this->read_byte();

  return FAIL;
}


/* Read a byte from the pin */

uint8_t PJON::read_byte() {
  uint8_t byte_value = B00000000;
  delayMicroseconds(BIT_WIDTH / 2);
  for(uint8_t i = 0; i < 8; i++) {
    byte_value += digitalReadFast(_input_pin) << i;
    delayMicroseconds(BIT_WIDTH);
  }
  return byte_value;
}


/* Try to receive a packet from the pin: */

uint16_t PJON::receive() {
  uint16_t state;
  uint16_t package_length = PACKET_MAX_LENGTH;
  uint8_t CRC = 0;

  for(uint8_t i = 0; i < package_length; i++) {
    data[i] = state = this->receive_byte();

    if(state == FAIL) return FAIL;

    if(i == 0 && data[i] != _device_id && data[i] != BROADCAST)
      return BUSY;

    if(i == 1)
      if(data[i] > 3 && data[i] < PACKET_MAX_LENGTH)
        package_length = data[i];
      else return FAIL;

    CRC ^= data[i];
  }

  pinModeFast(_input_pin, OUTPUT);

  if(!CRC) {
    if(data[0] != BROADCAST) {
      this->send_byte(ACK);
      digitalWriteFast(_input_pin, LOW);
    }
    this->_receiver(data[1] - 3, data + 2);
    return ACK;
  } else {
    if(data[0] != BROADCAST) {
      this->send_byte(NAK);
      digitalWriteFast(_input_pin, LOW);
    }
    return NAK;
  }
}


/* Try to receive a packet from the pin repeatedly with a maximum duration: */

uint16_t PJON::receive(uint32_t duration) {
  uint16_t response;
  uint32_t time = micros();
  /* (freak condition used to avoid micros() overflow bug) */
  while( !(uint32_t)( micros() - time >= duration ) ) {
    response = this->receive();
    if(response == ACK)
      return ACK;
  }
  return response;
}
