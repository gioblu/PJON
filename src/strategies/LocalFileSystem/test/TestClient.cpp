#include <stdint.h>
// #define LINUX On Mac and Linux
#define PJON_INCLUDE_LOCAL
#include "PJON.h"

bool didReceive = false;
int busId =0;
int receivedCnt = 0;
int lastReceivedVal = 0;
int endVal=0;
int errorCnt = 0;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    char buffer[100];
    for (int i=0;i<99;i++) {buffer[i]=0;}
    memcpy(buffer, payload,length);
    char prefix[20];
    char postfix[20];
    int client=0;
    int val=0;
    sscanf(buffer, "%s %d, sending %d and some %s", prefix, &client, &val, postfix);
    printf("Controller received  #%u '%s' on %u, value: %d\n",receivedCnt++,buffer,busId, val);
    if(lastReceivedVal != (val - 1)) {
        printf("Warning, we missed something, val: %d, received: %d\n", val, lastReceivedVal);
        errorCnt++;
    } 
    lastReceivedVal = val;
    didReceive = lastReceivedVal==endVal;
}

int main(int argc,  char** argv) 
{ 
    busId = atoi(argv[1]);
    endVal = atoi(argv[2]);
    PJON<LocalFile> testBus(busId);
    printf("Mock Controller waiting on %u\n",busId);

    testBus.set_receiver(receiver_function);
    testBus.begin();

    printf("Init done waiting for %d responses\n", endVal); 
    while (!didReceive) {
       // Be 'nice' to other processes
       std::this_thread::sleep_for(std::chrono::milliseconds(50));
       uint16_t resp = testBus.receive();
    }
    printf("Total errors: %d\n", errorCnt);
    while(true) {
       // Be 'nice' to other processes
       std::this_thread::sleep_for(std::chrono::milliseconds(50));
    };
    return errorCnt; 
} 
