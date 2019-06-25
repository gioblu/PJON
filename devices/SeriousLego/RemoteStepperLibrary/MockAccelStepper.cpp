#ifndef MockAccelStepper_h
#include "MockAccelStepper.h"
#endif
//g++ -std=c++11 -Ic:/Users/det/Documents/Arduino/Libraries/PJON/src  MockAccelStepper.cpp MockTinyReceiver.cpp -o MockTinyReceiver
#include <stdio.h>
#include <thread>
long stepCounter=0;
long currentPos=0;
long nextReportThreshold=0;
bool running=false;
int currentDir=1;
MockAccelStepper::MockAccelStepper() {
    
}

void MockAccelStepper::run() {
    
    if (stepCounter<0) {
        running = true;
        currentDir = 1;
        stepCounter++;
        currentPos--;
    }
    if (stepCounter>0) {
        running = true;
        currentDir = -1;
        stepCounter--;
        currentPos++;
    } 
    if (stepCounter==0) {
        running = false;
    }
}

void MockAccelStepper::move(long param) {
    stepCounter = param;
    printf("Stepper move: %ld\n", param);
}
void MockAccelStepper::moveTo(long param) {
    stepCounter = currentPos + param;
    printf("Stepper moveTo: %ld\n", param);
}
void MockAccelStepper::setAcceleration(float param) {
    printf("Stepper setAcceleration: %ld\n", (long)param);
}
void MockAccelStepper::setMaxSpeed(float param) {
    printf("Stepper setMaxSpeed: %ld\n", (long)param);
}
void MockAccelStepper::setCurrentPosition(int param) {
    currentPos = param;
    printf("Stepper setCurrentPosition %ld\n", param);
};
long MockAccelStepper::currentPosition() {
    return currentPos;
}
long MockAccelStepper::distanceToGo () {
    return stepCounter;
}
