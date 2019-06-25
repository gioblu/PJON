#include <BrightBricks.h>
#include <RemoteStepper.h>

BrickBus bus(10);
RemoteStepper stepper(10);
bool at100 = false;

void At100(char* atStep) {
  at100 = true;
}

void setup() {
    pinMode(13,OUTPUT);
    bus.initialize(11);
    stepper.initialize(&bus);
    stepper.setMaxSpeed(200);
    stepper.setAcceleration(50);
    stepper.setReportFunction(At100);
    stepper.setReportThreshold(100);
    stepper.move(300);
}

void loop() {
  bus.update();
  if (at100) {
    digitalWrite(13,HIGH);
  } else {
    digitalWrite(13,LOW);
  }
  
}
