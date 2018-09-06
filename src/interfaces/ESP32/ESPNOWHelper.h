#pragma once

#include <PJON.h>
#include <PJONDefines.h>

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

static const char *TAG = "espnow";

/* ESPNOW can work in both station and softap mode. It is configured in menuconfig. */
#if CONFIG_STATION_MODE
#define ESPNOW_WIFI_MODE WIFI_MODE_STA
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_STA
#else
#define ESPNOW_WIFI_MODE WIFI_MODE_AP
#define ESPNOW_WIFI_IF   ESP_IF_WIFI_AP
#endif

#define ESPNOW_QUEUE_SIZE           6

static uint8_t espnow_broadcast_mac[ESP_NOW_ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
#define IS_BROADCAST_ADDR(addr) (memcmp(addr, espnow_broadcast_mac, ESP_NOW_ETH_ALEN) == 0)


typedef enum {
    ESPNOW_SEND_CB,
    ESPNOW_RECV_CB,
} espnow_event_id_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    esp_now_send_status_t status;
} espnow_event_send_cb_t;

typedef struct {
    uint8_t mac_addr[ESP_NOW_ETH_ALEN];
    uint8_t *data;
    int data_len;
} espnow_packet_t;

typedef union {
    espnow_event_send_cb_t send_cb;
    espnow_packet_t recv_cb;
} espnow_event_info_t;

/* When ESPNOW sending or receiving callback function is called, post event to ESPNOW task. */
typedef struct {
    espnow_event_id_t id;
    espnow_event_info_t info;
} espnow_event_t;

enum {
    ESPNOW_DATA_BROADCAST,
    ESPNOW_DATA_UNICAST,
    ESPNOW_DATA_MAX,
};


static uint8_t last_mac[ESP_NOW_ETH_ALEN];
static espnow_packet_t *received = NULL;
static xQueueHandle espnow_queue = NULL;


static esp_err_t espnow_event_handler(void *ctx, system_event_t *event) {
    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            ESP_LOGI(TAG, "WiFi started");
            break;
        default:
            break;
    }
    return ESP_OK;
}


/* WiFi should start before using ESPNOW */
static void espnow_wifi_init(void) {
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(espnow_event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(ESPNOW_WIFI_MODE));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* In order to simplify example, channel is set after WiFi started.
     * This is not necessary in real application if the two devices have
     * been already on the same channel.
     */
    ESP_ERROR_CHECK(esp_wifi_set_channel(CONFIG_ESPNOW_CHANNEL, WIFI_SECOND_CHAN_NONE));
}

static void espnow_send_cb(const uint8_t *mac_addr, esp_now_send_status_t status) {
    espnow_event_t evt;
    espnow_event_send_cb_t *send_cb = &evt.info.send_cb;

    if (mac_addr == NULL) {
        ESP_LOGE(TAG, "Send cb arg error");
        return;
    }

    evt.id = ESPNOW_SEND_CB;
    memcpy(send_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    send_cb->status = status;
    if (xQueueSend(espnow_queue, &evt, portMAX_DELAY) != pdTRUE) {
        ESP_LOGW(TAG, "Send send queue fail");
    }
}

static void espnow_recv_cb(const uint8_t *mac_addr, const uint8_t *data, int len) {
    espnow_event_t evt;
    espnow_packet_t *recv_cb = &evt.info.recv_cb;

    if (mac_addr == NULL || data == NULL || len <= 0) {
        ESP_LOGE(TAG, "Receive cb arg error");
        return;
    }

    evt.id = ESPNOW_RECV_CB;
    memcpy(recv_cb->mac_addr, mac_addr, ESP_NOW_ETH_ALEN);
    recv_cb->data = (uint8_t *) malloc(len);
    if (recv_cb->data == NULL) {
        ESP_LOGE(TAG, "Malloc receive data fail");
        return;
    }
    memcpy(recv_cb->data, data, len);
    recv_cb->data_len = len;
    if (xQueueSend(espnow_queue, &evt, portMAX_DELAY) != pdTRUE) {
        ESP_LOGW(TAG, "Send receive queue fail");
        free(recv_cb->data);
    }
}

static esp_err_t add_peer(uint8_t mac_addr[ESP_NOW_ETH_ALEN]) {

    /* Add broadcast peer information to peer list. */
    esp_now_peer_info_t *peer = (esp_now_peer_info_t *) malloc(sizeof(esp_now_peer_info_t));
    if (peer == NULL) {
        ESP_LOGE(TAG, "Malloc peer information fail");
        vSemaphoreDelete(espnow_queue);
        esp_now_deinit();
        return ESP_FAIL;
    }
    memset(peer, 0, sizeof(esp_now_peer_info_t));
    peer->channel = CONFIG_ESPNOW_CHANNEL;
    peer->ifidx = ESPNOW_WIFI_IF;
    peer->encrypt = false;
    memcpy(peer->peer_addr, mac_addr, ESP_NOW_ETH_ALEN);
    ESP_ERROR_CHECK(esp_now_add_peer(peer));
    free(peer);
    return ESP_OK;

}


/* task that handles callbacks */
static void espnow_task(void *pvParameter) {
    espnow_event_t evt;
//    ENHelper *me = (ENHelper *) pvParameter;

    vTaskDelay(1000 / portTICK_RATE_MS);

    while (xQueueReceive(espnow_queue, &evt, portMAX_DELAY) == pdTRUE) {
        switch (evt.id) {
            case ESPNOW_SEND_CB: {


                espnow_event_send_cb_t *send_cb = &evt.info.send_cb;
//                    is_broadcast = IS_BROADCAST_ADDR(send_cb->mac_addr);

//                    ESP_LOGD(TAG, "Send data to "
//                    MACSTR
//                    ", status1: %d", MAC2STR(send_cb->mac_addr), send_cb->status);


                /* Delay a while before sending the next data. */
                vTaskDelay(50 / portTICK_RATE_MS);
                /* remove packet from send list */

                break;
            }
            case ESPNOW_RECV_CB: {
                espnow_packet_t *recv_cb = &evt.info.recv_cb;

                while (received != NULL) {
                    //wait until the last packet has been received by PJON
                    vTaskDelay(10 / portTICK_RATE_MS);
                }

                //Update last mac received from
                memcpy(last_mac, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
                received = recv_cb;


//                    if (ret == ESPNOW_DATA_BROADCAST) {
//                        ESP_LOGI(TAG, "Receive %dth broadcast data from: "
//                        MACSTR
//                        ", len: %d", recv_seq, MAC2STR(recv_cb->mac_addr), recv_cb->data_len);

                /* If MAC address does not exist in peer list, add it to peer list. */
//                        if (esp_now_is_peer_exist(recv_cb->mac_addr) == false) {
//                            esp_now_peer_info_t *peer = (esp_now_peer_info_t*) malloc(sizeof(esp_now_peer_info_t));
//                            if (peer == NULL) {
//                                ESP_LOGE(TAG, "Malloc peer information fail");
//                                espnow_deinit(send_param);
//                                vTaskDelete(NULL);
//                            }
//                            memset(peer, 0, sizeof(esp_now_peer_info_t));
//                            peer->channel = CONFIG_ESPNOW_CHANNEL;
//                            peer->ifidx = ESPNOW_WIFI_IF;
//                            peer->encrypt = true;
//                            memcpy(peer->lmk, CONFIG_ESPNOW_LMK, ESP_NOW_KEY_LEN);
//                            memcpy(peer->peer_addr, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
//                            ESP_ERROR_CHECK(esp_now_add_peer(peer));
//                            free(peer);
//                        }

//                        /* Indicates that the device has received broadcast ESPNOW data. */
//                        if (send_param->state == 0) {
//                            send_param->state = 1;
//                        }
//
//                        /* If receive broadcast ESPNOW data which indicates that the other device has received
//                         * broadcast ESPNOW data and the local magic number is bigger than that in the received
//                         * broadcast ESPNOW data, stop sending broadcast ESPNOW data and start sending unicast
//                         * ESPNOW data.
//                         */
//                        if (recv_state == 1) {
//                            /* The device which has the bigger magic number sends ESPNOW data, the other one
//                             * receives ESPNOW data.
//                             */
//                            if (send_param->unicast == false && send_param->magic >= recv_magic) {
//                                ESP_LOGI(TAG, "Start sending unicast data");
//                                ESP_LOGI(TAG, "send data to "
//                                MACSTR
//                                "", MAC2STR(recv_cb->mac_addr));
//
//                                /* Start sending unicast ESPNOW data. */
//                                memcpy(send_param->dest_mac, recv_cb->mac_addr, ESP_NOW_ETH_ALEN);
//                                espnow_data_prepare(send_param);
//                                if (esp_now_send(send_param->dest_mac, send_param->buffer, send_param->len) != ESP_OK) {
//                                    ESP_LOGE(TAG, "Send error");
//                                    espnow_deinit(send_param);
//                                    vTaskDelete(NULL);
//                                } else {
//                                    send_param->broadcast = false;
//                                    send_param->unicast = true;
//                                }
//                            }
//                        }
//                    } else if (ret == ESPNOW_DATA_UNICAST) {
//                        ESP_LOGI(TAG, "Receive %dth unicast data from: "
//                        MACSTR
//                        ", len: %d", recv_seq, MAC2STR(recv_cb->mac_addr), recv_cb->data_len);
//
//                        /* If receive unicast ESPNOW data, also stop sending broadcast ESPNOW data. */
//                        send_param->broadcast = false;
//                    } else {
//                        ESP_LOGI(TAG, "Receive error data from: "
//                        MACSTR
//                        "", MAC2STR(recv_cb->mac_addr));
//                    }
//                    break;
            }
            default:
                ESP_LOGE(TAG, "Callback type error: %d", evt.id);
                break;
        }
    }
}

static esp_err_t espnow_init() {

    if (espnow_queue != NULL) {
        return ESP_FAIL;
    }

    espnow_queue = xQueueCreate(ESPNOW_QUEUE_SIZE, sizeof(espnow_event_t));
    if (espnow_queue == NULL) {
        ESP_LOGE(TAG, "Create mutex fail");
        return ESP_FAIL;
    }

    /* Initialize ESPNOW and register sending and receiving callback function. */
    ESP_ERROR_CHECK(esp_now_init());
    ESP_ERROR_CHECK(esp_now_register_send_cb(espnow_send_cb));
    ESP_ERROR_CHECK(esp_now_register_recv_cb(espnow_recv_cb));

    /* Set primary master key. */
    ESP_ERROR_CHECK(esp_now_set_pmk((uint8_t *) CONFIG_ESPNOW_PMK));

    /* Add broadcast peer information to peer list. */
    add_peer(espnow_broadcast_mac);

    /* create the task */
    xTaskCreate(espnow_task, "espnow_task", 2048, NULL, 4, NULL);

    return ESP_OK;
}


static void espnow_deinit(espnow_packet_t *send_param) {
    free(send_param->data);
    free(send_param);
    vSemaphoreDelete(espnow_queue);
    esp_now_deinit();
}

static void clear_received() {
    free(received->data);
    free(received);
    received = NULL;
}



class ENHelper {

    uint32_t _magic_header;

/* ESPNOW sending or receiving callback function is called in WiFi task.
 * Users should not do lengthy operations from this task. Instead, post
 * necessary data to a queue and handle it from a lower priority task. */


public:


    bool begin() {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);

        espnow_wifi_init();
        espnow_init();
        return true;
    }

    uint16_t receive_string(uint8_t *string, uint16_t max_length) {

        // see if there's any received data waiting

        if (received == NULL) {
            return PJON_FAIL; //no data waiting
        }

        if (received->data_len > 0) {
            uint32_t header = 0;
            uint16_t len;
            memcpy((uint8_t * )(&header), received->data, 4);

            if (header != _magic_header) {
                clear_received();
                return PJON_FAIL; // Not an expected packet
            }
            if (received->data_len > 4 + max_length) {
                //packet bigger than buffer
                clear_received();
                return PJON_FAIL;
            }

            len = received->data_len - 4;

            memcpy(string, received->data + 4, len);
            clear_received();
            return len;
        }
        clear_received();
        return PJON_FAIL;
    }

    void send_string(uint8_t *string, uint16_t length, uint8_t dest_mac[ESP_NOW_ETH_ALEN]) {
        if (esp_now_send(dest_mac, string, length) != ESP_OK) {
            ESP_LOGE(TAG, "Send error");
        }
    }

    void send_response(uint8_t response) {
        send_string(&response, 1, last_mac);
    }

    void send_string(uint8_t *string, uint16_t length) {
        // Broadcast
        send_string(string, length, espnow_broadcast_mac);
    }

    void set_magic_header(uint32_t magic_header) { _magic_header = magic_header; }

    void get_sender(uint8_t *ip) {
        memcpy(ip, last_mac, ESP_NOW_ETH_ALEN);
    }
};

