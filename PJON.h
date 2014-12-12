
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \|
   |y"s§+`\     Giovanni Blu Mitolo 2011 - 2014
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
   |/:%&-a3f.:/\
    \+//u/+gosv//\
     \o+&/osw+odss\\
       \:/+-.§°-:+oss\
       `|.|       \oy\\
       ->-<
       -||*/

#ifndef PJON_h
#define PJON_h
#include "WProgram.h"
#include "WConstants.h"
#include "includes/digitalWriteFast.h"
#endif

// PJON Slow mode
// Absolute bandwidth 3.0 kb/s | Practical bandwidth 2.38 kb/s | Accuracy: 99.25%
// bit_width 20 - bit_spacer 68 - acceptance 16 - read_delay 9

// PJON Medium mode
// Absolute bandwidth 3.51 kb/b | Practical bandwidth 2.75 kb/s | Accuracy: 96.6%
// bit_width 18 - bit_spacer 36 - acceptance 16 - read_delay 7

// PJON Fast mode
// Absolute bandwidth 3.40 kb/b | Practical bandwidth 2.7 kb/s | Accuracy: 89.9%
// bit_width 17 - bit_spacer 36 - acceptance 17 - read_delay 5

#define max_package_length 255
#define bit_width 18
#define bit_spacer 36
#define acceptance 16
#define read_delay 7

#define ACK  6
#define NAK  21
#define FAIL 0x100
#define BUSY 666

class PJON {

  public:

    PJON(int input_pin, byte ID);
    void set_collision_avoidance(boolean state);
    void set_acknowledge(boolean state);

    void send_bit(byte VALUE, int duration);
    void send_byte(byte b);
    int send_string(byte ID, const char *string);
    int send_command(byte ID, byte command_type, unsigned int value);

    int receive_bit();
    byte receive_byte();
    boolean can_start();
    int start();
    int receive();

    byte data[max_package_length];

  private:
    byte _device_id;
    int _input_pin;
    int _read_delay;

    boolean _acknowledge;
    boolean _collision_avoidance;
};
