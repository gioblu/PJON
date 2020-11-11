#pragma once

#if defined(ESP8266)
	#include <c_types.h>
	#include <espnow.h>
#elif defined(ESP32)
	#include <esp_now.h>
#endif

#include <Arduino.h>
#include "PJONDefines.h"

#define ESP_NOW_MAC_LENGTH 6
#define ESPNOW_PACKET_HEADER_LENGTH 2
#define ESPNOW_PACKET_FOOTER_LENGTH 2
uint8_t* const ESP_NOW_MAGIC_HEADER = (uint8_t*)"\xEE\xFE\x0E\xEF";

class EspNowPacket
{
	private:
		uint8_t mac_address[ESP_NOW_MAC_LENGTH];
		uint8_t data[PJON_PACKET_MAX_LENGTH + ESPNOW_PACKET_HEADER_LENGTH + ESPNOW_PACKET_FOOTER_LENGTH];
		int data_len;

	public:
		void set(const uint8_t *mac_address, const uint8_t *data, int len) {
			memcpy(this->mac_address, mac_address, ESP_NOW_MAC_LENGTH);
			memcpy(this->data, data, len);
			this->data_len = len;
		}

		uint16_t checkAndGet(uint8_t *out_mac_address, uint8_t *out_data, uint16_t max_length) {
			if (data_len < ESPNOW_PACKET_HEADER_LENGTH + ESPNOW_PACKET_FOOTER_LENGTH) {
				// The packet is too small
				return PJON_FAIL;
			}

			uint8_t len = data_len - ESPNOW_PACKET_HEADER_LENGTH - ESPNOW_PACKET_FOOTER_LENGTH;

			if(
				(data[0] ^ len) != ESP_NOW_MAGIC_HEADER[0] ||
				(data[1] ^ len) != ESP_NOW_MAGIC_HEADER[1] ||
				(data[data_len - 2] ^ len) != ESP_NOW_MAGIC_HEADER[2] ||
				(data[data_len - 1] ^ len) != ESP_NOW_MAGIC_HEADER[3]
			) {
				// Magic mismatch
				return PJON_FAIL;
			}

			if (len > max_length) {
				// Too short buffer
				return PJON_FAIL;
			}

			memcpy(out_mac_address, mac_address, ESP_NOW_MAC_LENGTH);
			memcpy(out_data, data + ESPNOW_PACKET_HEADER_LENGTH, len);

			return len;
		}
};

/**
 * A queue of incoming packets of max capacity PJON_MAX_PACKETS.
 * The queue should be atomic (not tested).
 * This was done to remove a dependency on FreeRTOS - replacement of xQueueHandle (and related functions).
 */
class PacketQueue
{
	private:
		// 1 extra capacity is necessary to distinguisch between the queue being full and empty
		EspNowPacket queue[PJON_MAX_PACKETS + 1];
		volatile uint8_t firstElement = 0;
		volatile uint8_t firstSpace = 0;
		
	public:
		/**
		 * Adds a packet to the queue.
		 * @return true if successfull, false if the queue was full and the packet was not added.
		 */
		bool push(const uint8_t *mac_addr, const uint8_t *data, int len);

		/**
		 * Remove the first packet from the queue, check it for length, remove ESPNOW headers and return it.
		 * @param out_mac_address - pass in a buffer, the method will copy the sender's MAC address into it.
		 * @param out_data - pass in an allocated buffer, the method will copy the data into it.
		 * @param max_length - length of the allocated buffer.
		 * @return length of the data or PJON_FAIL in case the queue is empty or there was an error
		 */
		uint16_t pop(uint8_t *out_mac_address, uint8_t *out_data, uint16_t max_length);
};

bool PacketQueue::push(const uint8_t *mac_addr, const uint8_t *data, int len)
{   
	bool isFull;

	noInterrupts();
	{
		int firstSpacePlus1 = (firstSpace + 1) % (PJON_MAX_PACKETS + 1);
		isFull = firstSpacePlus1 == firstElement;
		if (!isFull)
		{
			queue[firstSpace].set(mac_addr, data, len);
			firstSpace = firstSpacePlus1;
		}
	}
	interrupts();

	return !isFull;
}

uint16_t PacketQueue::pop(uint8_t *out_mac_address, uint8_t *out_data, uint16_t max_length) {
	if (firstElement == firstSpace) {
		return PJON_FAIL;
	}
	else
	{
		uint16_t length;

		noInterrupts();
		{
			length = queue[firstElement].checkAndGet(out_mac_address, out_data, max_length);
			firstElement = (firstElement + 1) % (PJON_MAX_PACKETS + 1);
		}
		interrupts();

		return length;
	}
}