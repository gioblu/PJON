#include <stdint.h>
#include <chrono>
#include <thread>
 
// #define LINUX On Mac and Linux
#define PJON_INCLUDE_LOCAL
#include "PJON.h"

bool didReceive = false;
int sendings = -1;
int errors=0;
int clients = -1;

static void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    printf("Received sth on 99\n");
    didReceive = true;
}

int main(int argc,  char** argv) 
{ 
    sendings = atoi(argv[1]);
    clients = atoi(argv[2]);
    printf("Sending %d to %d clients and counting errors\n", sendings, clients);
    PJON<LocalFile> testBus(99);
    testBus.begin();
    printf("Init done\n"); 
    char tosend[100];

    for(int cnt = 1; cnt<=sendings;cnt++) {
        
        for(int client = 1; client<=clients; client++) {
            sprintf(tosend, "hello %d, sending %d and some stuff", client, cnt);
            int result = (testBus.send_packet_blocking(client, tosend, strlen(tosend)));
            if (result!=PJON_ACK) {
                errors++;
                printf("Error sending '%s' #%d %u\n",tosend, cnt, result);
            } else {
                printf("Successfully sent '%s'\n",tosend);
            }

        }
        // sprintf(tosend, "hallo %d and some other stuff", cnt);
        // result = (testBus.send_packet_blocking(112, tosend, strlen(tosend)));
        // if (result!=PJON_ACK) {
        //     errors++;
        //     printf("Error sending '%s' #%d %u\n", tosend,cnt, result);
        // }
    }
    printf("Sender done, total errors: %d\n", errors);
    return errors; 
} 
