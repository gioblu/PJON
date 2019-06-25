#include "options.h"
#include "BrickBus.h"
// TODO That needed??
#ifdef TEST_SETUP
#include "MockRemoteStepper.h"
#else
#include "RemoteStepper.h"
#endif
byte BrickBus::maxDelegates=3;
Delegate* BrickBus::delegates = 0; //{emptyReportFunc,emptyReportFunc,emptyReportFunc};
byte* BrickBus::deviceIds=0;//[3]={-1,-1,-1};
byte* BrickBus::deviceTypes=0;//[3]={-1,-1,-1};
char** BrickBus::mappedPayload=0;//[3]={'0','0','0'};

PJON<SoftwareBitBang> BrickBus::pjonBus = PJON<SoftwareBitBang>(1);

