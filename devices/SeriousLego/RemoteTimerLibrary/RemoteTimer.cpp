#ifndef RemoteTimer_h
#include "RemoteTimer.h"
#endif
#ifdef TEST_SETUP
#include <stdio.h>
#endif
#ifndef BrickBus_h
#include "BrickBus.h"
#endif

RemoteTimer::RemoteTimer(int stepperId) 
{
    deviceId = stepperId;
}

void RemoteTimer::initialize(BrickBus* brickBus) {
    bus = brickBus;
}


void RemoteTimer::setReportFunction(Delegate delegate) {
  int pos = bus->registerDelegate(deviceId,4,delegate);
  if (pos<0) {
    #ifdef TEST_SETUP
        printf("Error, cannot register delegate");
    #else
		Serial.println("Error, cannot register delegate");
	#endif
  } 
  return pos;
}
        
bool RemoteTimer::start() {
    createCommData(1,0);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Send startTimer for %d set to %ld\n",deviceId,toVal);
			#else
				Serial.print("startTimer sent for ");
				Serial.print(deviceId);
			#endif    
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("Error sending startTimer for %d set to %ld\n",deviceId,toVal);
        #else
            Serial.print("ERROR: startTimer NOT sent. For ");
            Serial.println(deviceId);
        #endif
    }
}

bool RemoteTimer::stop() {
    createCommData(2,0);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Send stopTimer for %d set to %ld\n",deviceId,toVal);
			#else
				Serial.print("stopTimer sent for ");
				Serial.print(deviceId);
			#endif    
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("Error sending stopTimer for %d set to %ld\n",deviceId,toVal);
        #else
            Serial.print("ERROR: stopTimer NOT sent. For ");
            Serial.println(deviceId);
        #endif
    }
}

bool RemoteTimer::setInterval(long interval) {
    createCommData(3,0);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Send setInterval(1) for %d set to %ld\n",deviceId,toVal);
			#else
				Serial.print("setInterval(1) sent for ");
				Serial.print(deviceId);
			#endif    
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("Error sending setInterval(1) for %d set to %ld\n",deviceId,toVal);
        #else
            Serial.print("ERROR: setInterval(1) NOT sent. For ");
            Serial.println(deviceId);
        #endif
    }
    
    createCommData(interval,0);
    result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Send setInterval(2) for %d set to %ld\n",deviceId,toVal);
			#else
				Serial.print("setInterval(2) sent for ");
				Serial.print(deviceId);
			#endif    
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("Error sending setInterval(2) for %d set to %ld\n",deviceId,toVal);
        #else
            Serial.print("ERROR: setInterval(2) NOT sent. For ");
            Serial.println(deviceId);
        #endif
    }
}

void RemoteTimer::createCommData(long command, long param1) {
  commData.deviceId=deviceId;
  commData.commandId=command;
  commData.param=param1;
}


bool RemoteTimer::sendCommand() {
  char cmd[10] = {0};

  ltoa(commData.commandId,cmd,10);

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

  return true;
  
}

