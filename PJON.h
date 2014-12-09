
#ifndef OPCP_h
#define OPCP_h
#include "WProgram.h"
#include "WConstants.h"
#include "digitalWriteFast.h"
#endif

#define ACK  6
#define NAK  21
#define FAIL 0x100 
#define BUSY 666

// PJON Standard mode
// Absolute bandwidth 3.0 kb/s | Practical bandwidth 2.4 kb/s | Accuracy: 99.65%
// bit_width 20 - bit_spacer 68 - acceptance 16 - read_delay 9

// PJON Fast mode
// Absolute bandwidth 3.25 kb/b | Practical bandwidth 2.55 kb/s | Accuracy: 94.5%
// bit_width 18 - bit_spacer 36 - acceptance 16 - read_delay 8

#define max_package_length 255
#define bit_width 20
#define bit_spacer 68
#define acceptance 16

class OPCP {

  public:
    
    OPCP(int input_pin, byte ID);
    void set_read_delay(int delay);
    void send_bit(byte VALUE, int duration);
    void send_byte(byte b);
    int send_string(byte ID, const char *string);
    int send_command(byte ID, byte command_type, unsigned int value);

    int receive_bit();
    byte receive_byte();
    boolean can_start();
    int start();
    int receive();
    
    byte received_bytes[max_package_length];    

  private:

    byte _device_id;
    int _input_pin;
    int _read_delay;

};
