#pragma once

#if defined(ESP8266)
    #include <c_types.h>
    #include <espnow.h>
#elif defined(ESP32)
    #include <esp_now.h>
#endif

#include <SimplyAtomic.h> // https://github.com/wizard97/SimplyAtomic
#include "PJONDefines.h"

#ifndef ESP_NOW_MAC_LENGTH
    #define ESP_NOW_MAC_LENGTH 6
#endif

typedef struct
{
    uint8_t mac_addr[ESP_NOW_MAC_LENGTH];
    uint8_t *data;
    int data_len;
} espnow_packet_t;

/**
 * A queue of incoming packets of max capacity PJON_MAX_PACKETS.
 * The queue should be atomic (not tested).
 * This was done to remove a dependency on FreeRTOS - replacement of xQueueHandle (and related functions).
 */
class PacketQueue
{
    private:
        // 1 extra capacity is necessary to distinguisch between the queue being full and empty
        espnow_packet_t* queue[PJON_MAX_PACKETS + 1];
        volatile uint8_t firstElement = 0;
        volatile uint8_t firstSpace = 0;
        
    public:
        /**
         * Adds a packet to the queue.
         * @return true if successfull, false if the queue was full and the packet was not added.
         */
        bool push(espnow_packet_t* packet);

        /**
         * Remove the first packet from the queue and return it.
         * @return first packet or NULL if the queue is empty
         */
        espnow_packet_t* pop();
};

bool PacketQueue::push(espnow_packet_t* packet)
{   
    bool isFull;

    ATOMIC()
    {
        int firstSpacePlus1 = (firstSpace + 1) % (PJON_MAX_PACKETS + 1);
        isFull = firstSpacePlus1 == firstElement;
        if (!isFull)
        {
            queue[firstSpace] = packet;
            firstSpace = firstSpacePlus1;
        }
    }

    return !isFull;
}

espnow_packet_t* PacketQueue::pop() {
    if (firstElement == firstSpace) {
        return NULL;
    }
    else
    {
        espnow_packet_t* packet;

        ATOMIC()
        {
            packet = queue[firstElement];
            firstElement = (firstElement + 1) % (PJON_MAX_PACKETS + 1);
        }

        return packet;
    }
}