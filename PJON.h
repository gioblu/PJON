
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
  __________________________________________________________________________________
 |PJON Standard mode | SPEED SETUP CHANGES ACCORGING TO YOUR IDE VERSION!           |
 |----------------------------------------------------------------------------------|
 |Arduino 00x   -> bit_width 20 | bit_spacer 68 | acceptance 16 | read_delay 7      |
 |Arduino 1.6.x -> bit_width 20 | bit_spacer 68 | acceptance  8 | read_delay 5      |
 |----------------------------------------------------------------------------------|
 |Absolute bandwidth:  3.16-3.28 kB/s | Transfer speed: 4.32kB/s                    |
 |Practical bandwidth: 2.19-2.45 kB/s | Baud rate: 32256 baud/s                     |
 |Accuracy: 99.45-99.95%              |                                             |
 |----------------------------------------------------------------------------------|
  __________________________________________________________________________________
 |PJON Fast mode | SPEED SETUP CHANGES ACCORGING TO YOUR IDE VERSION!               |
 |----------------------------------------------------------------------------------|
 |Arduino 00x   -> bit_width 18 | bit_spacer 40 | acceptance 18 | read_delay 8      |
 |Arduino 1.6.x -> bit_width 18 | bit_spacer 40 | acceptance  8 | read_delay 5      |
 |----------------------------------------------------------------------------------|
 |Absolute bandwidth:  3.25-3.81 kB/s | Transfer speed: 4.95kB/s                    |
 |Practical bandwidth: 2.52-2.85 kB/s | Baud rate: 39600 baud/s                     |
 |Accuracy: 94.51-98.63%              |                                             |
 |----------------------------------------------------------------------------------|  */

#ifndef PJON_h
  #define PJON_h

  #if defined(ARDUINO) && (ARDUINO >= 100)
    #include "Arduino.h"
    #include "includes/digitalWriteFast.h"
    // Function execution time changes according to your IDE version 
    // So here we have to set dedicated timing to 0.x version
    // STANDARD MODE - if you need more speed please follow the comments above
    #define bit_width 20
    #define bit_spacer 68
    #define acceptance 8
    #define read_delay 5
  #else
    #include "WProgram.h"
    #include "WConstants.h"
    #include "includes/digitalWriteFast.h"
    // Function execution time changes according to your IDE version (0.x or 1.x) 
    // So here we have to set dedicated timing to 1.x version
    // STANDARD MODE - if you need more speed please follow the comments above
    #define bit_width 20
    #define bit_spacer 68
    #define acceptance 16
    #define read_delay 7
  #endif
#endif

#define max_package_length 100

#define ACK  6
#define NAK  21
#define FAIL 0x100
#define BUSY 666
#define BROADCAST 124
#define TO_BE_SENT 74
#define CMD 88

#define encryption_key "19id°?=(!$=<zkl"
#define encryption_strength 2

#define swap(a,b) do { int t = _s_box[a]; _s_box[a] = _s_box[b]; _s_box[b] = t; } while(0)

#define max_reactions 20
#define max_packets 20

struct packet {
  uint8_t device_id;
  char *content;
  int state;
  unsigned long registration;
  unsigned long timing;
};

struct reaction {
  void (*execution)(void);
  char command_type;
  boolean active;
  boolean once;
  boolean empty;
};

class PJON {

  public:

    PJON(int input_pin, uint8_t ID);
    void set_collision_avoidance(boolean state);
    void set_acknowledge(boolean state);
    void set_encryption(boolean state);

    void update();
    int send(uint8_t ID, char *packet, unsigned long timing = 0);
    void remove(int packet_id);

    void crypt(char *content, boolean initialization_vector = false, boolean side = false);
    uint8_t generate_IV(uint8_t string_length);

    void send_bit(uint8_t VALUE, int duration);
    void send_byte(uint8_t b);

    int send_string(uint8_t ID, char *string);
    int send_string(uint8_t ID, char *string, int count);

    int send_command(uint8_t ID, char command_type, unsigned int value, int count = 0);
    int send_short_command(byte ID, char command_type, int count = 0);

    uint8_t syncronization_bit();
    uint8_t read_byte();

    boolean can_start();

    int receive_byte();
    int receive();
    int receive(unsigned long duration);

    void activate_reaction(uint8_t id);
    void deactivate_reaction(uint8_t id);
    uint8_t insert_reaction(char command_type, void (*task)(void), boolean once);
    void remove_reaction(uint8_t id);
    void process_reaction();
    boolean compare_reaction(char command_type);

    uint8_t data[max_package_length];
    char hash[max_package_length];
    packet packets[max_packets];
    reaction reactions[max_reactions];

  private:
    uint8_t _device_id;
    int _input_pin;
    int _read_delay;
    unsigned char _s_box[encryption_strength];

    boolean _acknowledge;
    boolean _collision_avoidance;
    boolean _encryption;
};
