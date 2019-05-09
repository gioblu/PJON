
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| v11.2
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2019 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

For the PJON® Protocol specification see the specification directory.

Thanks to the support, expertise, kindness and talent of the following
contributors, the protocol's documentation, specification and implementation
have been strongly tested, enhanced and verified:

  Fred Larsen, Zbigniew Zasieczny, Matheus Garbelini, sticilface,
  Felix Barbalet, Oleh Halitskiy, fabpolli, Adrian Sławiński,
  Osman Selçuk Aktepe, Jorgen-VikingGod, drtrigon, Endre Karlson,
  Wilfried Klaas, budaics, ibantxo, gonnavis, maxidroms83, Evgeny Dontsov,
  zcattacz, Valerii Koval, Ivan Kravets, Esben Soeltoft, Alex Grishin,
  Andrew Grande, Michael Teeww, Paolo Paolucci, per1234, Santiago Castro,
  pacproduct, elusive-code, Emanuele Iannone, Christian Pointner,
  Fabian Gärtner, Mauro Mombelli, Remo Kallio, hyndruide, sigmaeo, filogranaf,
  Maximiliano Duarte, Viktor Szépe, Shachar Limor, Andrei Volkau, maniekq,
  DetAtHome, Michael Branson and chestwood96.

Compatible tools:

  - ModuleInterface - https://github.com/fredilarsen/ModuleInterface
  - PJON-cython     - https://github.com/xlfe/PJON-cython
  - PJON-piper      - https://github.com/Girgitt/PJON-piper
  - PJON-python     - https://github.com/Girgitt/PJON-python
  - PJON-gRPC       - https://github.com/Galitskiy/PJON-gRPC
 _____________________________________________________________________________

This software is experimental and it is distributed "AS IS" without any
warranty, use it at your own risk.

Copyright 2010-2019 by Giovanni Blu Mitolo gioscarab@gmail.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#pragma once
#include "utils/crc/PJON_CRC8.h"
#include "utils/crc/PJON_CRC32.h"

/* Id used for broadcasting to all devices */
#ifndef PJON_BROADCAST
  #define PJON_BROADCAST        0
#endif

/* Master device id */
#ifndef PJON_MASTER_ID
  #define PJON_MASTER_ID      254
#endif

/* Device id of still unindexed devices */
#ifndef PJON_NOT_ASSIGNED
  #define PJON_NOT_ASSIGNED   255
#endif

/* Maximum devices handled by master */
#ifndef PJON_MAX_DEVICES
  #define PJON_MAX_DEVICES   25
#endif

/* Communication modes */
#define PJON_SIMPLEX        150
#define PJON_HALF_DUPLEX    151

/* Protocol symbols */
#define PJON_ACK              6
#define PJON_BUSY           666
#define PJON_NAK             21

/* Dynamic addressing */
#define PJON_ID_DISCOVERY   200
#define PJON_ID_REQUEST     201
#define PJON_ID_CONFIRM     202
#define PJON_ID_NEGATE      203

/* INTERNAL CONSTANTS */
#define PJON_FAIL         65535
#define PJON_TO_BE_SENT      74

/* No header present (unacceptable value used)*/
#define PJON_NO_HEADER      0B01001000
/* HEADER BITS DEFINITION: */
/* 0 - Local network
   1 - Shared  network */
#define PJON_MODE_BIT       0B00000001
/* 0 - No info inclusion
   1 - Local:  Sender device id included
       Shared: Sender device id + Sender bus id */
#define PJON_TX_INFO_BIT    0B00000010
/* 0 - Synchronous acknowledgement disabled
   1 - Synchronous acknowledgement enabled */
#define PJON_ACK_REQ_BIT    0B00000100
/* 0 - Asynchronous acknowledgement disabled
   1 - Asynchronous acknowledgement enabled */
#define PJON_ACK_MODE_BIT   0B00001000
/* 0 - No port id contained
   1 - Port id contained (2 bytes integer) */
#define PJON_PORT_BIT       0B00010000
/* 0 - CRC8 (1 byte) included at the end of the packet
   1 - CRC32 (4 bytes) included at the end of the packet */
#define PJON_CRC_BIT        0B00100000
/* 0 - 1 byte long (max 255 bytes)
   1 - 2 bytes long (max 65535 bytes) */
#define PJON_EXT_LEN_BIT    0B01000000
/* 0 - Packet id not present
   1 - Packet id present */
#define PJON_PACKET_ID_BIT  0B10000000

/* ERRORS: */
#define PJON_CONNECTION_LOST     101
#define PJON_PACKETS_BUFFER_FULL 102
#define PJON_CONTENT_TOO_LONG    104
#define PJON_ID_ACQUISITION_FAIL 105
#define PJON_DEVICES_BUFFER_FULL 254

/* CONSTRAINTS: */

/* Packet buffer length, if full PJON_PACKETS_BUFFER_FULL error is thrown.
   The packet buffer is preallocated, so its length strongly affects
   memory consumption */
#ifndef PJON_MAX_PACKETS
  #define PJON_MAX_PACKETS 5
#endif

/* Max packet length, higher if necessary.
   The max packet length defines the length of packets pre-allocated buffers
   so it strongly affects memory consumption */
#ifndef PJON_PACKET_MAX_LENGTH
  #define PJON_PACKET_MAX_LENGTH 50
#endif

/* If set to false async ack feature is not included saving memory
   (it saves around 1kB of memory) */
#ifndef PJON_INCLUDE_ASYNC_ACK
  #define PJON_INCLUDE_ASYNC_ACK false
#endif

/* If set to false packet id feature is not included saving memory
   (it saves around 1kB of memory) */
#ifndef PJON_INCLUDE_PACKET_ID
  #define PJON_INCLUDE_PACKET_ID false
#endif

/* Maximum packet ids record kept in memory (to avoid duplicated exchanges) */
#ifndef PJON_MAX_RECENT_PACKET_IDS
  #define PJON_MAX_RECENT_PACKET_IDS 10
#endif

/* Dynamic addressing port number */
#define PJON_DYNAMIC_ADDRESSING_PORT    1
/* Master ID_REQUEST and ID_NEGATE timeout (5 seconds) */
#define PJON_ADDRESSING_TIMEOUT   5000000
/* Master discovery broadcast interval (10 seconds) */
#define PJON_DISCOVERY_INTERVAL   1000000

struct PJON_Packet {
  uint8_t  attempts;
  uint8_t  content[PJON_PACKET_MAX_LENGTH];
  uint16_t length;
  uint32_t registration;
  uint16_t state;
  uint32_t timing;
};

struct PJON_Packet_Record {
  uint16_t id;
  uint8_t  header;
  uint8_t  sender_id;
  uint8_t  sender_bus_id[4];
};

/* Last received packet Metainfo */
struct PJON_Packet_Info {
  uint8_t header;
  uint16_t id;
  uint8_t receiver_id;
  uint8_t receiver_bus_id[4];
  uint8_t sender_id;
  uint8_t sender_bus_id[4];
  uint16_t port;
  void *custom_pointer;
};

typedef void (* PJON_Receiver)(
  uint8_t *payload,
  uint16_t length,
  const PJON_Packet_Info &packet_info
);

static void PJON_dummy_receiver_handler(
  uint8_t *,               // payload
  uint16_t,                // length
  const PJON_Packet_Info & // packet_info
) {};

typedef void (* PJON_Error)(
  uint8_t code,
  uint16_t data,
  void *custom_pointer
);

static void PJON_dummy_error_handler(
  uint8_t,  // code
  uint16_t, // data
  void *    // custom_pointer
) {};

struct PJONTools {
  /* Copy a bus id: */

  static void copy_bus_id(uint8_t dest[], const uint8_t src[]) {
    memcpy(dest, src, 4);
  };

  /* Check equality between two bus ids */

  static bool bus_id_equality(const uint8_t *n_one, const uint8_t *n_two) {
    for(uint8_t i = 0; i < 4; i++)
      if(n_one[i] != n_two[i])
        return false;
    return true;
  };

  /* Copy a device address: */

  static void copy_address(uint8_t dest[], const uint8_t src[]) {
    memcpy(dest, src, 5);
  };

  /* Check equality between two mac addresses */

  static bool address_equality(const uint8_t *n_one, const uint8_t *n_two) {
    for(uint8_t i = 0; i < 5; i++)
      if(n_one[i] != n_two[i])
        return false;
    return true;
  };

  /* Fill a PJON_Packet_Info struct with data parsing a packet: */

  static void parse_header(const uint8_t *packet, PJON_Packet_Info &info) {
    memset(&info, 0, sizeof info);
    uint8_t index = 0;
    info.receiver_id = packet[index++];
    bool extended_length = packet[index] & PJON_EXT_LEN_BIT;
    info.header = packet[index++];
    index += extended_length + 2; // + LENGTH + HEADER CRC
    if(info.header & PJON_MODE_BIT) {
      copy_bus_id(info.receiver_bus_id, packet + index);
      index += 4;
      if(info.header & PJON_TX_INFO_BIT) {
        copy_bus_id(info.sender_bus_id, packet + index);
        index += 4;
      }
    }
    if(info.header & PJON_TX_INFO_BIT)
      info.sender_id = packet[index++];
    #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
      if(((info.header & PJON_ACK_MODE_BIT) &&
          (info.header & PJON_TX_INFO_BIT)
        ) || info.header & PJON_PACKET_ID_BIT
      ) {
        info.id =
          (packet[index] << 8) | (packet[index + 1] & 0xFF);
        index += 2;
      }
    #endif
    if(info.header & PJON_PORT_BIT)
      info.port = (packet[index] << 8) | (packet[index + 1] & 0xFF);
  };
};
