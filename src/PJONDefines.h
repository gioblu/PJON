
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| 13.0
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2021 by Giovanni Blu Mitolo gioscarab@gmail.com
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
  Felix Barbalet, Oleh Halitskiy, fotosettore, fabpolli, Adrian Sławiński,
  Osman Selçuk Aktepe, Jorgen-VikingGod, drtrigon, Endre Karlson,
  Wilfried Klaas, budaics, ibantxo, gonnavis, maxidroms83, Evgeny Dontsov,
  zcattacz, Valerii Koval, Ivan Kravets, Esben Soeltoft, Alex Grishin,
  Andrew Grande, Michael Teeww, Paolo Paolucci, per1234, Santiago Castro,
  pacproduct, elusive-code, Emanuele Iannone, Christian Pointner,
  Fabian Gärtner, Mauro Mombelli, Remo Kallio, hyndruide, sigmaeo, filogranaf,
  Maximiliano Duarte, Viktor Szépe, Shachar Limor, Andrei Volkau, maniekq,
  DetAtHome, Michael Branson, chestwood96, Mattze96, Steven Bense,
  Jack Anderson, callalilychen and Julio Aguirre.

Compatible tools:

  - ModuleInterface - https://github.com/fredilarsen/ModuleInterface
  - PJON-cython     - https://github.com/xlfe/PJON-cython
  - PJON-piper      - https://github.com/Girgitt/PJON-piper
  - PJON-python     - https://github.com/Girgitt/PJON-python
  - PJON-gRPC       - https://github.com/Galitskiy/PJON-gRPC
 _____________________________________________________________________________

This software is experimental and it is distributed "AS IS" without any
warranty, use it at your own risk.

Copyright 2010-2021 by Giovanni Blu Mitolo gioscarab@gmail.com

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

/* Protocol symbols: */
#define PJON_ACK                      6
#define PJON_BUSY                   666
#define PJON_NAK                     21

/* Id used for broadcasting to all devices */
#ifndef PJON_BROADCAST
  #define PJON_BROADCAST              0
#endif

/* Device id of still unindexed devices */
#ifndef PJON_NOT_ASSIGNED
  #define PJON_NOT_ASSIGNED         255
#endif

/* Device id used by master */
#ifndef PJON_MASTER_ID
  #define PJON_MASTER_ID            254
#endif

/* Internal constants: */
#define PJON_FAIL                 65535
#define PJON_TO_BE_SENT              74

/* Communication modes: */
#define PJON_SIMPLEX              false
#define PJON_HALF_DUPLEX           true

/* Header bits definition: */

/* No header present (unacceptable value used)*/
#define PJON_NO_HEADER       0B01001000
/* 0 - Local network
   1 - Shared  network */
#define PJON_MODE_BIT        0B00000001
/* 0 - No info inclusion
   1 - Local:  Sender device id included
       Shared: Sender device id + Sender bus id */
#define PJON_TX_INFO_BIT     0B00000010
/* 0 - Synchronous acknowledgement disabled
   1 - Synchronous acknowledgement enabled */
#define PJON_ACK_REQ_BIT     0B00000100
/* 0 - MAC address inclusion disabled
   1 - MAC address inclusion enabled (2x 48 bits) */
#define PJON_MAC_BIT         0B00001000
/* 0 - No port id contained
   1 - Port id contained (2 bytes integer) */
#define PJON_PORT_BIT        0B00010000
/* 0 - CRC8 (1 byte) included at the end of the packet
   1 - CRC32 (4 bytes) included at the end of the packet */
#define PJON_CRC_BIT         0B00100000
/* 0 - 1 byte long (max 255 bytes)
   1 - 2 bytes long (max 65535 bytes) */
#define PJON_EXT_LEN_BIT     0B01000000
/* 0 - Packet id not present
   1 - Packet id present */
#define PJON_PACKET_ID_BIT   0B10000000

/* Errors: */

#define PJON_CONNECTION_LOST        101
#define PJON_PACKETS_BUFFER_FULL    102
#define PJON_CONTENT_TOO_LONG       104

/* Constraints: */

/* Maximum amount of routers a packet can pass before being discarded: */
#ifndef PJON_MAX_HOPS
  #define PJON_MAX_HOPS              15
#endif

/* Packet buffer length, if full PJON_PACKETS_BUFFER_FULL error is thrown.
   The packet buffer is preallocated, so its length strongly affects
   memory consumption */
#ifndef PJON_MAX_PACKETS
  #define PJON_MAX_PACKETS            5
#endif

/* Max packet length, higher if necessary.
   The max packet length defines the length of packets pre-allocated buffers
   so it strongly affects memory consumption */
#ifndef PJON_PACKET_MAX_LENGTH
  #define PJON_PACKET_MAX_LENGTH     50
#endif

/* Maximum packet ids record kept in memory (to avoid duplicated exchanges) */
#ifndef PJON_MAX_RECENT_PACKET_IDS
  #define PJON_MAX_RECENT_PACKET_IDS 10
#endif

/* Optional features: */

/* If defined includes the packet id feature */
#ifdef PJON_INCLUDE_PACKET_ID
  #undef PJON_INCLUDE_PACKET_ID
  #define PJON_INCLUDE_PACKET_ID   true
#else
  #define PJON_INCLUDE_PACKET_ID  false
#endif

/* If defined includes the port id feature */
#ifdef PJON_INCLUDE_PORT
  #undef PJON_INCLUDE_PORT
  #define PJON_INCLUDE_PORT        true
#else
  #define PJON_INCLUDE_PORT       false
#endif

/* If defined includes the mac address feature */
#ifdef PJON_INCLUDE_MAC
  #undef PJON_INCLUDE_MAC
  #define PJON_INCLUDE_MAC         true
#else
  #define PJON_INCLUDE_MAC        false
#endif

/* Data structures: */

struct PJON_Packet {
  uint8_t  attempts = 0;
  uint8_t  content[PJON_PACKET_MAX_LENGTH];
  uint16_t length;
  uint32_t registration;
  uint16_t state = 0;
  uint32_t timing = 0;
};

struct PJON_Packet_Record {
  uint8_t  header;
  uint8_t  sender_id;
  #ifndef PJON_LOCAL
    uint8_t  sender_bus_id[4];
  #endif
  #if(PJON_INCLUDE_PACKET_ID)
    uint16_t id;
  #endif
};

struct PJON_Endpoint {
  uint8_t id = PJON_NOT_ASSIGNED;
  #ifndef PJON_LOCAL
    uint8_t bus_id[4] = {0, 0, 0, 0};
  #endif
  #if(PJON_INCLUDE_MAC)
    uint8_t mac[6] = {0, 0, 0, 0, 0, 0};
  #endif
};

struct PJON_Packet_Info {
  PJON_Endpoint tx;
  PJON_Endpoint rx;
  uint8_t header = PJON_NO_HEADER;
  #ifndef PJON_LOCAL
    void *custom_pointer;
    uint8_t hops = 0;
  #endif
  #if(PJON_INCLUDE_PACKET_ID)
    uint16_t id = 0;
  #endif
  #if(PJON_INCLUDE_PORT)
    uint16_t port = PJON_BROADCAST;
  #endif
};

typedef void (* PJON_Receiver)(
  uint8_t *payload,
  uint16_t length,
  const PJON_Packet_Info &packet_info
);

typedef void (* PJON_Error)(
  uint8_t code,
  uint16_t data,
  void *custom_pointer
);

/* PJON general purpose functions: */

struct PJONTools {
  /* Bus id used as localhost (used by shared mode broadcast and NAT) */

  static const uint8_t* localhost() {
    static const uint8_t lh[4] = {0, 0, 0, 0};
    return lh;
  };

  /* Unused MAC address value */

  static const uint8_t* no_mac() {
    static const uint8_t lh[6] = {0, 0, 0, 0, 0, 0};
    return lh;
  };

  /* Calculates the packet's overhead using the header: */

  static uint8_t packet_overhead(uint8_t header) {
    return (
      (
        (header & PJON_MODE_BIT) ?
          (header & PJON_TX_INFO_BIT   ? 11 : 6) :
          (header & PJON_TX_INFO_BIT   ?  2 : 1)
      ) + (header & PJON_EXT_LEN_BIT   ?  2 : 1)
        + (header & PJON_CRC_BIT       ?  4 : 1)
        + (header & PJON_PORT_BIT      ?  2 : 0)
        + (header & PJON_PACKET_ID_BIT ?  2 : 0)
        + (header & PJON_MAC_BIT       ? 12 : 0)
        + 2 // header + header's CRC
    );
  };

  /* Calculates the packet's CRC overhead using the header: */

  static uint8_t crc_overhead(uint8_t header) {
    return (header & PJON_CRC_BIT) ? 4 : 1;
  };

  /* Generates a new unique packet id: */

  static uint16_t new_packet_id(uint16_t seed) {
    if(!(++seed)) seed = 1;
    return seed;
  };

  /* Copy an id: */

  static void copy_id(uint8_t dest[], const uint8_t src[], uint8_t length) {
    memcpy(dest, src, length);
  };

  /* Check equality between two ids: */

  static bool id_equality(
    const uint8_t *n_one,
    const uint8_t *n_two,
    uint8_t length
  ) {
    for(uint8_t i = 0; i < length; i++)
      if(n_one[i] != n_two[i])
        return false;
    return true;
  };

  /* Composes a packet in PJON format: */

  static uint16_t compose_packet(
    PJON_Packet_Info info,
    uint8_t *destination,
    const void *source,
    uint16_t length
  ) {
    uint8_t index = 0;
    if(length > 255) info.header |= PJON_EXT_LEN_BIT;
    #if(PJON_INCLUDE_PORT)
      if(info.port != PJON_BROADCAST) info.header |= PJON_PORT_BIT;
      if((info.header & PJON_PORT_BIT) && (info.port == PJON_BROADCAST))
        info.header &= ~PJON_PORT_BIT;
    #endif
    if(info.rx.id == PJON_BROADCAST) info.header &= ~(PJON_ACK_REQ_BIT);
    uint16_t new_length = length + packet_overhead(info.header);
    bool extended_length = info.header & PJON_EXT_LEN_BIT;
    if(new_length > 15 && !(info.header & PJON_CRC_BIT)) {
      info.header |= PJON_CRC_BIT;
      new_length = (uint16_t)(length + packet_overhead(info.header));
    }
    if(new_length > 255 && !extended_length) {
      info.header |= PJON_EXT_LEN_BIT;
      new_length = (uint16_t)(length + packet_overhead(info.header));
    }
    if(new_length >= PJON_PACKET_MAX_LENGTH) return new_length;
    destination[index++] = info.rx.id;
    destination[index++] = (uint8_t)info.header;
    if(extended_length) {
      destination[index++] = (uint8_t)(new_length >> 8);
      destination[index++] = (uint8_t)new_length;
      destination[index++] = PJON_crc8::compute((uint8_t *)destination, 4);
    } else {
      destination[index++] = (uint8_t)new_length;
      destination[index++] = PJON_crc8::compute((uint8_t *)destination, 3);
    }
    #ifndef PJON_LOCAL
      if(info.header & PJON_MODE_BIT) {
        copy_id((uint8_t*) &destination[index], info.rx.bus_id, 4);
        index += 4;
        if(info.header & PJON_TX_INFO_BIT) {
          copy_id((uint8_t*) &destination[index], info.tx.bus_id, 4);
          index += 4;
        }
        destination[index++] = info.hops;
      }
    #endif
    if(info.header & PJON_TX_INFO_BIT) destination[index++] = info.tx.id;
    #if(PJON_INCLUDE_PACKET_ID)
      if(info.header & PJON_PACKET_ID_BIT) {
        destination[index++] = (uint8_t)(info.id >> 8);
        destination[index++] = (uint8_t)info.id;
      }
    #endif
    #if(PJON_INCLUDE_PORT)
      if(info.header & PJON_PORT_BIT) {
        if(info.port != PJON_BROADCAST) {
          destination[index++] = (uint8_t)(info.port >> 8);
          destination[index++] = (uint8_t)info.port;
        }
      }
    #endif
    #if(PJON_INCLUDE_MAC)
      if(info.header & PJON_MAC_BIT) {
        copy_id(&destination[index], info.rx.mac, 6);
        index += 6;
        copy_id(&destination[index], info.tx.mac, 6);
        index += 6;
      }
    #endif
    memcpy(destination + index, source, length);
    if(info.header & PJON_CRC_BIT) {
      uint32_t computed_crc =
        PJON_crc32::compute((uint8_t *)destination, new_length - 4);
      destination[new_length - 4] =
        (uint8_t)((uint32_t)(computed_crc) >> 24);
      destination[new_length - 3] =
        (uint8_t)((uint32_t)(computed_crc) >> 16);
      destination[new_length - 2] =
        (uint8_t)((uint32_t)(computed_crc) >>  8);
      destination[new_length - 1] =
        (uint8_t)((uint32_t)computed_crc);
    } else destination[new_length - 1] =
      PJON_crc8::compute((uint8_t *)destination, new_length - 1);
    return new_length;
  };

  /* Fills a PJON_Packet_Info struct with data parsing a packet: */

  static void parse_header(const uint8_t *packet, PJON_Packet_Info &info) {
    uint8_t index = 0;

    // replace passed in info with a fresh instance
    // (initializing all values to their defaults)
    // This is important, as we conditionally parse some info fields (e.g. Port)
    // Those would then be left uninitialized or in an old state corresponding to
    // the previous packet:
    info = PJON_Packet_Info{};

    info.rx.id = packet[index++];
    bool extended_length = packet[index] & PJON_EXT_LEN_BIT;
    info.header = packet[index++];
    index += extended_length + 2; // + LENGTH + HEADER CRC
    #ifndef PJON_LOCAL
      if(info.header & PJON_MODE_BIT) {
        copy_id(info.rx.bus_id, packet + index, 4);
        index += 4;
        if(info.header & PJON_TX_INFO_BIT) {
          copy_id(info.tx.bus_id, packet + index, 4);
          index += 4;
        }
        info.hops = packet[index++];
      }
    #endif
    if(info.header & PJON_TX_INFO_BIT)
      info.tx.id = packet[index++];
    #if(PJON_INCLUDE_PACKET_ID)
      if(info.header & PJON_PACKET_ID_BIT) {
        info.id = (packet[index] << 8) | (packet[index + 1] & 0xFF);
        index += 2;
      }
    #endif
    #if(PJON_INCLUDE_PORT)
      if(info.header & PJON_PORT_BIT) {
        info.port = (packet[index] << 8) | (packet[index + 1] & 0xFF);
        index += 2;
      }
    #endif
    #if(PJON_INCLUDE_MAC)
      copy_id(info.rx.mac, packet + index, 6);
      index += 6;
      copy_id(info.tx.mac, packet + index, 6);
    #endif
  };
};
