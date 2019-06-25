#include <stdint.h>
#ifdef TEST_SETUP
#endif
#ifndef BrickBus_h
#include "BrickBus.h"
#endif


#include <PJON.h>

class MockRemoteStepper {
  private:
    int deviceId;
    BrickBus* bus;
    // #ifdef TEST_SETUP
    //     PJON<LocalFile> *bus;
    // #else
    //     PJON<SoftwareBitBang> *bus;
    //#endif  
    long currentReportAtStep;
    long *currentReportAtStepPtr;
    long threshold;
    void getDecimalString (char* str, size_t len, long val);
  public:
    MockRemoteStepper(int stepperId, BrickBus* brickBus);
    int setReportFunction(Delegate reportDone);
    void setReportThreshold(long reportAtStep);
    void move(long steps);
    void moveTo(long steps);
    void setMaxSpeed(long speed);
    void setAcceleration(long toVal);
    long distanceToGo();
    long currentPosition();
    void defineHome();
};