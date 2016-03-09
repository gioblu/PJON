
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \| v1.0
   |y"s§+`\     Giovanni Blu Mitolo 2012 - 2016
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\     PJON is a device communications bus system that connects up to 255
  \+//u/+g%{osv,,\    arduino boards over one wire up to 5.29kB/s data communication speed.
    \=+&/osw+olds.\\   Contains acknowledge, collision detection, CRC and encpryption all done
       \:/+-.-°-:+oss\  with micros() and delayMicroseconds(), with no use of interrupts or timers.
        | |       \oy\\  Pull down resistor on the bus is generally used to reduce interference.
        > <
  _____-| |-________________________________________________________________________
 | BIT_WIDTH 20 | BIT_SPACER 56 | ACCEPTANCE  20 | READ_DELAY 8                     |
 |----------------------------------------------------------------------------------|
 |Transfer speed: 5.29kB/s     | Absolute bandwidth:  3.23 kB/s                     |
 |Baud rate:      42372 baud   | Data throughput:     2.68 kB/s                     |
 |__________________________________________________________________________________|

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


#ifndef PJON_h
  #define PJON_h

  #include "Arduino.h"
  #include "includes/digitalWriteFast.h"

  /* COMPATIBILITY_MODE set to true:
  Run the network with a cautious speed to support all architectures.
  8Mhz devices will be able to communicate with 16 or more Mhz devices.
     COMPATIBILITY_MODE set to false:
  Run the network at full speed with the best performances, but is
  indicated only for networks made by a group of devices with the
  same architecture / processor (for example 10 Arduino Uno) */

  #define  COMPATIBILITY_MODE false

  /* The following constants setup are quite conservative and determined only
     with a huge amount of time and blind testing (without oscilloscope)
     tweaking values and analysing results. Theese can be changed to obtain
     faster speed. Probably you need experience, time and an oscilloscope. */


  #if (COMPATIBILITY_MODE)
    #define BIT_WIDTH 40
    #define BIT_SPACER 112
    #define ACCEPTANCE 40
    #define READ_DELAY 4
  #else
    /* Values known to work
       BIT_WIDTH 18
       BIT_SPACER 54
       ACCEPTANCE 18
       READ_DELAY 8
    */
    #if defined(ARDUINO_SAMD_ZERO ) 
      #define BIT_WIDTH 14
      #define BIT_SPACER 42
      #define ACCEPTANCE 14
      #define READ_DELAY 4
/*    #elif ( (F_CPU == 48000000)|| defined(ARDUINO_ARCH_SAMD) || defined(__SAMD21G18A__) )
      #define BIT_WIDTH 18
      #define BIT_SPACER 54
      #define ACCEPTANCE 18
      #define READ_DELAY 8 */
    #elif defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)
      #if ( F_CPU == 16000000 )
        /* Mega has shorter values than Duemianove / Uno because
        micros() produces longer delays on ATmega1280/2560 */      
        #define BIT_WIDTH 18
        #define BIT_SPACER 54
        #define ACCEPTANCE 18
        #define READ_DELAY 8
      #else  // internal clock 8000000
        #define BIT_WIDTH 38
        #define BIT_SPACER 110
        #define ACCEPTANCE 38
        #define READ_DELAY 16
      #endif
    #elif defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
      // Internal clock F_CPU == 80000
      /* ATtiny has shorter values than Duemianove / Uno because
      micros() produces longer delays on ATtiny45/85 */
      #define BIT_WIDTH  35
      #define BIT_SPACER 107
      #define ACCEPTANCE 35
      #define READ_DELAY 15
    #else
      #define BIT_WIDTH 40
      #define BIT_SPACER 112
      #define ACCEPTANCE 40
      #define READ_DELAY 4    
    #endif            
  #endif


 

// Protocol symbols
#define ACK        6
#define NAK        21
#define FAIL       0x100
#define BUSY       666
#define BROADCAST  124
#define TO_BE_SENT 74

// Errors
#define CONNECTION_LOST 101
#define PACKETS_BUFFER_FULL 102
#define MEMORY_FULL 103
#define CONTENT_TOO_LONG 104

// Maximum sending attempts before throwing CONNECTON_LOST error
#define MAX_ATTEMPTS 250

// Packets buffer length, if full PACKET_BUFFER_FULL error is thrown
#define MAX_PACKETS 10

// Max packet length, higher if necessary (affects memory)
#define PACKET_MAX_LENGTH 50

struct packet {
  uint8_t attempts;
  uint8_t device_id;
  char *content;
  uint8_t length;
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

    uint8_t get_id();
    void set_id(uint8_t id);
    void set_receiver(receiver r);
    void set_error(error e);

    uint16_t receive_byte();
    uint16_t receive();
    uint16_t receive(uint32_t duration);

    void send_bit(uint8_t VALUE, uint32_t duration);
    void send_byte(uint8_t b);
    uint16_t  send_string(uint8_t id, char *string, uint8_t length);
    uint16_t  send(uint8_t id, char *packet, uint8_t length, uint32_t timing = 0);

    void update();
    void remove(uint16_t id);

    uint8_t read_byte();
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
