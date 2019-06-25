#pragma once
#ifndef RemoteTimer_h
#define RemoteTimer_h
#endif
#include "options.h"
#ifndef BrickBus_h
#include "BrickBus.h"
#endif

#include <PJON.h>

class RemoteTimer {
    private:
        struct CommData{
            long deviceId;
            long commandId;
            long param;
        };
        BrickBus* bus;
        CommData commData={};
        int deviceId;
		char* thresholdAsString = (char *) malloc(10);
        void createCommData(long command, long param);
        bool sendCommand();
    public:
        RemoteTimer(int timerId);
		void initialize(BrickBus* bus);
        void setReportFunction(Delegate delegate);
        bool setInterval(long interval);
        bool start();
        bool stop();
        
};
