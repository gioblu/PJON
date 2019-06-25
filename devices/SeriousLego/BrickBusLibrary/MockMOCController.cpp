#include "options.h"
#ifdef TEST_SETUP
#include <stdio.h>
#endif
#include "BrightBricks.h"

bool finalThresholdReached = false;
BrickBus bus(11,10);
BrickBus* busPtr = &bus;
MockRemoteStepper stepper(44, busPtr);

void motorDone(char* atStep) {
    long step = atoi(atStep);
    printf("Motor reported done in MockMocController: %ld\n", step);
    finalThresholdReached = true;    
}

int main() {
    stepper.setReportThreshold(100);
    stepper.setReportFunction(motorDone);
    int reportingTimes=0;
    stepper.setMaxSpeed(200);
    stepper.setAcceleration(50);
    stepper.move(300);
    
    while(!finalThresholdReached) {
        bus.update();
    };
}