
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \| v2.0
   |y"s§+`\     Giovanni Blu Mitolo 2012 - 2016
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\     PJON is a multimaster device communications bus system able to connect
  \+//u/+g%{osv,,\    255 arduino boards over one wire up to 5.95kB/s data communication speed.
    \=+&/osw+olds.\\   Contains acknowledge, collision and error detection, all done with
       \:/+-.-°-:+oss\  micros() and delayMicroseconds(), with no use of interrupts or timers.
        | |       \oy\\  Pull down resistor is generally used to reduce interference.
        > <
  _____-| |-________________________________________________________________________

  Released with BSD-3 License.
  PJON Copyright (c) 2012-2016, Giovanni Blu Mitolo All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.

  -  Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

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

#ifndef PJON_h
  #define PJON_h
  #include "Arduino.h"
  #include "includes/digitalWriteFast.h"

  /* STANDARD mode performance:
     Transfer speed: 16.944kBb or 2.12kB/s
     Absolute  communication speed: 1.81kB/s (data length 20 of characters)
     Data throughput: 1.51kB/s (data length 20 of characters) */
  #define STANDARD  0

  /* FAST mode performance:
     Transfer speed: 25.157kBd or 3.15kB/s
     Absolute  communication speed: 2.55kB/s (data length 20 of characters)
     Data throughput: 2.13kB/s (data length 20 of characters) */
  #define FAST      1

  /* OVERDRIVE mode performance:
     Architecture / setup dependant, see Timing.h */
  #define OVERDRIVE 2

  #define MODE STANDARD

  /* Include BIT banging timing (mode / architecture / clock specific) */
  #include "Timing.h"

  /* Protocol symbols */
  #define ACK           6
  #define ACQUIRE_ID    63
  #define BROADCAST     0
  #define BUSY          666
  #define NAK           21
  #define NOT_ASSIGNED  255

  /* Internal constants */
  #define FAIL          0x100
  #define TO_BE_SENT    74

  /* Errors */
  #define CONNECTION_LOST     101
  #define PACKETS_BUFFER_FULL 102
  #define MEMORY_FULL         103
  #define CONTENT_TOO_LONG    104
  #define ID_ACQUISITION_FAIL 105

  /* Constraints:
  Max attempts before throwing CONNECTON_LOST error */
  #define MAX_ATTEMPTS        125
  /* Packets buffer length, if full PACKETS_BUFFER_FULL error is thrown */
  #define MAX_PACKETS         10
  /* Max packet length, higher if necessary (and you have free memory) */
  #define PACKET_MAX_LENGTH   50
  /* Maximum random delay on startup */
  #define INITIAL_MAX_DELAY   255000
  /* Maximum randon delay on collision */
  #define COLLISION_MAX_DELAY 16
  /* Maximum id scan time (5 seconds) */
  #define MAX_ID_SCAN_TIME    5000000

  struct packet {
    uint8_t  attempts;
    uint8_t  device_id;
    char     *content;
    uint8_t  length;
    uint32_t registration;
    uint16_t state;
    uint32_t timing;
  };

  typedef void (* receiver)(uint8_t length, uint8_t *payload);
  typedef void (* error)(uint8_t code, uint8_t data);
  static void dummy_error_handler(uint8_t code, uint8_t data) {};
  static void dummy_receiver_handler(uint8_t length, uint8_t *payload) {};

  class PJON {
    public:
      PJON(uint8_t input_pin);
      PJON(uint8_t input_pin, uint8_t id);
      void initialize();

      uint8_t device_id();
      void acquire_id();

      void set_id(uint8_t id);
      void set_receiver(receiver r);
      void set_error(error e);

      uint8_t  read_byte();
      uint16_t receive_byte();
      uint16_t receive();
      uint16_t receive(uint32_t duration);

      void     send_byte(uint8_t b);
      uint16_t send_string(uint8_t id, char *string, uint8_t length);
      uint16_t send(uint8_t id, char *packet, uint8_t length, uint32_t timing = 0);

      void update();
      void remove(uint16_t id);

      boolean can_start();
      uint8_t syncronization_bit();

      uint8_t data[PACKET_MAX_LENGTH];
      packet  packets[MAX_PACKETS];

    private:
      uint8_t   _device_id;
      uint8_t   _input_pin;
      receiver  _receiver;
      error     _error;
  };

#endif
