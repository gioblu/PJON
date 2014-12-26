
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
 |PJON Standard mode |                                                              |
 |----------------------------------------------------------------------------------|
 |bit_width 20 | bit_spacer 68 | acceptance 16 | read_delay 7                       |
 |----------------------------------------------------------------------------------|
 |Absolute bandwidth:  3.16-3.28 kB/s | Transfer speed: 4.32kB/s                    |
 |Practical bandwidth: 2.19-2.45 kB/s | Baud rate: 32256 baud/s                     |
 |Accuracy: 99.45-99.95%              |                                             |
 |----------------------------------------------------------------------------------|
  __________________________________________________________________________________
 |PJON Fast mode |                                                                  |
 |----------------------------------------------------------------------------------|
 |bit_width 18 | bit_spacer 40 | acceptance 18 | read_delay 8                       |
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
  #else
    #include "WProgram.h"
    #include "WConstants.h"
    #include "includes/digitalWriteFast.h"
  #endif
#endif

#define max_package_length 100
#define bit_width 18
#define bit_spacer 40
#define acceptance 18
#define read_delay 8

#define ACK  6
#define NAK  21
#define FAIL 0x100
#define BUSY 666
#define BROADCAST 124

#define encryption_key "19id°?=(!$=<zkl"
#define encryption_strength 2

#define swap(a,b) do { int t = _s_box[a]; _s_box[a] = _s_box[b]; _s_box[b] = t; } while(0)

class PJON {

  public:

    PJON(int input_pin, uint8_t ID);
    void set_collision_avoidance(boolean state);
    void set_acknowledge(boolean state);
    void set_encryption(boolean state);

    void crypt(char *data, boolean initialization_vector = false, boolean side = false);
    uint8_t generate_IV(uint8_t string_length);

    void send_bit(uint8_t VALUE, int duration);
    void send_byte(uint8_t b);

    int send_string(uint8_t ID, char *string);
    int send_string(uint8_t ID, char *string, int count);

    int send_command(uint8_t ID, uint8_t command_type, unsigned int value);

    uint8_t receive_bit();
    uint8_t receive_byte();

    boolean can_start();

    int syncronize();
    int receive();
    int receive(int count);

    uint8_t data[max_package_length];
    char hash[max_package_length];

  private:
    uint8_t _device_id;
    int _input_pin;
    int _read_delay;
    unsigned char _s_box[encryption_strength];

    boolean _acknowledge;
    boolean _collision_avoidance;
    boolean _encryption;
};
