#include <stdint.h>
#define WINDOWS true
#define PJON_INCLUDE_LF true
#include <PJON.h>
#include "MockAccelStepper.h"

enum {
  // direct
  pinLow,
  pinHigh,

  // handlePin
  timedPin,
  timedPinSequence,
  pwmPin,

  // motor
  setCurrentPosition,
  setSpeed,
  setAcceleration,
  moveTo,
  move,
  home,
  reportFinished,

  // sensor
  blockingReadDigital,
  blockingReadAnalog,
  asynchReadDigital,
  asynchReadAnalog,
  continualRead,

  // meta
  configure,       
  initialize,
  sendState,
  sleep,
  wakeUp,
  testSerialComm,
  resync,
  error
};


PJON<LocalFile> testBus(44);
int receiveState;
int currentCommand;
MockAccelStepper stepper = MockAccelStepper();
bool didReceive = false;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  
  char buff[length+1] = {0};
  memcpy(buff, payload,length);
  printf("got somethng %d\n",atoi(buff));
  payload[length]=0;
  if (receiveState == 0) {
    receiveState++;
    currentCommand = atoi(buff);
    return;
  }
  receiveState=0;
  long param = (long) atoi(buff);
  switch (currentCommand) {
  case move: 
    stepper.move(param);
    break;

  case moveTo:  
    stepper.moveTo(param);
    break;
    
  case setAcceleration:   
    stepper.setAcceleration((float)param);
    break;
  
  case setSpeed: 
    stepper.setMaxSpeed((float)param);
    break;
 
  case home:   
    stepper.setCurrentPosition(0);
    break;

  }
  currentCommand=0;

}

int main(int argc,  char** argv) 
{ 
    testBus.set_receiver(receiver_function);
    testBus.begin();

    printf("Init done\n"); 
    while (!didReceive) {
        uint16_t resp = testBus.receive(1000);
    }
    return 0; 
};
