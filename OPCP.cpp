
#include "WProgram.h"
#include "WConstants.h"
#include "digitalWriteFast.h"
#include "OPCP.h"

OPCP::OPCP(int input_pin, byte device_id) {
	_input_pin = input_pin;
	_device_id = device_id;
}

void OPCP::send_bit(byte VALUE, int duration) {
  digitalWriteFast(_input_pin, VALUE);         
  delayMicroseconds(duration);              
}

void OPCP::send_byte(byte b) {                     
  pinModeFast(_input_pin, OUTPUT);
  this->send_bit(HIGH, bit_spacer);                    
  this->send_bit(LOW, bit_width);                      
  for(int i = 7; i >= 0; i--) this->send_bit(bitRead(b, i) == 1 ? HIGH : LOW, bit_width);
}

int OPCP::send_string(byte ID, const char *string) {
  int package_length = strlen(string) + 3;
  byte CRC = 0;
  if(!this->can_start()) return BUSY;
  this->send_byte(ID);
  CRC ^= ID;
  this->send_byte(package_length);
  CRC ^= package_length;
  
  while(*string) {
    this->send_byte(*string);
    CRC ^= *string;
    *string++;
  }
  
  this->send_byte(CRC);
  digitalWriteFast(_input_pin, LOW); 
  unsigned long time = micros();
  int r = FAIL;
  while(r == FAIL && micros() - time <= bit_spacer + bit_width) r = this->start();   
  if(r == NAK) return NAK;
  if(r == ACK) return ACK;
  return FAIL;  
    
};

int OPCP::send_command(byte ID, byte command_type, unsigned int value) {
  char bytes_to_send[3] = { command_type, value >> 8, value & 0xFF };
  return this->send_string(ID, bytes_to_send);
}

int OPCP::receive_bit() {                            
  unsigned long time = micros();                  
  int bit_value = digitalReadFast(_input_pin);       
  delayMicroseconds(bit_width - (micros() - time));   
  return bit_value;                      
}

byte OPCP::receive_byte() {                          
  byte byte_value = B00000000;                                                           
  for (int i = 7; i >= 0; i--) byte_value += ((this->receive_bit() == HIGH) ? 1 : 0) << i;  
  return byte_value;                                                              
}

boolean OPCP::can_start() {
  pinModeFast(_input_pin, INPUT);
  this->send_bit(0, 10);
  if(this->receive_byte() == 0) return true;
  return false;
}

int OPCP::start() {
  pinModeFast(_input_pin, INPUT);
  digitalWriteFast(_input_pin, LOW);
  unsigned long time = micros(); 
  while (digitalReadFast(_input_pin) == HIGH && micros() - time <= bit_spacer);
  time = micros() - time;                    
  if(time > acceptance && this->receive_bit() == LOW) return (int)this->receive_byte();  
  return FAIL;
}  

int OPCP::receive() { 
  int package_length = max_package_length;
  byte CRC = 0;
  
  for (int i = 0; i < package_length; i++) {
    received_bytes[i] = this->start();
    if (received_bytes[i] == FAIL) return NAK;
    if(i == 0 && received_bytes[i] != _device_id) return BUSY;
    if(i == 1) package_length = received_bytes[i];
    if (i < package_length - 1) CRC ^= received_bytes[i];
  }

  if (received_bytes[package_length - 1] == CRC) { this->send_byte(ACK); return ACK; }
  else { this->send_byte(NAK); return NAK; }
}

