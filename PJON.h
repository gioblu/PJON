
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
ENCRYPTION: Private key encryption + initialization vector to ensure almost random data stream
  
  0.x IDE VERSION SPEED CONFIGURATIONS:
  __________________________________________________________________________________
 |PJON Standard mode | SPEED SETUP CHANGES ACCORDING TO YOUR IDE VERSION!           |
 |----------------------------------------------------------------------------------|
 | SLOW MODE:  BIT_WIDTH 20 | BIT_SPACER 68 | ACCEPTANCE 16 | READ_DELAY 7          |
 |----------------------------------------------------------------------------------|
 |Absolute bandwidth:  3.16-3.28 kB/s | Transfer speed: 4.32kB/s                    |
 |Practical bandwidth: 2.19-2.45 kB/s | Baud rate: 32256 baud/s                     |
 |Accuracy: 99.45-99.95%              |                                             |
 |----------------------------------------------------------------------------------|
  __________________________________________________________________________________
 |PJON Standard mode | SPEED SETUP CHANGES ACCORDING TO YOUR IDE VERSION!           |
 |----------------------------------------------------------------------------------|
 | FAST MODE:  BIT_WIDTH 18 | BIT_SPACER 40 | ACCEPTANCE 18 | READ_DELAY 8          |
 |----------------------------------------------------------------------------------|
 |Absolute bandwidth:  3.25-3.81 kB/s | Transfer speed: 4.95kB/s                    |
 |Practical bandwidth: 2.52-2.85 kB/s | Baud rate: 39600 baud/s                     |
 |Accuracy: 94.51-98.63%              |                                             |
 |----------------------------------------------------------------------------------|


  1.x IDE VERSION SPEED CONFIGURATIONS:
  __________________________________________________________________________________
 | BIT_WIDTH 20 | BIT_SPACER 68 | ACCEPTANCE  8 | READ_DELAY 6                      |
 |----------------------------------------------------------------------------------|
 |Absolute bandwidth:  3.16-3.28 kB/s | Transfer speed: 4.32kB/s                    |
 |Practical bandwidth: 2.19-2.45 kB/s | Baud rate: 32256 baud/s                     |
 |Accuracy: 99.45-99.95%              |                                             |
 |----------------------------------------------------------------------------------| */

#ifndef PJON_h
  #define PJON_h

  #include "includes/digitalWriteFast.h"

  #if defined(ARDUINO) && (ARDUINO >= 100)
    #include "Arduino.h"
    
    // Function execution time changes according to your IDE version 
    // So here we have to set dedicated timing to 1.x version
    #define BIT_WIDTH 20
    #define BIT_SPACER 68
    #define ACCEPTANCE 8
    #define READ_DELAY 6
  #else
    #include "WProgram.h"
    #include "WConstants.h"
    // Function execution time changes according to your IDE version (0.x or 1.x) 
    // So here we have to set dedicated timing to 0.x version
    // STANDARD MODE - if you need more speed please follow the comments above
    #define BIT_WIDTH 20
    #define BIT_SPACER 68
    #define ACCEPTANCE 16
    #define READ_DELAY 7
  #endif
#endif

#define ACK  6
#define NAK  21
#define FAIL 0x100
#define BUSY 666
#define BROADCAST 124
#define TO_BE_SENT 74

// Exceptions
#define CONNECTION_LOST 101
#define PACKETS_BUFFER_FULL 102

// Maximum sending attempts before throwing CONNECTON_LOST exception
#define MAX_ATTEMPTS 100 

// Packets buffer length, if full PACKET_BUFFER_FULL exception is thrown
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
typedef void (* exception)(uint8_t motivation, uint8_t data);

static void dummy_exception_handler(uint8_t motivation, uint8_t data) {};

class PJON {

  public:
    PJON(int input_pin, uint8_t ID);
    void set_receiver(receiver r);
    void set_exception(exception e);

    int receive_byte();
    int receive();
    int receive(unsigned long duration);

    void send_bit(uint8_t VALUE, int duration);
    void send_byte(uint8_t b);
    int  send_string(uint8_t ID, char *string);
    int  send(uint8_t ID, char *packet, unsigned long timing = 0);
    
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
    exception _exception;
};
