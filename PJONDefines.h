
 /*-O//\             __     __
   |-gfo\           |__| | |  | |\ | ™
   |!y°o:\          |  __| |__| | \| v6.0
   |y"s§+`\         multi-master, multi-media communications bus system framework
  /so+:-..`\        Copyright 2010-2016 by Giovanni Blu Mitolo gioscarab@gmail.com
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

PJON™ Protocol specification:
- v0.1 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.1.md
- v0.2 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.2.md
- v0.3 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v0.3.md
- v1.0 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-specification-v1.0.md

PJON™ Acknowledge specification:
- v0.1 https://github.com/gioblu/PJON/blob/master/specification/PJON-protocol-acknowledge-specification-v0.1.md

PJON™ Dynamic addressing specification:
- v0.1 https://github.com/gioblu/PJON/blob/master/specification/PJON-dynamic-addressing-specification-v0.1.md

PJON™ Standard compliant tools:
- https://github.com/aperepel/saleae-pjon-protocol-analyzer Logic analyzer by Andrew Grande
- https://github.com/Girgitt/PJON-python PJON running on Python by Zbigniew Zasieczny
 ______________________________________________________________________________

Copyright 2012-2016 by Giovanni Blu Mitolo gioscarab@gmail.com

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
  #include "utils/CRC8.h"
  #include "utils/CRC32.h"

  /* Device id of the master */
  #define MASTER_ID   254
  #define MAX_DEVICES  25

  /* Communication modes */
  #define SIMPLEX     150
  #define HALF_DUPLEX 151

  /* Protocol symbols */
  #define ACK           6
  #define BUSY        666
  #define NAK          21

  #define ID_ACQUIRE  199
  #define ID_REQUEST  200
  #define ID_CONFIRM  201
  #define ID_NEGATE   203
  #define ID_LIST     204
  #define ID_REFRESH  205

  #ifndef BROADCAST
    #define BROADCAST   0
  #endif

  #ifndef NOT_ASSIGNED
    #define NOT_ASSIGNED 255
  #endif

  #if BROADCAST == NOT_ASSIGNED
    #error BROADCAST and NOT_ASSIGNED point the same address
  #endif

  /* INTERNAL CONSTANTS */
  #define FAIL       65535
  #define TO_BE_SENT 74

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
  Max attempts before throwing CONNECTON_LOST error */
  #ifndef MAX_ATTEMPTS
    #define MAX_ATTEMPTS       50
  #endif

  /* Max time delayed by backoff in microseconds  */
  #ifndef MAX_BACK_OFF
    #define MAX_BACK_OFF (uint32_t)MAX_ATTEMPTS * (uint32_t)MAX_ATTEMPTS * (uint32_t)MAX_ATTEMPTS
  #endif

  /* Packet buffer length, if full PACKETS_BUFFER_FULL error is thrown.
     The packet buffer is preallocated, so its length strongly affects
     memory consumption */
  #ifndef MAX_PACKETS
    #define MAX_PACKETS         5
  #endif

  /* Max packet length, higher if necessary.
     The max packet length defines the length of packets pre-allocated buffers
     so it strongly affects memory consumption */
  #ifndef PACKET_MAX_LENGTH
    #define PACKET_MAX_LENGTH  50
  #endif

  /* If set to true avoids async ack code memory allocation if not used
     (it saves around 1kB of memory) */
  #ifndef INCLUDE_ASYNC_ACK
    #define INCLUDE_ASYNC_ACK false
  #endif

  /* If set to true ensures packet ordered sending */
  #ifndef ORDERED_SENDING
    #define ORDERED_SENDING   false
  #endif

  /* TIMING:
     Maximum number of device id collisions during auto-addressing */
  #define MAX_ACQUIRE_ID_COLLISIONS      10
  /* Delay between device id acquisition and self request */
  #define ACQUIRE_ID_DELAY             1250
  /* Maximum random delay on startup in milliseconds */
  #define INITIAL_DELAY                1000
  /* Maximum randon delay on collision */
  #define COLLISION_DELAY                48
  /* Maximum id scan time (6 seconds) */
  #define ID_SCAN_TIME              6000000
  /* Master free id broadcast response interval (0.1 seconds) */
  #define ID_REQUEST_INTERVAL        100000
  /* Master ID_REQUEST and ID_NEGATE timeout */
  #define ADDRESSING_TIMEOUT        2900000
  /* Master reception time during LIST_ID request broadcast (2 milliseconds) */
  #define LIST_IDS_RECEPTION_TIME     20000
  /* Asynchronous acknowledge backoff offset: */
  #define ASYNC_BACKOFF_OFFSET       100000

  struct PJON_Packet {
    uint8_t  attempts;
    char     content[PACKET_MAX_LENGTH];
    uint16_t length;
    uint32_t registration;
    uint16_t state;
    uint32_t timing;
  };

  /* Last received packet Metainfo */
  struct PacketInfo {
    uint16_t header = 0;
    uint16_t id = 0;
    uint8_t receiver_id = 0;
    uint8_t receiver_bus_id[4];
    uint8_t sender_id = 0;
    uint8_t sender_bus_id[4];
  };

  typedef void (* receiver)(uint8_t *payload, uint16_t length, const PacketInfo &packet_info);
  typedef void (* error)(uint8_t code, uint8_t data);

  static void dummy_receiver_handler(uint8_t *payload, uint16_t length, const PacketInfo &packet_info) {};
  static void dummy_error_handler(uint8_t code, uint8_t data) {};

  /* Check equality between two bus ids */

  boolean bus_id_equality(const uint8_t *name_one, const uint8_t *name_two) {
    for(uint8_t i = 0; i < 4; i++)
      if(name_one[i] != name_two[i])
        return false;
    return true;
  };

  /* Copy a bus id: */

  void copy_bus_id(uint8_t dest[], const uint8_t src[]) { memcpy(dest, src, 4); };

#endif
