#include <options.h>
#include <BrightBricks.h>
#include <RemoteTimer.h>

bool toggle=false;
long toggleCount = 0;
bool timerStopped=false;
bool timerUpdated=false;

BrickBus bus(10);

RemoteTimer timer(40);

static void timerReport(char* numberOfTicks) {
  toggleCount++;
  long step = atol(numberOfTicks);
  toggle = !toggle;
  if (toggle) {
    digitalWrite(13,HIGH);
  } else {
    digitalWrite(13,LOW);
  }
}


void setup() {
    pinMode(13,OUTPUT);
    digitalWrite(13,LOW);

    Serial.begin(9600);
    Serial.println("Initing\n");
    bus.initialize(11);
    timer.initialize(&bus);
    timer.setReportFunction(timerReport);
    timer.start();
    timer.setInterval(1000000);
    
    Serial.println("Done Initing\n");
}


void loop() {
  bus.update();
  if (toggleCount>10){
    if (!timerUpdated) {
      timer.setInterval(2000000);
      timerUpdated=true;
    }
  }
  if (toggleCount>20){
    if (!timerStopped) {
      timer.stop();
      timerStopped=true;
    }
  }
/*  if (toggleCount>30){
    timer.setInterval(100000);
  }
  if (toggleCount>100){
    timer.setInterval(10000);
  }
  if (toggleCount>500){
    timer.setInterval(100000);
  }
  if (toggleCount>1000){
    timer.stop();
    digitalWrite(13,LOW);
  }
 */ 
}
