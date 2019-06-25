#include <stdint.h>
#ifndef MockRemoteStepper_h
#include "MockRemoteStepper.h"
#endif

#define WINDOWS true
#define PJON_INCLUDE_LF true

MockRemoteStepper::MockRemoteStepper(int stepperId, BrickBus* brickBus) {
    bus = brickBus;
    currentReportAtStepPtr = &currentReportAtStep;
    deviceId = stepperId;
}

void MockRemoteStepper::getDecimalString (char* str, size_t len, long val)
{
  char i;
  for(i=1; i<=len; i++)
  {
    str[len-i] = (char) ((val % 10UL) + '0');
    val/=10;
  }
}


int MockRemoteStepper::setReportFunction(Delegate delegate) {
  int pos = bus->registerDelegate(deviceId,1,delegate);
  char str[10]={0};
  getDecimalString(str,7,threshold);
  DelegateRepo::instance()->mappedPayload[pos]=str;
  return pos;
}

void MockRemoteStepper::setReportThreshold(long reportAtStep) {
  threshold = reportAtStep;
}

void MockRemoteStepper::move(long steps) {
  PJON<LocalFile> myBus(deviceId);
  myBus.send_packet_blocking(1,"rsr",3);
}
void MockRemoteStepper::moveTo(long steps) {}
void MockRemoteStepper::setMaxSpeed(long speed) {}
void MockRemoteStepper::setAcceleration(long toVal) {}
long MockRemoteStepper::distanceToGo() { return 0;}
long MockRemoteStepper::currentPosition() {return 0;}
void MockRemoteStepper::defineHome() {}