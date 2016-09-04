
 /*-O//\             __     __
   |-gfo\           |__| | |  | |\ |
   |!y°o:\          |  __| |__| | \| v5.0 beta
   |y"s§+`\         multi-master, multi-media communications bus system framework
  /so+:-..`\        Copyright 2012-2016 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
  _____-| |-___________________________________________________________________

Credits to contributors:
- Fred Larsen (Systems engineering, header driven communication, debugging)
- Pantovich github user (update returning number of packets to be delivered)
- Adrian Sławiński (Fix to enable SimpleModbusMasterV2 compatibility)
- SticilFace github user (Teensy porting)
- Esben Soeltoft (Arduino Zero porting)
- Alex Grishin (ESP8266 porting)
- Andrew Grande (Testing, support, bugfix)
- Mauro Zancarlin (Systems engineering, testing, bugfix)
- Michael Teeww (Callback based reception, debugging)
- PaoloP74 github user (Library conversion to 1.x Arduino IDE)

Bug reports:
- Zbigniew Zasieczny (header reference inconsistency report)
- DanRoad reddit user (can_start ThroughHardwareSerial bugfix)
- Remo Kallio (Packet index 0 bugfix)
- Emanuele Iannone (Forcing SIMPLEX in OverSamplingSimplex)
- Christian Pointner (Fixed compiler warnings)
- Andrew Grande (ESP8266 example watchdog error bug fix)
- Fabian Gärtner (receive function and big packets bugfix)
- Mauro Mombelli (Code cleanup)
- Shachar Limor (Blink example pinMode bugfix)

PJON Standard compliant tools:
- https://github.com/aperepel/saleae-pjon-protocol-analyzer Logic analyzer by Andrew Grande
- https://github.com/Girgitt/PJON-python PJON running on Python by Zbigniew Zasieczny

PJON is a self-funded, no-profit project created and mantained by Giovanni Blu Mitolo,
with the support ot the internet community if you want to see the PJON project growing
with a faster pace, consider a donation at the following link: https://www.paypal.me/PJON
__________________________________________________________________________________________

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

#ifndef PJONBase_h
  #define PJONBase_h

  #include "utils/CRC8.h"

  /* Communication modes */
  #define SIMPLEX     150
  #define HALF_DUPLEX 151

  /* Protocol symbols */
  #define ACK         6
  #define BUSY        666
  #define NAK         21

  /* Reserved addresses */
  #ifndef BROADCAST
    #define BROADCAST 0
  #endif
  #ifndef NOT_ASSIGNED
    #define NOT_ASSIGNED 255
  #endif

  #if BROADCAST == NOT_ASSIGNED
    #error BROADCAST and NOT_ASSIGNED point the same address
  #endif

  /* Internal constants */
  #define FAIL       0x100
  #define TO_BE_SENT 74

  /* HEADER CONFIGURATION:
  Thanks to the header byte the transmitter is able to instruct
  the receiver to handle communication as requested. */

  /* Packet header bits */
  #define MODE_BIT        1 // 1 - Shared | 0 - Local
  #define SENDER_INFO_BIT 2 // 1 - Sender device id + Sender bus id if shared | 0 - No info inclusion
  #define ACK_REQUEST_BIT 4 // 1 - Request synchronous acknowledge | 0 - Do not request acknowledge

  /* [0, 1, 1]: Local bus  | Sender info included    | Acknowledge requested - DEFAULT
     [0, 0, 1]: Local bus  | No sender info included | Acknowledge requested
     [0, 0, 0]: Local bus  | No sender info included | No acknowledge
     [1, 0, 0]: Shared bus | No sender info included | No acknowledge
     [1, 1, 0]: Shared bus | Sender info included    | No acknowledge
     [1, 1, 1]: Shared bus | Sender info included    | Acknowledge requested  */

  /* Errors */
  #define CONNECTION_LOST     101
  #define PACKETS_BUFFER_FULL 102
  #define CONTENT_TOO_LONG    104

  /* Constraints:

  Max attempts before throwing CONNECTON_LOST error */
  #ifndef MAX_ATTEMPTS
    #define MAX_ATTEMPTS      125
  #endif

  /* Packet buffer length, if full PACKETS_BUFFER_FULL error is thrown.
     The packet buffer is preallocated, so its length strongly affects
     memory consumption */
  #ifndef MAX_PACKETS
    #define MAX_PACKETS       5
  #endif

  /* Max packet length, higher if necessary.
     The max packet length defines the length of packets pre-allocated buffers
     so it strongly affects memory consumption */
  #ifndef PACKET_MAX_LENGTH
    #define PACKET_MAX_LENGTH 50
  #endif

  /* Maximum random delay on startup in milliseconds */
  #define INITIAL_MAX_DELAY   1000
  /* Maximum randon delay on collision */
  #define COLLISION_MAX_DELAY 48
  /* Maximum id scan time (5 seconds) */
  #define MAX_ID_SCAN_TIME    5000000

  struct PJON_Packet {
    uint8_t  attempts;
    char     content[PACKET_MAX_LENGTH];
    uint8_t  length;
    uint32_t registration;
    uint16_t state;
    uint32_t timing;
  };

  /* Last received packet Metainfo */
  struct PacketInfo {
    uint8_t header = 0;
    uint8_t receiver_id = 0;
    uint8_t receiver_bus_id[4];
    uint8_t sender_id = 0;
    uint8_t sender_bus_id[4];
  };

  typedef void (* receiver)(uint8_t *payload, uint8_t length, const PacketInfo &packet_info);
  typedef void (* error)(uint8_t code, uint8_t data);

  static void dummy_receiver_handler(uint8_t *payload, uint8_t length, const PacketInfo &packet_info) {};
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
