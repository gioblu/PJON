#include <stdio.h>
#include "options.h"
#include <PJON.h>
#include "RemoteStepper.h"

bool finalThresholdReached = false;
PJON<LocalFile> bus(1);
PJON<LocalFile> *busPtr = &bus;
RemoteStepper stepper(44, busPtr);

void motorDone(long atStep) {
    printf("Motor reported done in MockMocController: %ld\n", atStep);
    if (atStep == 300) {
        finalThresholdReached = true;
    } else {
        stepper.setReportThreshold(atStep+100);
    }

}

int main() {
    stepper.setReportFunction(motorDone);
    stepper.setReportThreshold(100);
    int reportingTimes=0;
    stepper.setMaxSpeed(200);
    stepper.setAcceleration(50);
    stepper.move(300);
    
    while(!finalThresholdReached) {
        stepper.update();
        // wait for motor to complete
    };
}