/*
  Agenda.h - Library for reflectometer sensor.
  Created by Giovanni Blu Mitolo, November 13, 2014.
  Released into the public domain.
*/

#ifndef OPCP_h
#define OPCP_h
#include "WProgram.h"
#include "WConstants.h"
#include "digitalWriteFast.h"
#endif

#define REQ  255
#define ACK  6
#define NAK  21
#define FAIL 0x100 
#define BUSY 666

#define max_package_length 255
#define bit_width 32
#define bit_spacer 72
#define acceptance 32

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
