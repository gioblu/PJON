#ifndef RemoteStepper_h
#include "RemoteStepper.h"
#endif
//g++ -std=c++11 -pthread -Ic:/Users/det/Documents/Arduino/Libraries/PJON/src MockAccelStepper.cpp RemoteStepper.cpp MockMocController.cpp -o MockMocController
#ifdef TEST_SETUP
#include <stdio.h>
#endif
#ifndef BrickBus_h
#include "BrickBus.h"
#endif

RemoteStepper::RemoteStepper(int stepperId) 
{
    deviceId = stepperId;
}

void RemoteStepper::initialize(BrickBus* brickBus) {
    bus = brickBus;

}


void RemoteStepper::setReportFunction(Delegate delegate) {
  int pos = bus->registerDelegate(deviceId,1,delegate);
  if (pos<0) {
    #ifdef TEST_SETUP
        printf("Error, cannot register delegate");
    #else
		Serial.println("Error, cannot register delegate");
	#endif
  } 
  return pos;
}

void RemoteStepper::setGetStateFunction(Delegate delegate) {
  int pos = bus->registerDelegate(deviceId,2,delegate);
  if (pos<0) {
    #ifdef TEST_SETUP
        printf("Error, cannot register delegate");
    #else
		Serial.println("Error, cannot register delegate");
	#endif
  } 
  return pos;
}



void RemoteStepper::setReportThreshold(long reportAtStep) {
  threshold = reportAtStep;
  for(byte i=0;i<bus->maxDelegates;i++) {
	if(bus->deviceIds[i]==deviceId) {
	  itoa(threshold,thresholdAsString,10);
	  bus->mappedPayload[i]=thresholdAsString;
	}
  }
  createCommData(11,threshold);
  bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Set threshold for %d set to %ld\n",deviceId,steps);
			#else
				Serial.print("threshold for ");
				Serial.print(deviceId);
				Serial.print("set to:");
				Serial.println(threshold);
			#endif
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("ERROR: set threshold not set, for %d\n",deviceId);
        #else
            Serial.print("ERROR: MoveTo NOT set. For ");
            Serial.println(deviceId);
        #endif

    }   
}
        
void RemoteStepper::move(long steps) {
	setReportThreshold(threshold);
    createCommData(9,steps);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Moving %d for %ld\n",deviceId,steps);
			#else
				Serial.print("Moving ");
				Serial.print(deviceId);
				Serial.print("for:");
				Serial.println(steps);
			#endif
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("ERROR: not moving, for %d\n",deviceId);
        #else
            Serial.print("ERROR: Move NOT set. For ");
            Serial.println(deviceId);
        #endif

    }   
}

void RemoteStepper::setMaxSpeed(long speed) {
    createCommData(6, speed);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Setspeed for %d set to %ld\n",deviceId,speed);
			#else
				Serial.print("max speed for ");
				Serial.print(deviceId);
				Serial.print("set to:");
				Serial.println(speed);
			#endif    
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("Error setting max speed for %d set to %ld\n",deviceId,speed);
        #else
            Serial.print("ERROR: Speed NOT set. For ");
            Serial.println(deviceId);
        #endif
    }
}
void RemoteStepper::stop() {
    createCommData(5,0);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Stopping %d\n",deviceId);
			#else
				Serial.print("stopping ");
				Serial.println(deviceId);
			#endif    
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("ERROR: Could not stop %d\n",deviceId);
        #else
            Serial.print("ERROR: Could not stop ");
            Serial.println(deviceId);
        #endif
    }
}

void RemoteStepper::setAcceleration(long toVal) {
    createCommData(7,toVal);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Set acceleration for %d set to %ld\n",deviceId,toVal);
			#else
				Serial.print("acceleration for ");
				Serial.print(deviceId);
				Serial.print("set to:");
				Serial.println(toVal);
			#endif    
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("Error setting acceleration for %d set to %ld\n",deviceId,toVal);
        #else
            Serial.print("ERROR: Acceleration NOT set. For ");
            Serial.println(deviceId);
        #endif
    }
}
void RemoteStepper::getState() {
    createCommData(8,0);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Getting state for %ld\n",deviceId,steps);
			#else
				Serial.print("Querying ");
				Serial.println(deviceId);
			#endif
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("ERROR: could not query %d\n",deviceId);
        #else
            Serial.print("ERROR: Device not queried. For ");
            Serial.println(deviceId);
        #endif

    }   
}

void RemoteStepper::createCommData(long command, long param1) {
  commData.deviceId=deviceId;
  commData.commandId=command;
  commData.param=param1;
}


bool RemoteStepper::sendCommand() {
  char cmd[3] = {0};

  itoa(commData.commandId,cmd,10);

  int len = strlen(cmd);

  int result=0;

  for(int attempts=0; attempts<5; attempts++) {
    result = bus->pjonBus.send_packet_blocking(commData.deviceId, cmd, len);
    if(result==PJON_ACK) break;
  } 
  
  if (result!=PJON_ACK) {
    #ifdef TEST_SETUP
        printf("Error %d for %d command %d to %ld\n",result,deviceId,cmd,len);
    #else
        Serial.print("Error:");
        Serial.println(result);
        Serial.print("DeviceID:");
        Serial.println(commData.deviceId);
        Serial.print("Command:");
        Serial.println(cmd);
        Serial.print("Length:");
        Serial.println(len);
    #endif    
    return false;
  }

  char data[10] = {0};
  itoa(commData.param,data,10);
  len = strlen(data);
  for(int attempts=0; attempts<5; attempts++) {
    result = bus->pjonBus.send_packet_blocking(commData.deviceId, data, len);
    if(result==PJON_ACK) break;
  }
  if (result!=PJON_ACK) {
    #ifdef TEST_SETUP
        printf("Error %d for %d command %d to %ld\n",result,deviceId,cmd,len);
    #else
        Serial.print("Error:");
        Serial.println(result);
        Serial.print("DeviceID:");
        Serial.println(commData.deviceId);
        Serial.print("Command:");
        Serial.println(cmd);
        Serial.print("Length:");
        Serial.println(len);
    #endif
    return false;
  }
  return true;
  
}

