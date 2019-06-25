#pragma once
#ifndef RemoteButton_h
#define RemoteButton_h
#endif
#include "options.h"
#ifndef BrickBus_h
#include "BrickBus.h"
#endif

#include <PJON.h>

class RemoteButton {
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
        RemoteButton(int buttonId);
		void initialize(BrickBus* bus);
        void setReportFunction(Delegate delegate);
        bool getState();
};
