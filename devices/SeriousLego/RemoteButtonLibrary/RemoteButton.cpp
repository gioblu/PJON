#ifndef RemoteButton_h
#include "RemoteButton.h"
#endif
#ifdef TEST_SETUP
#include <stdio.h>
#endif
#ifndef BrickBus_h
#include "BrickBus.h"
#endif

RemoteButton::RemoteButton(int stepperId) 
{
    deviceId = stepperId;
}

void RemoteButton::initialize(BrickBus* brickBus) {
    bus = brickBus;
}


void RemoteButton::setReportFunction(Delegate delegate) {
  int pos = bus->registerDelegate(deviceId,3,delegate);
  if (pos<0) {
    #ifdef TEST_SETUP
        printf("Error, cannot register delegate");
    #else
		Serial.println("Error, cannot register delegate");
	#endif
  } 
  return pos;
}
        
bool RemoteButton::getState() {
    createCommData(0,0);
    bool result = sendCommand();
    if (result) {
		#ifdef LOG_DEBUG
			#ifdef TEST_SETUP
				printf("Send getState for %d set to %ld\n",deviceId,toVal);
			#else
				Serial.print("getState sent for ");
				Serial.print(deviceId);
			#endif    
		#endif
    } else {
        #ifdef TEST_SETUP
            printf("Error sending getState for %d set to %ld\n",deviceId,toVal);
        #else
            Serial.print("ERROR: getState NOT sent. For ");
            Serial.println(deviceId);
        #endif
    }
}

void RemoteButton::createCommData(long command, long param1) {
  commData.deviceId=deviceId;
  commData.commandId=command;
  commData.param=param1;
}


bool RemoteButton::sendCommand() {
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

  return true;
  
}

