
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \|
   |y"s§+`\     Giovanni Blu Mitolo 2012 - 2014
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
   |/:%&-a3f.:/\     PJON is a communication protocol and a network that connects up to 255
    \+//u/+gosv//\    arduino boards with a single digital pin to the same wire and communicates
     \o+&/osw+odss\\   up to 4Kb/s with acknowledge, collision detection, CRC and encpryption.
       \:/+-.§°-:+oss\
        | |       \oy\\   Pull down resistor on the bus is generally used to reduce interference
        > <
       -| |-

ADDRESS: Is possible assign up to 255 different adresses
CRC: XOR Cyclic Redundancy Check to ensure almost errorless data communication
ACKNOLEDGE: Packet delivery is ensured by an acknowledge byte sent by receiver
COLLISION DETECTION: Transmissions collision avoidance analyzing network bus before starting
ENCRYPTION: Private key encryption + initialization vector to ensure almost random data stream

PJON Slow mode
Absolute bandwidth 3.0 kb/s | Practical bandwidth 2.38 kb/s | Accuracy: 99.25%
bit_width 20 - bit_spacer 68 - acceptance 16 - read_delay 9

PJON Medium mode
Absolute bandwidth 3.51 kb/b | Practical bandwidth 2.75 kb/s | Accuracy: 96.6%
bit_width 18 - bit_spacer 36 - acceptance 16 - read_delay 7 */

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

// PJON Fast mode
// Absolute bandwidth 3.40 kb/b | Practical bandwidth 2.7 kb/s | Accuracy: 89.9%
// bit_width 17 - bit_spacer 36 - acceptance 17 - read_delay 5

#define max_package_length 256
#define bit_width 18
#define bit_spacer 36
#define acceptance 16
#define read_delay 7

#define ACK  6
#define NAK  21
#define FAIL 0x100
#define BUSY 666

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
    int send_command(uint8_t ID, uint8_t command_type, unsigned int value);

    uint8_t receive_bit();
    uint8_t receive_byte();

    boolean can_start();

    int start();
    int receive();

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
