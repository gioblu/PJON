#include <MinimalAccelStepper.h>
#define PJON_MINIMAL true
#include "d:\Projects\PJON\src\PJON.h"
//#include <PJON.h>
/*
 * ATTiny85 Stepper client.
 * ========================
 * Using A988 Shield
 * Configure: Pin2 for PJON
 * Pin 1 for direction
 * Pin 0 for steps
 * External clock on pin 3 and 4
 */

//////////////////////////////////
#define myId 10
////////////////////////////////

PJON<SoftwareBitBang> bus(myId);
MinimalAccelStepper stepper(0 ,1);
byte receiveState = 0;
byte currentCommand = 0;
long reportAtStep=-1;
bool movementTriggered=false;
bool doReport=false;
char buf[10];

void setup() {
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
//  pinMode(2, INPUT);
  bus.strategy.set_pin(2);
  bus.set_receiver(receiver_function);
  bus.begin();

};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  payload[length]=0;
  if (receiveState == 0) {
    receiveState++;
    currentCommand = (int)atol(payload);
    return;
  }
  receiveState=0;
  long param = atol(payload);
  switch (currentCommand) {
  case 5: 
    stepper.setCurrentPosition(0);
    stepper.moveTo(0);
    break;
  case 9: 
    stepper.move(param);
    movementTriggered=true;
    break;
 
  case 7:   
    stepper.setAcceleration((float)param);
    break;
  
  case 6: 
    stepper.setMaxSpeed((float)param);
    break;
 
  case 8:   
    doReport = true;
    break;

  case 11:
    reportAtStep = param;
  }
  //currentCommand=0;
  
}

void loop() {
  bus.receive();
  stepper.run();
  if (movementTriggered || doReport) {
    long currentDis=stepper.distanceToGo(); 
    if ((abs(currentDis) <= reportAtStep) || doReport) {
      doReport=false;
      for (int i=0;i<50;i++) {      
        stepper.run();

        memset(buf,0,10);

        itoa(currentDis,buf,10);
        int x=0;
        for (x=0;x<10;x++) {
          if (buf[x]==0) break;
        }
        if (bus.send_packet(1,buf,x)==PJON_ACK) {
          break;
        }
        PJON_DELAY(50);
      }
      reportAtStep=-1; 
    } 
    if (stepper.distanceToGo()==0) {
      movementTriggered=false;
    }
  }
  
};
