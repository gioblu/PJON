#include <stdint.h>
#include <chrono>
#include <thread>
 
// #define LINUX On Mac and Linux
#define PJON_INCLUDE_LOCAL
#include "PJON.h"

bool didReceive = false;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    printf("Received sth on 99\n");
    didReceive = true;
}

int main() 
{ 
    printf("Sending sth and waiting for response\n");
    PJON<LocalFile> testBus(99);
    testBus.begin();
    printf("Init done\n"); 
    // int x=1234;
    // sprintf(tosend, "hello %d and some stuff", x);
    // x=1235;
    // sprintf(tosend, "hello %d and some stuff", x);
        char tosend[100];

//    while (true) {
    for(int cnt = 1; cnt<=30;cnt++) {
    //    std::this_thread::sleep_for(std::chrono::milliseconds(10));
   //     for (int i=0;i<99;i++) {tosend[i]=0;}
        
        sprintf(tosend, "hello %d and some stuff", cnt);
        int result = (testBus.send_packet_blocking(111, tosend, strlen(tosend)));
        if (result!=PJON_ACK) {
            printf("Error sending '%s' #%d %u\n",tosend, cnt, result);
        }
    //    std::this_thread::sleep_for(std::chrono::milliseconds(10));
      //  for (int i=0;i<99;i++) {tosend[i]=0;}
        sprintf(tosend, "hallo %d and some other stuff", cnt);
        result = (testBus.send_packet_blocking(112, tosend, strlen(tosend)));
        if (result!=PJON_ACK) {
            printf("Error sending '%s' #%d %u\n", tosend,cnt, result);
        }
    }
    printf("Sender done");
    return 0; 
} 
