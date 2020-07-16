
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| 12.1
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2020 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

PJONLocal implements a subset of the PJON protocol's features. It does support
only local mode and does not support packet queueing, infact it can transmit
only one packet at a time without any buffering, although it conserves
interoperability with other classes.

This class has been developed to enable PJON networking on very limited
microcontrollers, like ATtiny45, ATtiny84 and ATtiny85, where even 1kB
of program memory and 100B of ram make a difference.

  For examples see examples/ARDUINO/Local/SoftwareBitBang/PJONLocal/

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
  DetAtHome, Michael Branson, chestwood96, Mattze96 and Steven Bense,
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

Copyright 2010-2020 by Giovanni Blu Mitolo gioscarab@gmail.com

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
#define PJON_LOCAL
#include "interfaces/PJON_Interfaces.h"
#include "PJONDefines.h"

template<typename Strategy>
class PJONLocal {
  public:
    Strategy strategy;
    uint8_t config = PJON_TX_INFO_BIT | PJON_ACK_REQ_BIT;

    #if(PJON_INCLUDE_PACKET_ID)
      PJON_Packet_Record recent_packet_ids[PJON_MAX_RECENT_PACKET_IDS];
    #endif

    #if(PJON_INCLUDE_PORT)
      uint16_t port = PJON_BROADCAST;
    #endif

    /* PJONLocal initialization with no parameters:
       PJONLocal<SoftwareBitBang> bus; */

    PJONLocal() : strategy(Strategy()) {
      _device_id = PJON_NOT_ASSIGNED;
      set_default();
    };

    /* PJONLocal initialization passing device id:
       PJONLocal<SoftwareBitBang> bus(1); */

    PJONLocal(uint8_t device_id) : strategy(Strategy()) {
      _device_id = device_id;
      set_default();
    };

    /* Begin function to be called after initialization: */

    void begin() {
      strategy.begin(_device_id);
      #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
        _packet_id_seed = PJON_RANDOM(65535) + _device_id;
      #endif
    };

    /* Compose packet in PJON format: */

    uint16_t compose_packet(
      const uint8_t id,
      uint8_t *destination,
      const void *source,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      PJON_Packet_Info info;
      info.rx.id = id;
      info.tx.id = _device_id;
      info.header = (header == PJON_NO_HEADER) ? config : header;
      #if(PJON_INCLUDE_PACKET_ID)
        if(!packet_id && (info.header & PJON_PACKET_ID_BIT))
          info.id = PJONTools::new_packet_id(_packet_id_seed++);
        else info.id = packet_id;
      #else
        (void)packet_id;
      #endif
      #if(PJON_INCLUDE_PORT)
        info.port = (rx_port == PJON_BROADCAST) ? port : rx_port;
      #else
        (void)rx_port;
      #endif
      uint16_t l =
        PJONTools::compose_packet(info, destination, source, length);
      return l;
    };

    /* Get device id: */

    uint8_t device_id() const { return _device_id; };

    /* Returns a pointer to the start of the payload: */

    uint8_t *get_payload(uint8_t *buffer) {
      return buffer + (
        PJONTools::packet_overhead(buffer[1]) -
        PJONTools::crc_overhead(buffer[1])
      );
    }

    /* Calculate packet overhead: */

    uint8_t packet_overhead(uint8_t header = 0) const {
      return PJONTools::packet_overhead(header);
    };

    /* Fill a PJON_Packet_Info struct with data parsing a packet: */

    void parse(const uint8_t *packet, PJON_Packet_Info &packet_info) const {
      PJONTools::parse_header(packet, packet_info);
    };

    /* Try to receive data: */

    uint16_t receive(uint8_t *buffer, PJON_Packet_Info info) {
      uint16_t length = PJON_PACKET_MAX_LENGTH;
      uint16_t batch_length = 0;
      uint8_t overhead = 0;
      bool extended_length = false;
      for(uint16_t i = 0; i < length; i++) {
        if(!batch_length) {
          batch_length = strategy.receive_frame(buffer + i, length - i);
          if(batch_length == PJON_FAIL || batch_length == 0) return 0;
        }
        batch_length--;

        if(!i && !_router)
          if((buffer[i] != _device_id) && (buffer[i] != PJON_BROADCAST))
            return 0;

        if(i == 1) {
          if(
            (buffer[1] & PJON_MODE_BIT) ||
            (buffer[1] & PJON_MAC_BIT)  || (
              (buffer[0] == PJON_BROADCAST) && (buffer[1] & PJON_ACK_REQ_BIT)
            ) || (
              (buffer[1] & PJON_EXT_LEN_BIT) && !(buffer[1] & PJON_CRC_BIT)
            ) || (
              !PJON_INCLUDE_PACKET_ID && (buffer[1] & PJON_PACKET_ID_BIT)
            )
          ) return 0;
          extended_length = buffer[i] & PJON_EXT_LEN_BIT;
          overhead = packet_overhead(buffer[i]);
        }

        if((i == 2) && !extended_length) {
          length = buffer[i];
          if((length < overhead) || (length >= PJON_PACKET_MAX_LENGTH))
            return 0;
          if((length > 15) && !(buffer[1] & PJON_CRC_BIT)) return PJON_BUSY;
        }

        if((i == 3) && extended_length) {
          length = (buffer[i - 1] << 8) | (buffer[i] & 0xFF);
          if((length < overhead) || (length >= PJON_PACKET_MAX_LENGTH))
            return 0;
          if((length > 15) && !(buffer[1] & PJON_CRC_BIT)) return PJON_BUSY;
        }
      }

      if(
        PJON_crc8::compute(buffer, 3 + extended_length) !=
        buffer[3 + extended_length]
      ) return 0;

      if(buffer[1] & PJON_CRC_BIT) {
        if(
          !PJON_crc32::compare(
            PJON_crc32::compute(buffer, length - 4), buffer + (length - 4)
          )
        ) return 0;
      } else if(PJON_crc8::compute(buffer, length - 1) != buffer[length - 1])
        return 0;
      if(buffer[1] & PJON_ACK_REQ_BIT && buffer[0] != PJON_BROADCAST)
        if((_mode != PJON_SIMPLEX) && !_router)
          strategy.send_response(PJON_ACK);
      parse(buffer, info);
      #if(PJON_INCLUDE_PACKET_ID)
        if(
          (info.header & PJON_PACKET_ID_BIT) &&
          known_packet_id(info) && !_router
        ) return 0;
      #endif
      #if(PJON_INCLUDE_PORT)
        if((port != PJON_BROADCAST) && (port != info.port)) return 0;
      #endif
      return length - overhead;
    };

    /* Check if ready to send a packet: */

    bool ready_to_send() {
      if(
        (
          !_retries || (
            (uint32_t)(PJON_MICROS() - _last_send) >
            (uint32_t)(strategy.back_off(_retries))
          )
        )
      ) return strategy.can_start();
      return false;
    };

    /* Transmit an already composed packet:  */

    uint16_t send_packet(const uint8_t *payload, uint16_t length) {
      _last_send = PJON_MICROS();
      strategy.send_frame((uint8_t *)payload, length);
      if(
        payload[0] == PJON_BROADCAST || !(payload[1] & PJON_ACK_REQ_BIT) ||
        _mode == PJON_SIMPLEX
      ) {
        _retries = 0;
        return PJON_ACK;
      }
      uint16_t response = strategy.receive_response();
      if(response == PJON_ACK) {
        _retries = 0;
        return response;
      }
      if(_retries < strategy.get_max_attempts()) _retries++;
      else _retries = 0;
      if(response == PJON_FAIL) return response;
      else return PJON_BUSY;
    };

    /* Compose and transmit a packet passing its info as parameters: */

    uint16_t send_packet(
      uint8_t id,
      uint8_t *buffer,
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      if(!(length = compose_packet(
        id, buffer, payload, length, header, packet_id, rx_port
      ))) return PJON_FAIL;
      return send_packet(buffer, length);
    };

    /* In router mode, the receiver function can acknowledge
       for selected receiver device ids for which the route is known */

    void send_acknowledge() { strategy.send_response(PJON_ACK); };

    /* Set the config bit state: */

    void set_config_bit(bool new_state, uint8_t bit) {
      if(new_state) config |= bit;
      else config &= ~bit;
    };

    /* Configure synchronous acknowledge presence:
       TRUE: Send 8bits synchronous acknowledge when a packet is received
       FALSE: Avoid acknowledge transmission */

    void set_acknowledge(bool state) {
      set_config_bit(state, PJON_ACK_REQ_BIT);
    };

    /* Configure CRC selected for packet checking:
       TRUE: CRC32
       FALSE: CRC8 */

    void set_crc_32(bool state) { set_config_bit(state, PJON_CRC_BIT); };

    /* Set communication mode:
       Passing PJON_SIMPLEX communication is mono-directional
       Padding PJON_HALF_DUPLEX communication is bi-directional */

    void set_communication_mode(uint8_t mode) { _mode = mode; };

    /* Set default configuration: */

    void set_default() { _mode = PJON_HALF_DUPLEX; };

    /* Set the device id passing a single byte (watch out to id collision): */

    void set_id(uint8_t id) { _device_id = id; };

    /* Configure sender's information inclusion in the packet.
       TRUE: sender's device id (+8bits overhead)
       FALSE: No sender's device id inclusion (-8bits overhead)

       If you don't need the sender info disable the inclusion to reduce
       overhead and obtain higher communication speed. */

    void include_sender_info(bool state) {
      set_config_bit(state, PJON_TX_INFO_BIT);
    };

    /* Configure if device acts as a router:
       TRUE: device receives messages only for its bus and device id
       FALSE: receiver function is always called if data is received */

    void set_router(bool state) { _router = state; };

    #if(PJON_INCLUDE_PACKET_ID)

      /* Check if the packet id and its transmitter info are already present in
      buffer of recently received packets, if not add it to the buffer. */

      bool known_packet_id(PJON_Packet_Info info) {
        for(uint8_t i = 0; i < PJON_MAX_RECENT_PACKET_IDS; i++)
          if(
            info.id == recent_packet_ids[i].id &&
            info.sender_id == recent_packet_ids[i].sender_id
          ) return true;
        save_packet_id(info);
        return false;
      };

      /* Save packet id in the buffer: */

      void save_packet_id(PJON_Packet_Info info) {
        for(uint8_t i = PJON_MAX_RECENT_PACKET_IDS - 1; i > 0; i--)
          recent_packet_ids[i] = recent_packet_ids[i - 1];
        recent_packet_ids[0].id = info.id;
        recent_packet_ids[0].header = info.header;
        recent_packet_ids[0].sender_id = info.sender_id;
      };

      /* Configure packet id presence:
         TRUE: include packet id, FALSE: Avoid packet id inclusion */

      void set_packet_id(bool state) {
        set_config_bit(state, PJON_PACKET_ID_BIT);
      };

    #endif

    #if(PJON_INCLUDE_PORT)

    /* Include the port:
       p = 1-65535 -> Include 16 bits port id
       p = 0       -> Avoid port id inclusion */

      void include_port(uint16_t p) {
        set_config_bit((p != 0) ? 1 : 0, PJON_PORT_BIT);
        port = p;
      };

    #endif


  private:
    uint32_t      _last_send = 0;
    uint8_t       _mode;
    uint16_t      _packet_id_seed = 0;
    bool          _router = false;
    uint8_t       _retries = 0;
  protected:
    uint8_t       _device_id;
};
