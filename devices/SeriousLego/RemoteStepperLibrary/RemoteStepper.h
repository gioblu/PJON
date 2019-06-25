#pragma once
#ifndef RemoteStepper_h
#define RemoteStepper_h
#endif
#include "options.h"
#ifndef BrickBus_h
#include "BrickBus.h"
#endif

#include <PJON.h>

class RemoteStepper {
    private:
        struct CommData{
            long deviceId;
            long commandId;
            long param;
        };
        BrickBus* bus;
        CommData commData={};
        long threshold;        
        int deviceId;
		char* thresholdAsString = (char *) malloc(10);
        void createCommData(long command, long param);
        bool sendCommand();
        void getDecimalString (char* str, size_t len, long val);
    public:
        RemoteStepper(int stepperId);
		void initialize(BrickBus* bus);
		void setGetStateFunction(Delegate delegate);
        void setReportFunction(Delegate delegate);
        void setReportThreshold(long reportAtStep);
        void move(long steps);
		void stop();
        void setMaxSpeed(long speed);
        void setAcceleration(long toVal);
		void getState();
};
