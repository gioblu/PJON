
 /*-O//\             __     __
   |-gfo\           |__| | |  | |\ | ™
   |!y°o:\          |  __| |__| | \| v6.2
   |y"s§+`\         multi-master, multi-media communications bus system framework
  /so+:-..`\        Copyright 2010-2017 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-___________________________________________________________________

PJON™ is a self-funded, no-profit project created and mantained by Giovanni Blu Mitolo
with the support ot the internet community if you want to see the PJON project growing
with a faster pace, consider a donation at the following link: https://www.paypal.me/PJON

For the PJON™ Protocol specification see the specification directory.

PJON™ Standard compliant tools:
- https://github.com/aperepel/saleae-pjon-protocol-analyzer Logic analyzer by Andrew Grande
- https://github.com/Girgitt/PJON-python PJON running on Python by Zbigniew Zasieczny
- https://github.com/fredilarsen/ModuleInterface Easy config and value sync between IOT modules
 ______________________________________________________________________________

Copyright 2012-2017 by Giovanni Blu Mitolo gioscarab@gmail.com

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */

#ifndef PJONDefines_h
  #define PJONDefines_h
  #include "utils/error.h"
  #include "utils/CRC8.h"
  #include "utils/CRC32.h"

  /* Id used for broadcasting to all devices */
  #ifndef BROADCAST
    #define BROADCAST      0
  #endif

  /* Master device id */
  #define MASTER_ID      254

  /* Device id of still unindexed devices */
  #ifndef NOT_ASSIGNED
    #define NOT_ASSIGNED 255
  #endif

  /* Maximum devices handled by master */
  #define MAX_DEVICES     25

  /* Communication modes */
  #define SIMPLEX        150
  #define HALF_DUPLEX    151

  /* Protocol symbols */
  #define ACK              6
  #define BUSY           666
  #define NAK             21

  /* Dynamic addressing */
  #define ID_ACQUIRE     199
  #define ID_REQUEST     200
  #define ID_CONFIRM     201
  #define ID_NEGATE      203
  #define ID_LIST        204
  #define ID_REFRESH     205

  /* INTERNAL CONSTANTS */
  #define FAIL         65535
  #define TO_BE_SENT      74

  /* HEADER BITS DEFINITION: */
  #define MODE_BIT          B00000001 // 1 - Shared | 0 - Local
  #define SENDER_INFO_BIT   B00000010 // 1 - Sender device id + Sender bus id if shared | 0 - No info inclusion
  #define ACK_REQUEST_BIT   B00000100 // 1 - Request acknowledge | 0 - Do not request acknowledge
  #define ACK_MODE_BIT      B00001000 // 1 - Asynchronous acknowledge | 0 - Synchronous acknowledge
  #define ADDRESS_BIT       B00010000 // 1 - Addressing related | 0 - Not addressing related
  #define CRC_BIT           B00100000 // 1 - CRC32 | 0 - CRC8
  #define EXTEND_LENGTH_BIT B01000000 // 1 - 2 bytes length | 0 - 1 byte length
  #define EXTEND_HEADER_BIT B10000000 // 1 - 2 bytes header | 0 - 1 byte header
  /* EXTENDED HEADER BITS DEFINITION: */
  #define ROUTING_BIT       0B0100000000000000 // 1 - Routing request 0 - No routing requested
  #define SEGMENTATION_BIT  0B0010000000000000 // 1 - Segmentated | 0 - Not segmented
  #define SESSION_BIT       0B0001000000000000 // 1 - Session | 0 - Not including Session
  #define PARITY_BIT        0B0000100000000000 // 1 - Parity redundant info | 0 - No parity included
  #define ENCODING_BIT      0B0000010000000000 // 1 - Encoding info | 0 - Not including encoding ingo
  #define DATA_COMP_BIT     0B0000001000000000 // 1 - Data compression | 0 - No data compression
  #define ENCRYPTION_BIT    0B0000000100000000 // 1 - Encrypted data | 0 - Not encrypted data

  /* ERRORS: */
  #define CONNECTION_LOST     101
  #define PACKETS_BUFFER_FULL 102
  #define CONTENT_TOO_LONG    104
  #define ID_ACQUISITION_FAIL 105
  #define DEVICES_BUFFER_FULL 254

  /* CONSTRAINTS:

  /* Packet buffer length, if full PACKETS_BUFFER_FULL error is thrown.
     The packet buffer is preallocated, so its length strongly affects
     memory consumption */
  #ifndef MAX_PACKETS
    #define MAX_PACKETS 5
  #endif

  /* Max packet length, higher if necessary.
     The max packet length defines the length of packets pre-allocated buffers
     so it strongly affects memory consumption */
  #ifndef PACKET_MAX_LENGTH
    #define PACKET_MAX_LENGTH 50
  #endif

  /* If set to true avoids async ack code memory allocation if not used
     (it saves around 1kB of memory) */
  #ifndef INCLUDE_ASYNC_ACK
    #define INCLUDE_ASYNC_ACK false
  #endif

  /* Maximum packet ids record kept in memory (to avoid duplicated exchanges) */
  #ifndef MAX_RECENT_PACKET_IDS
    #define MAX_RECENT_PACKET_IDS 10
  #endif

  /* If set to true ensures packet ordered sending */
  #ifndef ORDERED_SENDING
    #define ORDERED_SENDING false
  #endif

  /* Dynamic addressing timing constants:
     Maximum number of device id collisions during auto-addressing */
  #define MAX_ACQUIRE_ID_COLLISIONS      10
  /* Delay between device id acquisition and self request */
  #define ACQUIRE_ID_DELAY             1250
  /* Maximum id scan time (6 seconds) */
  #define ID_SCAN_TIME              6000000
  /* Master free id broadcast response interval (0.1 seconds) */
  #define ID_REQUEST_INTERVAL        100000
  /* Master ID_REQUEST and ID_NEGATE timeout */
  #define ADDRESSING_TIMEOUT        2900000
  /* Master reception time during LIST_ID request broadcast (20 milliseconds) */
  #define LIST_IDS_RECEPTION_TIME     20000

  struct PJON_Packet {
    uint8_t  attempts;
    char     content[PACKET_MAX_LENGTH];
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
    uint16_t header = 0;
    uint16_t id = 0;
    uint8_t receiver_id = 0;
    uint8_t receiver_bus_id[4];
    uint8_t sender_id = 0;
    uint8_t sender_bus_id[4];
  };

  typedef void (* PJON_Receiver)(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info);
  typedef void (* PJON_Error)(uint8_t code, uint8_t data);

  static void dummy_receiver_handler(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {};
  static void dummy_error_handler(uint8_t code, uint8_t data) {};

#endif
