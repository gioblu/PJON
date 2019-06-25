#pragma once
#ifndef BrickBus_h
#define BrickBus_h
#endif

#include "options.h"
#ifdef TEST_SETUP
#include <stdint.h>
#endif
#include <PJON.h>

typedef void (*Delegate)(char* payload);
static void emptyReportFunc(char* payload) {};

class BrickBus {
    private:
		static Delegate* delegates;

        static byte* deviceTypes;
		static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);
    public:
		static byte maxDelegates;
        static byte* deviceIds;
	    static char** mappedPayload;
		static PJON<SoftwareBitBang> pjonBus;
        BrickBus(byte maxNumberOfDelegates) {maxDelegates = maxNumberOfDelegates;}
        void update();              
		void initialize(int pin);
		int registerDelegate(int deviceId, int deviceType, Delegate delegate);
        void deRegisterDelegateAt(int pos);
};
