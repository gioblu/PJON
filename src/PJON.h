
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
  DetAtHome, Michael Branson, chestwood96, Mattze96, Steven Bense
  and Jack Anderson.

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
#include "interfaces/PJON_Interfaces.h"
#include "PJONDefines.h"
#include "strategies/PJON_Strategies.h"

static void PJON_dummy_receiver_handler(
  uint8_t *,               // payload
  uint16_t,                // length
  const PJON_Packet_Info & // packet_info
) {};

static void PJON_dummy_error_handler(
  uint8_t,  // code
  uint16_t, // data
  void *    // custom_pointer
) {};

template<typename Strategy>
class PJON {
  public:
    Strategy strategy;
    uint8_t config = PJON_TX_INFO_BIT | PJON_ACK_REQ_BIT;
    uint8_t bus_id[4] = {0, 0, 0, 0};
    uint8_t data[PJON_PACKET_MAX_LENGTH];
    PJON_Packet_Info last_packet_info;
    PJON_Packet packets[PJON_MAX_PACKETS];
    uint16_t port = PJON_BROADCAST;
    uint8_t random_seed = A0;

    #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
      PJON_Packet_Record recent_packet_ids[PJON_MAX_RECENT_PACKET_IDS];
    #endif

    /* PJON initialization with no parameters:
        State: Local (bus_id: 0.0.0.0)
        Synchronous acknowledge: true
        Asynchronous acknowledge: false
        device id: PJON_NOT_ASSIGNED (255)
        Mode: PJON_HALF_DUPLEX
        Sender info: true (Sender info are included in the packet)

       PJON<SoftwareBitBang> bus; */

    PJON() : strategy(Strategy()) {
      _device_id = PJON_NOT_ASSIGNED;
      set_default();
    };

    /* PJON initialization passing device id:
       PJON<SoftwareBitBang> bus(1); */

    PJON(uint8_t device_id) : strategy(Strategy()) {
      _device_id = device_id;
      set_default();
    };

    /* PJON initialization passing bus and device id:
       uint8_t my_bus = {1, 1, 1, 1};
       PJON<SoftwareBitBang> bus(my_bys, 1); */

    PJON(const uint8_t *b_id, uint8_t device_id) : strategy(Strategy()) {
      PJONTools::copy_bus_id(bus_id, b_id);
      _device_id = device_id;
      set_default();
    };

    /* Begin function to be called after initialization: */

    void begin() {
      PJON_RANDOM_SEED(PJON_ANALOG_READ(random_seed) + _device_id);
      strategy.begin(_device_id);
      #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
        _packet_id_seed = PJON_RANDOM(65535) + _device_id;
      #endif
    };

    /* Compose packet in PJON format: */

    uint16_t compose_packet(
      const uint8_t rx_id,
      const uint8_t *rx_bus_id,
      uint8_t *destination,
      const void *source,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      uint16_t l = PJONTools::compose_packet(
        _device_id,
        bus_id,
        rx_id,
        rx_bus_id,
        destination,
        source,
        length,
        (header == PJON_NO_HEADER) ? config : header,
        (!packet_id) ? PJONTools::new_packet_id(_packet_id_seed++) : packet_id,
        rx_port,
        port
      );
      if(l < PJON_PACKET_MAX_LENGTH) return l;
      _error(PJON_CONTENT_TOO_LONG, l, _custom_pointer);
      return 0;
    };

    /* Get device id: */

    uint8_t device_id() const {
      return _device_id;
    };

    /* Add packet to buffer (delivery attempt by the next update() call): */

    uint16_t dispatch(
      uint8_t rx_id,
      const uint8_t *rx_bus_id,
      const void *packet,
      uint16_t length,
      uint32_t timing,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST,
      uint16_t packet_index = PJON_FAIL
    ) {
      bool p = (packet_index != PJON_FAIL);
      for(uint16_t i = ((p) ? packet_index : 0); i < PJON_MAX_PACKETS; i++)
        if(packets[i].state == 0 || p) {
          if(!(length = compose_packet(
            rx_id,
            rx_bus_id,
            packets[i].content,
            packet,
            length,
            header,
            packet_id,
            rx_port
          ))) return PJON_FAIL;
          packets[i].length = length;
          packets[i].state = PJON_TO_BE_SENT;
          packets[i].registration = PJON_MICROS();
          packets[i].timing = timing;
          return i;
        }
      _error(PJON_PACKETS_BUFFER_FULL, PJON_MAX_PACKETS, _custom_pointer);
      return PJON_FAIL;
    };

    /* Check if a packet id is already dispatched in buffer: */

    bool dispatched(PJON_Packet_Info info) {
      PJON_Packet_Info actual_info;
      for(uint16_t i = 0; i < PJON_MAX_PACKETS; i++) {
        parse((uint8_t *)packets[i].content, actual_info);
        if(
          packets[i].state && packets[i].state != PJON_ACK &&
          (actual_info.header & PJON_ACK_MODE_BIT) &&
          (actual_info.header & PJON_TX_INFO_BIT)
        )
          if(
            actual_info.id == info.id &&
            info.sender_id == actual_info.receiver_id &&
            (
              !(info.header & PJON_MODE_BIT) ? true :
              PJONTools::bus_id_equality(
                info.receiver_bus_id,
                actual_info.receiver_bus_id
              )
            )
          ) return true;
      }
      return false;
    };

    /* Get count of packets:
       Don't pass any parameter to count all dispatched packets
       Pass a device id to count all it's related packets */

    uint16_t get_packets_count(uint8_t device_id = PJON_NOT_ASSIGNED) const {
      uint16_t packets_count = 0;
      for(uint16_t i = 0; i < PJON_MAX_PACKETS; i++) {
        if(packets[i].state == 0) continue;
        if(
          device_id == PJON_NOT_ASSIGNED ||
          packets[i].content[0] == device_id
        ) packets_count++;
      }
      return packets_count;
    };

    /* Calculate packet overhead: */

    uint8_t packet_overhead(uint8_t header = PJON_NO_HEADER) const {
      return PJONTools::packet_overhead(
        (header == PJON_NO_HEADER) ? config : header
      );
    };

    /* Fill a PJON_Packet_Info struct with data parsing a packet: */

    void parse(const uint8_t *packet, PJON_Packet_Info &packet_info) const {
      PJONTools::parse_header(packet, packet_info);
      packet_info.custom_pointer = _custom_pointer;
    };

    /* Try to receive data: */

    uint16_t receive() {
      uint16_t length = PJON_PACKET_MAX_LENGTH;
      uint16_t batch_length = 0;
      uint8_t  overhead = 0;
      bool extended_length = false;
      bool async_ack = false;
      for(uint16_t i = 0; i < length; i++) {
        if(!batch_length) {
          batch_length = strategy.receive_frame(data + i, length - i);
          if(batch_length == PJON_FAIL || batch_length == 0)
            return PJON_FAIL;
        }
        batch_length--;

        if(i == 0)
          if(data[i] != _device_id && data[i] != PJON_BROADCAST && !_router)
            return PJON_BUSY;

        if(i == 1) {
          if(
            (
              !_router &&
              ((config & PJON_MODE_BIT) && !(data[1] & PJON_MODE_BIT))
            ) || (
              data[0] == PJON_BROADCAST &&
              ((data[1] & PJON_ACK_MODE_BIT) || (data[1] & PJON_ACK_REQ_BIT))
            ) || (
              (data[1] & PJON_ACK_MODE_BIT) && !(data[1] & PJON_TX_INFO_BIT)
            ) || (
              (data[1] & PJON_EXT_LEN_BIT) && !(data[1] & PJON_CRC_BIT)
            ) || (
              !PJON_INCLUDE_ASYNC_ACK && (data[1] & PJON_ACK_MODE_BIT)
            ) || (
              !PJON_INCLUDE_PACKET_ID && (data[1] & PJON_PACKET_ID_BIT)
            )
          ) return PJON_BUSY;
          extended_length = data[i] & PJON_EXT_LEN_BIT;
          overhead = packet_overhead(data[i]);
          async_ack = (
            PJON_INCLUDE_ASYNC_ACK &&
            (data[1] & PJON_ACK_MODE_BIT) &&
            (data[1] & PJON_TX_INFO_BIT)
          );
        }

        if((i == 2) && !extended_length) {
          length = data[i];
          if(
            length < (uint8_t)(overhead + !async_ack) ||
            length >= PJON_PACKET_MAX_LENGTH
          ) return PJON_BUSY;
          if(length > 15 && !(data[1] & PJON_CRC_BIT)) return PJON_BUSY;
        }

        if((i == 3) && extended_length) {
          length = (data[i - 1] << 8) | (data[i] & 0xFF);
          if(
            length < (uint8_t)(overhead + !async_ack) ||
            length >= PJON_PACKET_MAX_LENGTH
          ) return PJON_BUSY;
          if(length > 15 && !(data[1] & PJON_CRC_BIT)) return PJON_BUSY;
        }

        if(
          ((data[1] & PJON_MODE_BIT) && !_router) &&
          (i > (uint8_t)(3 + extended_length)) &&
          (i < (uint8_t)(8 + extended_length))
        ) {
          if(config & PJON_MODE_BIT) {
            if(bus_id[i - 4 - extended_length] != data[i]) return PJON_BUSY;
          } else if(data[i] != 0) return PJON_BUSY; // Do not reject localhost
        }
      }

      if(
        PJON_crc8::compute(data, 3 + extended_length) !=
        data[3 + extended_length]
      ) return PJON_NAK;

      if(data[1] & PJON_CRC_BIT) {
        if(
          !PJON_crc32::compare(
            PJON_crc32::compute(data, length - 4), data + (length - 4)
          )
        ) return PJON_NAK;
      } else if(PJON_crc8::compute(data, length - 1) != data[length - 1])
        return PJON_NAK;

      if(data[1] & PJON_ACK_REQ_BIT && data[0] != PJON_BROADCAST)
        if((_mode != PJON_SIMPLEX) && !_router)
          strategy.send_response(PJON_ACK);

      parse(data, last_packet_info);

      #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
        bool filter =
          (last_packet_info.header & PJON_PACKET_ID_BIT) ? true : false;
        /* If a packet requesting asynchronous acknowledgement is received
           send the acknowledgement packet back to the packet's transmitter */
        if(async_ack && !_router) {
          if(_auto_delete && length == overhead)
            if(handle_asynchronous_acknowledgment(last_packet_info))
              return PJON_ACK;
          if(length > overhead) {
            if(!dispatched(last_packet_info)) {
              dispatch(
                last_packet_info.sender_id,
                (uint8_t *)last_packet_info.sender_bus_id,
                NULL,
                0,
                0,
                config | PJON_ACK_MODE_BIT | PJON_TX_INFO_BIT,
                last_packet_info.id,
                last_packet_info.port
              );
              update();
            }
            filter = true;
          }
        }
        if(filter && known_packet_id(last_packet_info) && !_router)
          return PJON_ACK;
      #endif

      if((port != PJON_BROADCAST) && (port != last_packet_info.port))
        return PJON_BUSY;

      _receiver(
        data + (overhead - PJONTools::crc_overhead(data[1])),
        length - overhead,
        last_packet_info
      );

      return PJON_ACK;
    };

    /* Try to receive data repeatedly with a maximum duration: */

    uint16_t receive(uint32_t duration) {
      uint32_t time = PJON_MICROS();
      uint16_t response;
      do response = receive();
      while(
        (response != PJON_ACK) &&
        ((uint32_t)(PJON_MICROS() - time) <= duration)
      );
      return response;
    };

    /* Remove a packet from buffer: */

    void remove(uint16_t index) {
      if((index >= 0) && (index < PJON_MAX_PACKETS)) {
        packets[index].attempts = 0;
        packets[index].length = 0;
        packets[index].registration = 0;
        packets[index].state = 0;
      }
    };

    /* Remove a packet from buffer passing its packet id as reference: */

    bool handle_asynchronous_acknowledgment(PJON_Packet_Info packet_info) {
      PJON_Packet_Info actual_info;
      for(uint16_t i = 0; i < PJON_MAX_PACKETS; i++) {
        parse((uint8_t *)packets[i].content, actual_info);
        if(actual_info.id == packet_info.id)
          if(actual_info.receiver_id == packet_info.sender_id && (
            (!(actual_info.header & PJON_MODE_BIT) &&
             !(packet_info.header & PJON_MODE_BIT)) ? true :
              PJONTools::bus_id_equality(
                actual_info.receiver_bus_id,
                packet_info.sender_bus_id
              )
          )) {
            if(packets[i].timing) {
              uint8_t offset = packet_overhead(actual_info.header);
              uint8_t crc_offset =
                PJONTools::crc_overhead(actual_info.header);
              dispatch(
                actual_info.receiver_id,
                (uint8_t *)actual_info.receiver_bus_id,
                packets[i].content + (offset - crc_offset),
                packets[i].length - offset,
                packets[i].timing,
                actual_info.header,
                PJONTools::new_packet_id(_packet_id_seed++),
                actual_info.port,
                i
              );
              packets[i].attempts = 0;
              return true;
            }
            remove(i);
            return true;
          }
      }
      return false;
    };

    /* Remove all packets from the buffer:
       Don't pass any parameter to delete all packets
       Pass a device id to delete all it's related packets  */

    void remove_all_packets(uint8_t device_id = 0) {
      for(uint16_t i = 0; i < PJON_MAX_PACKETS; i++) {
        if(packets[i].state == 0) continue;
        if(!device_id || packets[i].content[0] == device_id) remove(i);
      }
    };

    /* Schedule a packet sending to the sender of the last packet received.
       This function is typically called within the receive callback to
       deliver a response to a request. */

    uint16_t reply(
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      if(last_packet_info.sender_id != PJON_BROADCAST)
        return dispatch(
          last_packet_info.sender_id,
          last_packet_info.sender_bus_id,
          payload,
          length,
          0,
          ((header == PJON_NO_HEADER) ? last_packet_info.header : header),
          packet_id,
          rx_port
        );
      return false;
    };

    /* Schedule a packet sending: */

    uint16_t send(
      uint8_t rx_id,
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      return dispatch(
        rx_id, bus_id, payload, length, 0, header, packet_id, rx_port
      );
    };

    uint16_t send(
      uint8_t rx_id,
      const uint8_t *rx_bus_id,
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      return dispatch(
        rx_id, rx_bus_id, payload, length, 0, header, packet_id, rx_port
      );
    };

    /* Schedule a packet sending configuring its sender info:  */

    uint16_t send_from_id(
      uint8_t tx_id,
      const uint8_t *tx_bus_id,
      uint8_t rx_id,
      const uint8_t *rx_bus_id,
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      uint8_t original_device_id = _device_id;
      uint8_t original_bus_id[4];
      PJONTools::copy_bus_id(original_bus_id, bus_id);
      set_id(tx_id);
      PJONTools::copy_bus_id(bus_id, tx_bus_id);
      uint16_t result = PJON_FAIL;
      #if(PJON_MAX_PACKETS > 0)
        result = dispatch(
          rx_id, rx_bus_id, payload, length, 0, header, packet_id, rx_port
        );
      #endif
      if(result == PJON_FAIL)
        result = send_packet_blocking(
          rx_id, rx_bus_id, payload, length, header, packet_id, rx_port
        );
      PJONTools::copy_bus_id(bus_id, original_bus_id);
      set_id(original_device_id);
      return result;
    };

    /* IMPORTANT: send_repeatedly timing maximum
       is 4293014170 microseconds or 71.55 minutes */

    uint16_t send_repeatedly(
      uint8_t rx_id,
      const void *payload,
      uint16_t length,
      uint32_t timing,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      return dispatch(
        rx_id, bus_id, payload, length, timing, header, packet_id, rx_port
      );
    };

    /* IMPORTANT: send_repeatedly timing maximum
       is 4293014170 microseconds or 71.55 minutes */

    uint16_t send_repeatedly(
      uint8_t rx_id,
      const uint8_t *rx_bus_id,
      const void *payload,
      uint16_t length,
      uint32_t timing,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      return dispatch(
        rx_id, rx_bus_id, payload, length, timing, header, packet_id, rx_port
      );
    };

    /* Transmit an already composed packet:  */

    uint16_t send_packet(const uint8_t *payload, uint16_t length) {
      if(!payload) return PJON_FAIL;
      if(_mode != PJON_SIMPLEX && !strategy.can_start()) return PJON_BUSY;
      strategy.send_frame((uint8_t *)payload, length);
      if(
        payload[0] == PJON_BROADCAST ||
        !(payload[1] & PJON_ACK_REQ_BIT) ||
        _mode == PJON_SIMPLEX
      ) return PJON_ACK;
      uint16_t response = strategy.receive_response();
      if(
        response == PJON_ACK ||
        response == PJON_FAIL
      ) return response;
      else return PJON_BUSY;
    };

    /* Compose and transmit a packet passing its info as parameters: */

    uint16_t send_packet(
      uint8_t rx_id,
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      if(!(length = compose_packet(
        rx_id, bus_id, data, payload, length, header, packet_id, rx_port
      ))) return PJON_FAIL;
      return send_packet(data, length);
    };

    uint16_t send_packet(
      uint8_t rx_id,
      const uint8_t *rx_bus_id,
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST
    ) {
      if(!(length = compose_packet(
        rx_id, rx_bus_id, data, payload, length, header, packet_id, rx_port
      ))) return PJON_FAIL;
      return send_packet(data, length);
    };

    /* Transmit a packet without using the packet's buffer. Tries to transmit
       a packet multiple times within an internal cycle until the packet is
       delivered, or timing limit is reached. */

    uint16_t send_packet_blocking(
      uint8_t rx_id,
      const uint8_t *rx_bus_id,
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST,
      uint32_t timeout = 3500000
    ) {
      uint16_t state = PJON_FAIL;
      uint32_t attempts = 0;
      uint32_t start = PJON_MICROS();
      uint16_t old_length = length;

      _recursion++;
      while(
        (state != PJON_ACK) && (attempts <= strategy.get_max_attempts()) &&
        (uint32_t)(PJON_MICROS() - start) <= timeout
      ) {
        if(!(length = compose_packet(
          rx_id,
          rx_bus_id,
          data,
          payload,
          old_length,
          header,
          packet_id,
          rx_port
        ))) {
          _recursion--;
          return PJON_FAIL;
        }
        state = send_packet(data, length);
        if(state == PJON_ACK) {
          _recursion--;
          return state;
        }
        attempts++;
        if(state != PJON_FAIL) strategy.handle_collision();
        #if(PJON_RECEIVE_WHILE_SENDING_BLOCKING)
          if(_recursion <= 1) receive(strategy.back_off(attempts));
          else
        #endif
        PJON_DELAY((uint32_t)(strategy.back_off(attempts) / 1000));
      }
      _recursion--;
      return state;
    };

    uint16_t send_packet_blocking(
      uint8_t rx_id,
      const void *payload,
      uint16_t length,
      uint8_t  header = PJON_NO_HEADER,
      uint16_t packet_id = 0,
      uint16_t rx_port = PJON_BROADCAST,
      uint32_t timeout = 3000000
    ) {
      return send_packet_blocking(
        rx_id, bus_id, payload, length, header, packet_id, rx_port, timeout
      );
    };

    /* In router mode, the receiver function can acknowledge
       for selected receiver device ids for which the route is known */

    void send_synchronous_acknowledge() {
      strategy.send_response(PJON_ACK);
    };

    /* Set the config bit state: */

    void set_config_bit(bool new_state, uint8_t bit) {
      if(new_state) config |= bit;
      else config &= ~bit;
    };

    /* Configure synchronous acknowledge presence:
       TRUE: Send 1 byte synchronous acknowledge when a packet is received
       FALSE: Avoid acknowledge transmission */

    void set_synchronous_acknowledge(bool state) {
      set_config_bit(state, PJON_ACK_REQ_BIT);
    };

    /* Configure asynchronous acknowledge presence:
       TRUE: Send back asynchronous acknowledge packet
       FALSE: Avoid acknowledge packet transmission */

    void set_asynchronous_acknowledge(bool state) {
      set_config_bit(state, PJON_ACK_MODE_BIT);
    };

    /* Configure CRC selected for packet checking:
       TRUE: CRC32, FALSE: CRC8 */

    void set_crc_32(bool state) {
      set_config_bit(state, PJON_CRC_BIT);
    };

    /* Set communication mode:
       Passing PJON_SIMPLEX communication is mono-directional
       Padding PJON_HALF_DUPLEX communication is bi-directional */

    void set_communication_mode(uint8_t mode) {
      _mode = mode;
    };

    /* Configure packet id presence:
       TRUE: include packet id, FALSE: Avoid packet id inclusion */

    void set_packet_id(bool state) {
      set_config_bit(state, PJON_PACKET_ID_BIT);
    };

    /* Set a custom receiver callback pointer:
       (Generally needed to call a custom member function) */

    void set_custom_pointer(void *pointer) {
      _custom_pointer = pointer;
    };

    /* Set bus state default configuration: */

    void set_default() {
      _mode = PJON_HALF_DUPLEX;
      if(!PJONTools::bus_id_equality(bus_id, PJONTools::localhost()))
        set_shared_network(true);
      set_error(PJON_dummy_error_handler);
      set_receiver(PJON_dummy_receiver_handler);
      for(uint16_t i = 0; i < PJON_MAX_PACKETS; i++) {
        packets[i].state = 0;
        packets[i].timing = 0;
        packets[i].attempts = 0;
      }
    };

    /* Pass as a parameter a function you previously defined in the code.
       This function is called when PJON detects an error

    void error_handler(uint8_t code, uint16_t data) {
      Serial.print(code);
      Serial.print(" ");
      Serial.println(data);
    };

    bus.set_error(error_handler); */

    void set_error(PJON_Error e) {
      _error = e;
    };

    /* Set the device id passing a single byte (watch out to id collision): */

    void set_id(uint8_t id) {
      _device_id = id;
    };

    /* Include the port passing a boolean state and an unsigned integer: */

    void include_port(bool state, uint16_t p = PJON_BROADCAST) {
      set_config_bit(state, PJON_PORT_BIT);
      port = p;
    };

    /* Configure sender's information inclusion in the packet.
       TRUE: +1 byte (device id) local, +5 bytes (bus id + device id) shared
       FALSE: No inclusion -1 byte overhead in local, -5 in shared

       If you don't need the sender info disable the inclusion to reduce
       overhead and higher communication speed. */

    void include_sender_info(bool state) {
      set_config_bit(state, PJON_TX_INFO_BIT);
    };

    /* Configure the bus network behaviour.
       TRUE: Include 4 bytes bus id or group identification.
       FALSE: Use only 1 byte local device identification. */

    void set_shared_network(bool state) {
      set_config_bit(state, PJON_MODE_BIT);
    };

    /* Set if delivered or undeliverable packets are auto deleted:
       TRUE: Automatic deletion
       FALSE: No packet deleted from buffer, (deletion from buffer by user) */

    void set_packet_auto_deletion(bool state) {
      _auto_delete = state;
    };

    /* Set the analog pin used as a seed for random generation: */

    void set_random_seed(uint8_t seed) {
      random_seed = seed;
    };

    /* Pass as a parameter a receiver function you previously defined in your
       code that should be called when a message is received.
       Inside there you can code how to react when data is received. */

    void set_receiver(PJON_Receiver r) {
      _receiver = r;
    };

    /* Configure if device acts as a router:
       TRUE: device receives messages only for its bus and device id
       FALSE: receiver function is always called if data is received */

    void set_router(bool state) {
      _router = state;
    };

    /* Update the state of the send list:
       Check if there are packets to be sent or to be erased if correctly
       delivered. Returns the actual number of packets to be sent. */

    uint16_t update() {
      uint16_t packets_count = 0;
      for(uint16_t i = 0; i < PJON_MAX_PACKETS; i++) {
        if(packets[i].state == 0) continue;
        packets_count++;
        bool async_ack = (packets[i].content[1] & PJON_ACK_MODE_BIT) &&
          (packets[i].content[1] & PJON_TX_INFO_BIT);
        bool sync_ack = (packets[i].content[1] & PJON_ACK_REQ_BIT);

        if(
          (uint32_t)(PJON_MICROS() - packets[i].registration) >
          (uint32_t)(
            packets[i].timing +
            strategy.back_off(packets[i].attempts)
          )
        ) {

          #ifdef PJON_INJECT_ATTEMPTS
          //mxb added to replace last byte of payload with packet[i].attempts and recalculate checksum
          uint16_t new_length = packets[i].length;
          if(packets[i].content[1] & PJON_CRC_BIT) {
            packets[i].content[packets[i].length-5] = packets[i].attempts;
            uint32_t computed_crc =
              PJON_crc32::compute((uint8_t *)packets[i].content, new_length - 4);
            packets[i].content[new_length - 4] =
              (uint8_t)((uint32_t)(computed_crc) >> 24);
            packets[i].content[new_length - 3] =
              (uint8_t)((uint32_t)(computed_crc) >> 16);
            packets[i].content[new_length - 2] =
              (uint8_t)((uint32_t)(computed_crc) >>  8);
            packets[i].content[new_length - 1] =
              (uint8_t)((uint32_t)computed_crc);
          } else {
            packets[i].content[packets[i].length-1] = packets[i].attempts;
            packets[i].content[new_length - 1] =
            PJON_crc8::compute((uint8_t *)packets[i].content, new_length - 1);
          }
          #endif

          if(!(sync_ack && async_ack && packets[i].state == PJON_ACK))
            packets[i].state = // Avoid resending sync-acked async ack packets
              send_packet(packets[i].content, packets[i].length);
        } else continue;

        packets[i].attempts++;

        if(packets[i].state == PJON_ACK) {
          if(!packets[i].timing) {
            if(
              _auto_delete && (
                (
                  packets[i].length == packet_overhead(packets[i].content[1])
                  && async_ack
                ) || !(packets[i].content[1] & PJON_ACK_MODE_BIT)
              )
            ) {
              remove(i);
              packets_count--;
            }
          } else {
            if(!async_ack) {
              packets[i].attempts = 0;
              packets[i].registration = PJON_MICROS();
              packets[i].state = PJON_TO_BE_SENT;
            }
          }
          if(!async_ack) continue;
        }

        if(packets[i].state != PJON_FAIL && packets[i].state != PJON_ACK)
          strategy.handle_collision();

        if(packets[i].attempts > strategy.get_max_attempts()) {
          _error(PJON_CONNECTION_LOST, i, _custom_pointer);
          if(!packets[i].timing) {
            if(_auto_delete) {
              remove(i);
              packets_count--;
            }
          } else {
            packets[i].attempts = 0;
            packets[i].registration = PJON_MICROS();
            packets[i].state = PJON_TO_BE_SENT;
          }
        }
      }
      return packets_count;
    };

    /* Check if the packet id and its transmitter info are already present in
       buffer of recently received packets, if not add it to the buffer. */

    bool known_packet_id(PJON_Packet_Info info) {
      #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
        for(uint8_t i = 0; i < PJON_MAX_RECENT_PACKET_IDS; i++)
          if(
            info.id == recent_packet_ids[i].id &&
            info.sender_id == recent_packet_ids[i].sender_id && (
              (
                (info.header & PJON_MODE_BIT) &&
                (recent_packet_ids[i].header & PJON_MODE_BIT) &&
                PJONTools::bus_id_equality(
                  (uint8_t *)info.sender_bus_id,
                  (uint8_t *)recent_packet_ids[i].sender_bus_id
                )
              ) || (
                !(info.header & PJON_MODE_BIT) &&
                !(recent_packet_ids[i].header & PJON_MODE_BIT)
              )
            )
          ) return true;
        save_packet_id(info);
      #endif
      return false;
    };

    /* Save packet id in the buffer: */

    void save_packet_id(PJON_Packet_Info info) {
      #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
        for(uint8_t i = PJON_MAX_RECENT_PACKET_IDS - 1; i > 0; i--)
          recent_packet_ids[i] = recent_packet_ids[i - 1];
        recent_packet_ids[0].id = info.id;
        recent_packet_ids[0].header = info.header;
        recent_packet_ids[0].sender_id = info.sender_id;
        PJONTools::copy_bus_id(
          recent_packet_ids[0].sender_bus_id,
          info.sender_bus_id
        );
      #endif
    };

  private:
    bool          _auto_delete = true;
    void         *_custom_pointer;
    PJON_Error    _error;
    uint8_t       _mode;
    uint16_t      _packet_id_seed = 0;
    PJON_Receiver _receiver;
    uint8_t       _recursion = 0;
    bool          _router = false;
  protected:
    uint8_t       _device_id;
};
