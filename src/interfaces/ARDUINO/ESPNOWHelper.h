#pragma once

#if !defined(ESP8266) && !defined(ESP32)
	#error "ESP8266 or ESP32 constant is not defined."
#endif

#include "PacketQueue.h"
#include "PJONDefines.h"

#if defined(ESP8266)
	#include <c_types.h>
	#include <espnow.h>
	#include <ESP8266WiFi.h>
#elif defined(ESP32)
	#include <esp_now.h>
	#include <nvs_flash.h>
	#include <esp_wifi.h>
#endif

#if defined(CONFIG_STATION_MODE) // ESPNOW can work in both station and softap mode. It is configured in menuconfig.
	#if defined(ESP8266)
		#define ESPNOW_WIFI_MODE WIFI_STA
		#define ESPNOW_WIFI_ROLE ESP_NOW_ROLE_CONTROLLER
	#elif defined(ESP32)
		#define ESPNOW_WIFI_MODE WIFI_MODE_STA
		#define ESPNOW_WIFI_IF ESP_IF_WIFI_STA
	#endif
 
#else
	#if defined(ESP8266)
		#define ESPNOW_WIFI_MODE WIFI_AP
		#define ESPNOW_WIFI_ROLE ESP_NOW_ROLE_COMBO
	#elif defined(ESP32)
		#define ESPNOW_WIFI_MODE WIFI_MODE_AP
		#define ESPNOW_WIFI_IF ESP_IF_WIFI_AP
	#endif
#endif




static uint8_t espnow_broadcast_mac[ESP_NOW_MAC_LENGTH] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};



enum {
  ESPNOW_DATA_BROADCAST,
  ESPNOW_DATA_UNICAST,
  ESPNOW_DATA_MAX,
};

static uint8_t last_mac[ESP_NOW_MAC_LENGTH];
static PacketQueue packetQueue;
volatile bool sendingDone = true;

static void espnow_send_cb(const uint8_t *mac_addr, uint8_t status) {
	// The only thing we do in the send callback is unblock the other thread which blocks after posting data to the MAC
	sendingDone = true; 
};

static void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    // Add the packet to the incoming queue.
    // no error checking, if queue is full just ignore the packet.
	packetQueue.push(mac_addr, data, len);
};

class ENHelper {
	uint8_t _channel = 14;
	uint8_t _esp_pmk[16];

public:
	void add_node_mac(uint8_t mac_addr[ESP_NOW_MAC_LENGTH]) {
		add_peer(mac_addr);
	};

	void add_peer(uint8_t mac_addr[ESP_NOW_MAC_LENGTH]) {
		if(esp_now_is_peer_exist(mac_addr))
			return;

		#if defined(ESP8266) // TODO: Fix encryption! ESP_NOW_ROLE_SLAVE doesn't seem to matter. From the doc: The peer's Role does not affect any function, but only stores the Role information for the application layer.
			esp_now_add_peer(mac_addr, ESP_NOW_ROLE_SLAVE,_channel, NULL, 0); 

		#elif defined(ESP32) // TODO: Why malloc a local variable? Add broadcast peer information to peer list.
			esp_now_peer_info_t *peer = (esp_now_peer_info_t *)malloc(sizeof(esp_now_peer_info_t));

			memset(peer, 0, sizeof(esp_now_peer_info_t));
			peer->channel = _channel;
			peer->ifidx = ESPNOW_WIFI_IF;
			if( (memcmp(mac_addr, espnow_broadcast_mac, ESP_NOW_MAC_LENGTH) == 0))
				peer->encrypt = false;
            // else { peer->encrypt = true; memcpy(peer->lmk, _esp_pmk, 16); }
			memcpy(peer->peer_addr, mac_addr, ESP_NOW_MAC_LENGTH);
			esp_now_add_peer(peer);
			free(peer);
		#endif
	};

	bool begin(uint8_t channel, uint8_t *espnow_pmk) {
		 #if defined(ESP32) // TODO: Is this necessary?
			esp_err_t ret = nvs_flash_init();
			if(ret == ESP_ERR_NVS_NO_FREE_PAGES) {
				ESP_ERROR_CHECK(nvs_flash_erase());
				ret = nvs_flash_init();
			}
			ESP_ERROR_CHECK(ret);
		#endif

		_channel = channel;
		memcpy(_esp_pmk, espnow_pmk, 16);

		#ifdef ESP8266
			WiFi.mode(ESPNOW_WIFI_MODE);
			wifi_set_channel(_channel);
			esp_now_init();
			esp_now_set_self_role(ESPNOW_WIFI_ROLE);
			esp_now_register_send_cb(reinterpret_cast<esp_now_send_cb_t>(espnow_send_cb));
			esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(espnow_recv_cb));
		#else
			tcpip_adapter_init();
			wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
			esp_wifi_init(&cfg);
			//esp_wifi_set_country(&wifi_country);
			esp_wifi_set_storage(WIFI_STORAGE_RAM);
			esp_wifi_set_mode(ESPNOW_WIFI_MODE);

			// These two steps are required BEFORE the channel can be set
			esp_wifi_start();
			esp_wifi_set_promiscuous(true);

			esp_wifi_set_channel(_channel, WIFI_SECOND_CHAN_NONE);
			esp_now_init();
			esp_now_register_send_cb(reinterpret_cast<esp_now_send_cb_t>(espnow_send_cb));
			esp_now_register_recv_cb(reinterpret_cast<esp_now_recv_cb_t>(espnow_recv_cb));
			ESP_ERROR_CHECK(esp_now_set_pmk(_esp_pmk));
		#endif

		add_peer(espnow_broadcast_mac); // Add broadcast peer information to peer list

		return true;
	};

	uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
		return packetQueue.pop(last_mac, data, max_length);
	};

	void send_frame(uint8_t *data, uint16_t length, uint8_t dest_mac[ESP_NOW_MAC_LENGTH]) {
		if (!sendingDone) {
             // we are still sending the previous frame - discard this one
			return;
		}

        if (length > PJON_PACKET_MAX_LENGTH) {
            // the data is too long
			return;
		}

		uint8_t packet[PJON_PACKET_MAX_LENGTH + ESPNOW_PACKET_HEADER_LENGTH + ESPNOW_PACKET_FOOTER_LENGTH];

		uint8_t len = length;
		packet[0] = ESP_NOW_MAGIC_HEADER[0] ^ len;
		packet[1] = ESP_NOW_MAGIC_HEADER[1] ^ len;
		memcpy(packet + ESPNOW_PACKET_HEADER_LENGTH, data, len);
		packet[len + 2] = ESP_NOW_MAGIC_HEADER[2] ^ len;
		packet[len + 3] = ESP_NOW_MAGIC_HEADER[3] ^ len;

		ATOMIC()
		{
			sendingDone = false;
		}
		if(esp_now_send(dest_mac, packet, len + ESPNOW_PACKET_HEADER_LENGTH + ESPNOW_PACKET_FOOTER_LENGTH) == 0) {
			do { // wait for sending finished interrupt to be called
				yield(); 
			} while(!sendingDone);
		}
  };

	void send_response(uint8_t response) {
		send_frame(&response, 1, last_mac);
	};

	void send_frame(uint8_t *data, uint16_t length) { // Broadcast
		send_frame(data, length, espnow_broadcast_mac);
	};

	void get_sender(uint8_t *ip) {
		memcpy(ip, last_mac, ESP_NOW_MAC_LENGTH);
	};
};
