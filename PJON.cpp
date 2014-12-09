
 /*-O//\               
   |-gfo\             
   |yyso:\             
   |yyso+`\            
  /so+:-..`\           
  |+/:ngr-..`\         
   |/:%&-a3f.:/\      
    \+//u/+sosy//\    
     \oo+/oss+ooss\  
       \:/+-.§°-:+oss\
       `|.|        -oy\\
       ->-*/
       
#include "WProgram.h"
#include "WConstants.h"
#include "digitalWriteFast.h"
#include "PJON.h"

PJON::PJON(int input_pin, byte device_id) {
  _input_pin = input_pin;
  _device_id = device_id;
  _read_delay = 9;
}

void PJON::set_read_delay(int delay) {
  _read_delay = delay;
}

void PJON::send_bit(byte VALUE, int duration) {
  digitalWriteFast(_input_pin, VALUE);         
  delayMicroseconds(duration);              
}

void PJON::send_byte(byte b) {                     
  pinModeFast(_input_pin, OUTPUT);
  this->send_bit(HIGH, bit_spacer);                    
  this->send_bit(LOW, bit_width);      

  for(byte mask = 0x01; mask; mask <<= 1)
    this->send_bit(b & mask, bit_width);

}

int PJON::send_string(byte ID, const char *string) {
  int package_length = strlen(string) + 3;
  byte CRC = 0;

  if(!this->can_start())
    return BUSY;
  
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
  int response = FAIL;
  
  while(response == FAIL && micros() - time <= bit_spacer + bit_width)
    response = this->start();   
  
  if(response == NAK)
    return NAK;
  
  if(response == ACK)
    return ACK;
  
  return FAIL;  
};

int PJON::send_command(byte ID, byte command_type, unsigned int value) {
  char bytes_to_send[3] = { command_type, value >> 8, value & 0xFF };
  return this->send_string(ID, bytes_to_send);
}

int PJON::receive_bit() {                   
  delayMicroseconds((bit_width / 2) - _read_delay);
  int bit_value = digitalReadFast(_input_pin);        
  delayMicroseconds(bit_width / 2);
  return bit_value;
}

byte PJON::receive_byte() {                          
  byte byte_value = B00000000;                                                           
  
  delayMicroseconds(bit_width / 2);
  for (int i = 0; i < 8; i++) {
    byte_value += digitalReadFast(_input_pin) << i;
    delayMicroseconds(bit_width);  
  }
  
  return byte_value;                                                              
}

boolean PJON::can_start() {
  pinModeFast(_input_pin, INPUT);
  this->send_bit(0, 8);
  
  if(!this->receive_byte())
    return true;
  
  return false;
}

int PJON::start() {
  pinModeFast(_input_pin, INPUT);
  digitalWriteFast(_input_pin, LOW);
  
  unsigned long time = micros(); 
  while (digitalReadFast(_input_pin) == HIGH && micros() - time <= bit_spacer);
  time = micros() - time;                    
  
  if(time > acceptance && !this->receive_bit())
    return (int)this->receive_byte();  
  
  return FAIL;
}  

int PJON::receive() { 
  int package_length = max_package_length;
  byte CRC = 0;
  
  for (int i = 0; i < package_length; i++) {
    received_bytes[i] = this->start();
    if (received_bytes[i] == FAIL)
      return FAIL;
    
    if(i == 0 && received_bytes[i] != _device_id)
      return BUSY;
    
    if(i == 1)
      package_length = received_bytes[i];
    
    if (i < package_length - 1)
      CRC ^= received_bytes[i];
  }

  if (received_bytes[package_length - 1] == CRC) { 
  
    this->send_byte(ACK); 
    digitalWriteFast(_input_pin, LOW); 
    return ACK; 

  } else { 
  
    this->send_byte(NAK); 
    digitalWriteFast(_input_pin, LOW); 
    return NAK; 
  
  }
  
}

