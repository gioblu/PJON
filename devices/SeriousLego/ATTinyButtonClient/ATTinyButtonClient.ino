#include <PJON.h>
#define myId 30
/*
 * ATTiny85 button client.
 * ========================
  * Configure: Pin2 for PJON
 * Pin 0 for button read
 * External clock on pin 3 and 4
 */

PJON<SoftwareBitBang> bus(myId);
bool lastButtonState;
bool buttonStateSent;
bool gotCommand = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(0, INPUT);
  pinMode(1, OUTPUT);
  pinMode(2, INPUT);
  bus.strategy.set_pin(2);
  bus.set_receiver(receiver_function);
  bus.begin();
}

void report() {
    if(lastButtonState) {
      bus.send_packet_blocking(1,"1",1); 
    } else {
      bus.send_packet_blocking(1,"0",1); 
    }
}

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  gotCommand=true;
}

void loop() {
  bus.receive();
  bool buttonState = digitalRead(0)==HIGH?true:false;
  if(buttonState!=lastButtonState) {
    lastButtonState=buttonState;
    digitalWrite(1,lastButtonState?HIGH:LOW);
    report();
  }
  if(gotCommand) {
    report();
    gotCommand=false;
  }
}
