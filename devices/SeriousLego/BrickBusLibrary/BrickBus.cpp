#ifndef BrickBus_h
#include "BrickBus.h"
#endif
#ifdef TEST_SETUP
#include <stdio.h>
#endif

//g++ -std=c++11 -Ic:/Users/det/Documents/Arduino/Libraries/PJON/src BrickBus.cpp MockRemoteStepper.cpp DelegateRepo.cpp MockMocController.cpp -o MockMocController

static void BrickBus::receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {

	#ifdef LOG_DEBUG	
		#ifndef TEST_SETUP
			char buffer[10];
			memset(buffer,0,10);
			memcpy(buffer,payload,length);
			Serial.print("receiving from sender: ");
			Serial.println(packet_info.sender_id);
			Serial.print("data from sender: ");
			Serial.println(buffer);
		#else 
			printf("receiving from sender %d\n",packet_info.sender_id);
		#endif
	#endif
	
    for(int i=0;i<maxDelegates;i++) {
        if (packet_info.sender_id == deviceIds[i]) {
            if (deviceTypes[i]==1) // 1: a RemoteStepper provided "threshold Reached" answer
            {
				(delegates[i])(mappedPayload[i]);
            }
            if (deviceTypes[i]==2) // 2: a RemoteStepper provided "getState" answer
            {
				(delegates[i])(payload);
            }
            if (deviceTypes[i]==3) // 3: a RemoteButton provided an answer
            {
				(delegates[i])(payload);
            }
			if (deviceTypes[i]==4) // 3: a RemoteTimer provided a tick
            {
				(delegates[i])(payload);
            }
        }
    }


}
void BrickBus::initialize(int pin) {
	pjonBus.strategy.set_pin(pin);
	pjonBus.set_receiver(receiver_function);
	pjonBus.begin();
	
	delegates = (Delegate *) malloc(maxDelegates * sizeof(Delegate));
	deviceIds = (byte *) malloc(maxDelegates * sizeof(byte));
	deviceTypes = (byte *) malloc(maxDelegates * sizeof(byte));
	mappedPayload = (char **) malloc(maxDelegates * sizeof(char*));
	
	for (int i=0; i<maxDelegates;i++) {
		delegates[i] = emptyReportFunc;
		deviceIds[i] = 0;
		deviceTypes[i]  = 0;
		mappedPayload[i]=0;

	}	

}

void BrickBus::update() {
	pjonBus.receive();
}

int BrickBus::registerDelegate(int forDevice, int type, Delegate delegate) {
    for(int i=0;i<maxDelegates;i++) {  
        if(deviceIds[i]==0) {
            deviceIds[i]=forDevice;
            deviceTypes[i]=type;
            delegates[i]=delegate;
            return i;
        }
    } 

    return -1;   
} 

void BrickBus::deRegisterDelegateAt(int pos) {
	deviceIds[pos]=0;
}


