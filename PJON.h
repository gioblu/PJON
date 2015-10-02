
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \| v1.0
   |y"s§+`\     Giovanni Blu Mitolo 2012 - 2015
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\     PJON is a device communications bus system that connects up to 255
  \+//u/+g%{osv,,\    arduino boards over one wire up to 4.32KB/s data communication speed.
    \=+&/osw+olds.\\   Contains acknowledge, collision detection, CRC and encpryption all done
       \:/+-.-°-:+oss\  with micros() and delayMicroseconds(), with no use of interrupts or timers.
        | |       \oy\\  Pull down resistor on the bus is generally used to reduce interference.
        > <
       -| |-

DEVICE ID: 255 different adresses can be assigned.
CRC: XOR Cyclic Redundancy Check ensures almost errorless data communication.
ACKNOLEDGE: Acknowledge byte sent by receiver ensures packet delivery.
COLLISION DETECTION: Collision avoidance is ensured analyzing network bus before starting .
PACKET MANAGER: Schedules and manages packet sending and retries in exponential backoff.
ERROR HANDLING: Easy way to catch and program a reaction to network errors. 
  __________________________________________________________________________________
 | BIT_WIDTH 20 | BIT_SPACER 60 | ACCEPTANCE  8 | READ_DELAY 6                      |
 |----------------------------------------------------------------------------------|
 |Transfer speed: 4.32kB/s     | Absolute bandwidth:  2.65 kB/s                     |
 |Baud rate:      32256 baud/s | Practical bandwidth: 2.05 kB/s                     |
 |Accuracy:       99.995%      |                                                    |
 |__________________________________________________________________________________| 
 
Copyright (c) 2013-2015, Giovanni Blu Mitolo All rights reserved.
This software is provided by the copyright holders and contributors "as is" and any express or
implied warranties, including, but not limited to, the implied warranties of merchantability and 
fitness for a particular purpose are disclaimed. In no event shall the copyright holder or 
contributors be liable for any direct, indirect, incidental, special, exemplary, or consequential
damages (including, but not limited to, procurement of substitute goods or services; loss of use,
data, or profits; or business interruption) however caused and on any theory of liability, whether 
in contract, strict liability, or tort (including negligence or otherwise) arising in any way out 
of the use of this software, even if advised of the possibility of such damage. */

#ifndef PJON_h
  #define PJON_h

  #include "includes/digitalWriteFast.h"
  #include "Arduino.h"

  /* The following constants setup is quite conservative and determined only 
     with a huge amount of time and blind testing (without oscilloscope) 
     tweaking values and analysing results. Theese can be changed to obtain
     faster speed. Probably you need experience, time and an oscilloscope. */ 
  
  #define BIT_WIDTH 20
  #define BIT_SPACER 60
  #define ACCEPTANCE 8
  #define READ_DELAY 6
#endif

#define ACK  6
#define NAK  21
#define FAIL 0x100
#define BUSY 666
#define BROADCAST 124
#define TO_BE_SENT 74

// Errors
#define CONNECTION_LOST 101
#define PACKETS_BUFFER_FULL 102

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
  unsigned long registration;
  int state;
  unsigned long timing;
};

typedef void (* receiver)(uint8_t length, uint8_t *payload);
typedef void (* error)(uint8_t code, uint8_t data);

static void dummy_error_handler(uint8_t code, uint8_t data) {};

class PJON {

  public:
    PJON(int input_pin, uint8_t ID);
    void set_receiver(receiver r);
    void set_error(error e);

    int receive_byte();
    int receive();
    int receive(unsigned long duration);

    void send_bit(uint8_t VALUE, int duration);
    void send_byte(uint8_t b);
    int  send_string(uint8_t ID, char *string, uint8_t length);
    int  send(uint8_t ID, char *packet, uint8_t length, unsigned long timing = 0);
    
    void update();
    void remove(int id);

    uint8_t read_byte();
    boolean can_start();

    uint8_t syncronization_bit();

    uint8_t data[PACKET_MAX_LENGTH];
    packet  packets[MAX_PACKETS];

  private:
    uint8_t   _device_id;
    int       _input_pin;
    receiver  _receiver;
    error     _error;
};
