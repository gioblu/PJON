
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

// 2.777kb/s bit_width 32 - bit_spacer 72 - acceptance 32

// STANDARD MODE:
// Theorical 3.1kb/s | Practical 2.18kb/s | Accuracy: 99.94%
// bit_width 28 - bit_spacer 68 - acceptance 16

// FAST MODE: 
// Theorical 3.8kb/s | Practical 2.05kb/s | Accuracy: 90%
// bit_width 24 - bit_spacer 44 - acceptance 16

#define max_package_length 255
#define bit_width 24
#define bit_spacer 68 
#define acceptance 16
#define read_delay 0

class OPCP {

  public:

    OPCP(int input_pin, byte ID);
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

};
