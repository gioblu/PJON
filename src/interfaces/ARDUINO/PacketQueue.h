#pragma once

#if defined(ESP8266)
    #include <c_types.h>
    #include <espnow.h>
#elif defined(ESP32)
    #include <esp_now.h>
#endif

#include <SimplyAtomic.h> // https://github.com/wizard97/SimplyAtomic

#ifndef ESP_NOW_ETH_ALEN
    #define ESP_NOW_ETH_ALEN 6
#endif

#define ESPNOW_QUEUE_SIZE 200

typedef struct
{
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    int data_len;
} espnow_packet_t;

/**
 * A queue of incoming packets of max capacity ESPNOW_QUEUE_SIZE.
 * The queue should be atomic (not tested).
 * This was done to remove a dependency on FreeRTOS - replacement of xQueueHandle (and related functions).
 */
class PacketQueue
{
    private:
        espnow_packet_t* queue[ESPNOW_QUEUE_SIZE + 1];
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
        int firstSpacePlus1 = (firstSpace + 1) % ESPNOW_QUEUE_SIZE;
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
            firstElement = (firstElement + 1) % ESPNOW_QUEUE_SIZE;
        }

        return packet;
    }
}