#pragma once

#ifndef ESP32
#error "shouldn't happen"
#endif

// ESP includes
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <assert.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"
#include "tcpip_adapter.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_now.h"
#include "rom/ets_sys.h"
#include "rom/crc.h"
#include "esp_wifi_types.h"

static wifi_country_t wifi_country = {
        cc:"AU",
        schan:1,
        nchan:14,
        max_tx_power:80, // Level0
        policy: WIFI_COUNTRY_POLICY_MANUAL
};


static const char *TAG = "espnow";

/* ESPNOW can work in both station and softap mode. It is configured in menuconfig. */
#if CONFIG_STATION_MODE
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
#else
#define ESPNOW_WIFI_MODE WIFI_MODE_AP
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_AP
#endif

#define ESPNOW_MAX_PACKET   250
#define ESPNOW_QUEUE_SIZE           6

static uint8_t espnow_broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#define IS_BROADCAST_ADDR(addr) (memcmp(addr, espnow_broadcast_mac, ESP_NOW_ETH_ALEN) == 0)


typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
} espnow_event_send_cb_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    int data_len;
} espnow_packet_t;

enum {
    ESPNOW_DATA_BROADCAST,
    ESPNOW_DATA_UNICAST,
    ESPNOW_DATA_MAX,
};


static uint8_t last_mac[ESP_NOW_ETH_ALEN];
static TaskHandle_t pjon_task_h = NULL;
static xQueueHandle espnow_recv_queue = NULL;

static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {

    // The only thing we do in the send callback is unblock the
    // other thread which blocks after posting data to the MAC

    xTaskNotifyGive(pjon_task_h);

    if (mac_addr == NULL) {
        ESP_LOGE(TAG, "Send cb arg error");
        return;
    }

    return;
}


static void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    espnow_packet_t packet;

    if (mac_addr == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Receive cb arg error");
        return;
    }

    memcpy(packet.mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    packet.data = (uint8_t *) malloc(len);
    if (packet.data == NULL) {
        ESP_LOGE(TAG, "Malloc receive data fail");
        return;
    }
    memcpy(packet.data, data, len);
    packet.data_len = len;
    //Post to the queue, but don't wait
    if (xQueueSend(espnow_recv_queue, &packet, 0) != pdTRUE) {
        ESP_LOGW(TAG, "Send receive queue fail");
        free(packet.data);
    }
}

class ENHelper {

    uint8_t _magic_header[4];
    uint8_t _channel = 14;
    uint8_t _esp_pmk[16];

public:

    void add_node_mac(uint8_t mac_addr[ESP_NOW_ETH_ALEN]) {
        ESP_ERROR_CHECK(add_peer(mac_addr));
    }

    esp_err_t add_peer(uint8_t mac_addr[ESP_NOW_ETH_ALEN]) {

        if (esp_now_is_peer_exist(mac_addr)){
            return ESP_OK;
        }

        /* Add broadcast peer information to peer list. */
        esp_now_peer_info_t *peer = (esp_now_peer_info_t *) malloc(sizeof(esp_now_peer_info_t));
        if (peer == NULL) {
            ESP_LOGE(TAG, "Malloc peer information fail");
            vSemaphoreDelete(espnow_recv_queue);
            esp_now_deinit();
            return ESP_FAIL;
        }
        memset(peer, 0, sizeof(esp_now_peer_info_t));
        peer->channel = _channel;
        peer->ifidx = ESPNOW_WIFI_IF;
        if (IS_BROADCAST_ADDR(mac_addr)) {
            peer->encrypt = false;
        }
//        else {
//            peer->encrypt = true;
//            memcpy(peer->lmk, _esp_pmk, 16);
//        }
        memcpy(peer->peer_addr, mac_addr, ESP_NOW_ETH_ALEN);
        ESP_ERROR_CHECK(esp_now_add_peer(peer));
        free(peer);
        return ESP_OK;

    }


    bool begin(uint8_t channel, uint8_t *espnow_pmk) {

        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        pjon_task_h = xTaskGetCurrentTaskHandle();
        _channel = channel;
        memcpy(_esp_pmk, espnow_pmk, 16);

        if (espnow_recv_queue != NULL) {
            return ESP_FAIL;
        }

        espnow_recv_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_packet_t));

        if (espnow_recv_queue == NULL) {
            ESP_LOGE(TAG, "Create mutex fail");
            return ESP_FAIL;
        }

        tcpip_adapter_init();
        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));
        ESP_ERROR_CHECK(esp_wifi_set_country(&wifi_country));
        ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
        ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
        ESP_ERROR_CHECK(esp_wifi_start());
        ESP_ERROR_CHECK(esp_wifi_set_channel(_channel, WIFI_SECOND_CHAN_NONE));

        /* Initialize ESPNOW and register sending and receiving callback function. */
        ESP_ERROR_CHECK(esp_now_init());
        ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
        ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

        /* Set primary master key. */
        ESP_ERROR_CHECK(esp_now_set_pmk(_esp_pmk));

        /* Add broadcast peer information to peer list. */
        add_peer(espnow_broadcast_mac);

        return true;
    }

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {

        // see if there's any received data waiting
        espnow_packet_t packet;

        if (xQueueReceive(espnow_recv_queue, &packet, 0) == pdTRUE) {

            if (packet.data_len >= 4) {
                uint8_t len = packet.data_len - 4;

                if (
                        (packet.data[0                  ]   ^ len) != _magic_header[0] ||
                        (packet.data[1                  ]   ^ len) != _magic_header[1] ||
                        (packet.data[packet.data_len - 2]   ^ len) != _magic_header[2] ||
                        (packet.data[packet.data_len - 1]   ^ len) != _magic_header[3]) {

                    ESP_LOGE(TAG, "magic mismatch");
                    free(packet.data);
                    return PJON_FAIL;
                }

                if (len > max_length) {
                    free(packet.data);
                    ESP_LOGE(TAG, "buffer overflow - %d bytes but max is %d", len, max_length);
                    return PJON_FAIL;
                }

                memcpy(string, packet.data + 2, len);
                free(packet.data);

                //Update last mac received from
                memcpy(last_mac, packet.mac_addr, ESP_NOW_ETH_ALEN);

                return len;

            } else {
                ESP_LOGE(TAG, "packet < 4 received");
                free(packet.data);
                return PJON_FAIL; //no data waiting
            }
        }

        return PJON_FAIL;
    }

    void send_string(uint8_t *string, uint16_t length, uint8_t dest_mac[ESP_NOW_ETH_ALEN]) {
        uint8_t packet[ESPNOW_MAX_PACKET];

        if (length + 4 > ESPNOW_MAX_PACKET) {
            ESP_LOGE(TAG, "Packet send error - too long :%d",length + 4);
            return;
        }

        uint8_t len = length;

        packet[0] = _magic_header[0] ^ len;
        packet[1] = _magic_header[1] ^ len;

        memcpy(packet+2, string, len);

        packet[len + 2] = _magic_header[2] ^ len;
        packet[len + 3] = _magic_header[3] ^ len;

        if (esp_now_send(dest_mac, packet, len + 4) != ESP_OK) {
            ESP_LOGE(TAG, "Send error");
        } else {
            // Wait for notification that the data has been received by the MAC
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
    }

    void send_response(uint8_t response) {
        send_string(&response, 1, last_mac);
    }

    void send_string(uint8_t *string, uint16_t length) {
        // Broadcast
        send_string(string, length, espnow_broadcast_mac);
    }

    void set_magic_header(uint8_t *magic_header) { memcpy(_magic_header,magic_header,4); }

    void get_sender(uint8_t *ip) {
        memcpy(ip, last_mac, ESP_NOW_ETH_ALEN);
    }
};
