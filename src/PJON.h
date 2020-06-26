
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

static void PJON_dummy_receiver_handler(
  uint8_t *,               // payload
  uint16_t,                // length
  const PJON_Packet_Info & // packet_info
) {};

static void PJON_dummy_error_handler(
  uint8_t,                  // code
  uint16_t,                 // data
  void *                    // custom_pointer
) {};

template<typename Strategy>
class PJON {
  public:
    Strategy strategy;
    uint8_t config = PJON_TX_INFO_BIT | PJON_ACK_REQ_BIT;
    uint8_t data[PJON_PACKET_MAX_LENGTH];
    PJON_Packet_Info last_packet_info;
    PJON_Packet packets[PJON_MAX_PACKETS];
    uint8_t random_seed = A0;
    PJON_Endpoint tx;

    #if(PJON_INCLUDE_PACKET_ID)
      PJON_Packet_Record recent_packet_ids[PJON_MAX_RECENT_PACKET_IDS];
    #endif

    #if(PJON_INCLUDE_PORT)
      uint16_t port = PJON_BROADCAST;
    #endif

    /* PJON initialization with no parameters:
        State: Local (bus_id: 0.0.0.0)
        Acknowledge: true
        device id: PJON_NOT_ASSIGNED (255)
        Mode: PJON_HALF_DUPLEX
        Sender info: true (Sender info is included in the packet)

       PJON<SoftwareBitBang> bus; */

    PJON() : strategy(Strategy()) {
      set_default();
    };

    /* PJON initialization passing device id:
       PJON<SoftwareBitBang> bus(1); */

    PJON(uint8_t device_id) : strategy(Strategy()) {
      tx.id = device_id;
      set_default();
    };

    /* PJON initialization passing bus and device id:
       uint8_t my_bus = {1, 1, 1, 1};
       PJON<SoftwareBitBang> bus(my_bys, 1); */

    PJON(const uint8_t *b_id, uint8_t device_id) : strategy(Strategy()) {
      tx.id = device_id;
      PJONTools::copy_id(tx.bus_id, b_id, 4);
      config |= PJON_MODE_BIT;
      set_default();
    };

    #if(PJON_INCLUDE_MAC)

      /* PJON initialization passing the mac address:
      const uint8_t mac[6] = {1, 2, 3, 4, 5, 6};
      PJON<SoftwareBitBang> bus(mac); */

      PJON(const uint8_t *mac_addr) : strategy(Strategy()) {
        PJONTools::copy_id(tx.mac, mac_addr, 6);
        config |= PJON_MAC_BIT;
        set_default();
      };

    #endif

    /* Begin function to be called after initialization: */

    void begin() {
      PJON_RANDOM_SEED(PJON_ANALOG_READ(random_seed) + tx.id);
      strategy.begin(tx.id);
      #if(PJON_INCLUDE_PACKET_ID)
        _packet_id_seed = PJON_RANDOM(65535) + tx.id;
      #endif
    };

    /* Compose packet in PJON format: */

    uint16_t compose_packet(
      PJON_Packet_Info info,
      uint8_t *destination,
      const void *source,
      uint16_t length
    ) {
      info.header = (info.header == PJON_NO_HEADER) ? config : info.header;
      info.tx = tx;
      #if(PJON_INCLUDE_PACKET_ID)
        if(!info.id && (info.header & PJON_PACKET_ID_BIT))
          info.id = PJONTools::new_packet_id(_packet_id_seed++);
      #endif
      #if(PJON_INCLUDE_PORT)
        if(
          (port != PJON_BROADCAST) && (info.port == PJON_BROADCAST) &&
          (info.header & PJON_PORT_BIT)
        ) info.port = port;
      #endif
      #if(PJON_INCLUDE_MAC)
        if(info.header & PJON_MAC_BIT)
          PJONTools::copy_id(info.tx.mac, tx.mac, 6);
      #endif
      uint16_t l = PJONTools::compose_packet(
        info, destination, source, length
      );
      if(l < PJON_PACKET_MAX_LENGTH) return l;
      _error(PJON_CONTENT_TOO_LONG, l, _custom_pointer);
      return 0;
    };

    /* Get device id: */

    uint8_t device_id() const { return tx.id; };

    /* Add packet to buffer (delivery attempt by the next update() call): */

    uint16_t dispatch(
      const PJON_Packet_Info &info,
      const void *packet,
      uint16_t length,
      uint32_t timing = 0,
      uint16_t packet_index = PJON_FAIL
    ) {
      bool p = (packet_index != PJON_FAIL);
      for(uint16_t i = ((p) ? packet_index : 0); i < PJON_MAX_PACKETS; i++)
        if(packets[i].state == 0 || p) {
          if(!(length = compose_packet(
            info, packets[i].content, packet, length
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

    /* Returns a pointer to the bus id used by the instance: */

    const uint8_t *get_bus_id() const {
      return tx.bus_id;
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

    /* Fill a PJON_Packet_Info using parameters: */

    PJON_Packet_Info fill_info(
      uint8_t rx_id,
      uint8_t header,
      uint16_t packet_id,
      uint16_t rx_port
    ) {
      PJON_Packet_Info info;
      info.rx.id = rx_id;
      info.header = header;
      PJONTools::copy_id(info.rx.bus_id, tx.bus_id, 4);
      #if(PJON_INCLUDE_PACKET_ID)
        info.id = packet_id;
      #else
        (void)packet_id;
      #endif
      #if(PJON_INCLUDE_PORT)
        info.port = rx_port;
      #else
        (void)rx_port;
      #endif
      return info;
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
      bool extended_length = false, mac = false, drop = false;
      for(uint16_t i = 0; i < length; i++) {
        if(!batch_length) {
          batch_length = strategy.receive_frame(data + i, length - i);
          if(batch_length == PJON_FAIL || batch_length == 0)
            return PJON_FAIL;
        }
        batch_length--;

        if(i == 0)
          if((data[i] != tx.id) && (data[i] != PJON_BROADCAST) && !_router)
            drop = true;

        if(i == 1) {
          mac = (data[1] & PJON_MAC_BIT);
          if(
            (
              !_router &&
              ((config & PJON_MODE_BIT) && !(data[1] & PJON_MODE_BIT))
            ) || (
              (data[0] == PJON_BROADCAST) && (data[1] & PJON_ACK_REQ_BIT)
            ) || (
              (data[1] & PJON_EXT_LEN_BIT) && !(data[1] & PJON_CRC_BIT)
            ) || (
              !PJON_INCLUDE_PACKET_ID && (data[1] & PJON_PACKET_ID_BIT)
            ) || (
              !PJON_INCLUDE_PORT && (data[1] & PJON_PORT_BIT)
            ) || (
              (!PJON_INCLUDE_MAC && mac) || (mac && !(data[1] & PJON_CRC_BIT))
            )
          ) return PJON_BUSY;
          if(drop && !mac) return PJON_BUSY;
          extended_length = data[i] & PJON_EXT_LEN_BIT;
          overhead = packet_overhead(data[i]);
        }

        if((i == 2) && !extended_length) {
          length = data[i];
          if(
            length < (uint8_t)(overhead + 1) ||
            length >= PJON_PACKET_MAX_LENGTH
          ) return PJON_BUSY;
          if(length > 15 && !(data[1] & PJON_CRC_BIT)) return PJON_BUSY;
        }

        if((i == 3) && extended_length) {
          length = (data[i - 1] << 8) | (data[i] & 0xFF);
          if(
            length < (uint8_t)(overhead + 1) ||
            length >= PJON_PACKET_MAX_LENGTH
          ) return PJON_BUSY;
          if(length > 15 && !(data[1] & PJON_CRC_BIT)) return PJON_BUSY;
        }

        if(
          ((data[1] & PJON_MODE_BIT) && !_router && !mac) &&
          (i > (uint8_t)(3 + extended_length)) &&
          (i < (uint8_t)(8 + extended_length))
        ) {
          if(config & PJON_MODE_BIT) {
            if(tx.bus_id[i - 4 - extended_length] != data[i])
              return PJON_BUSY;
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

      #if(PJON_INCLUDE_MAC)
        if(mac && (length > 15) && !_router)
          if(!PJONTools::id_equality(data + (overhead - 16), tx.mac, 6))
            if(!
              PJONTools::id_equality(
                data + (overhead - 16),
                PJONTools::no_mac(), 6
              )
            ) return PJON_BUSY;
      #endif

      if(data[1] & PJON_ACK_REQ_BIT && data[0] != PJON_BROADCAST)
        if((_mode != PJON_SIMPLEX) && !_router)
          strategy.send_response(PJON_ACK);

      parse(data, last_packet_info);

      #if(PJON_INCLUDE_PACKET_ID)
        if(
          !_router &&
          (last_packet_info.header & PJON_PACKET_ID_BIT) &&
          known_packet_id(last_packet_info)
        ) return PJON_ACK;
      #endif

      #if(PJON_INCLUDE_PORT)
        if((port != PJON_BROADCAST) && (port != last_packet_info.port))
          return PJON_BUSY;
      #endif

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

    /* Remove all packets from the buffer:
       Don't pass any parameter to delete all packets
       Pass a device id to delete all it's related packets  */

    void remove_all_packets(uint8_t device_id = 0) {
      for(uint16_t i = 0; i < PJON_MAX_PACKETS; i++) {
        if(packets[i].state == 0) continue;
        if(!device_id || packets[i].content[0] == device_id) remove(i);
      }
    };

    /* Reset a packet sending present in the buffer: */

    bool reset_packet(uint16_t id) {
      if(!packets[id].timing) {
        if(_auto_delete) {
          remove(id);
          return true;
        }
      } else {
        packets[id].attempts = 0;
        packets[id].registration = PJON_MICROS();
        packets[id].state = PJON_TO_BE_SENT;
      }
      return false;
    };

    /* Schedule a packet sending to the sender of the last packet received.
       This function is typically called within the receive callback to
       deliver a response to a request. */

    uint16_t reply(const void *payload, uint16_t length) {
      PJON_Packet_Info info;
      info = last_packet_info;
      info.rx = info.tx;
      info.header = config;
      #ifndef PJON_LOCAL
        info.hops = 0;
      #endif
      return dispatch(info, payload, length);
    };

    uint16_t reply_blocking(const void *payload, uint16_t length) {
      PJON_Packet_Info info;
      info = last_packet_info;
      info.rx = info.tx;
      info.header = config;
      #ifndef PJON_LOCAL
        info.hops = 0;
      #endif
      return send_packet_blocking(info, payload, length);
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
      PJON_Packet_Info info = fill_info(rx_id, header, packet_id, rx_port);
      return dispatch(info, payload, length);
    };

    uint16_t send(
      const PJON_Packet_Info &info,
      const void *payload,
      uint16_t length
    ) {
      return dispatch(info, payload, length);
    };

    /* Forward a packet:  */

    uint16_t forward(
      PJON_Packet_Info info,
      const void *payload,
      uint16_t length
    ) {
      PJON_Endpoint original_end_point = tx;
      tx = info.tx;
      #ifndef PJON_LOCAL
        if(++info.hops > PJON_MAX_HOPS) return PJON_FAIL;
      #endif
      uint16_t result = dispatch(info, payload, length);
      tx = original_end_point;
      return result;
    };

    /* Forward a packet:  */

    uint16_t forward_blocking(
      PJON_Packet_Info info,
      const void *payload,
      uint16_t length
    ) {
      PJON_Endpoint original_end_point = tx;
      tx = info.tx;
      #ifndef PJON_LOCAL
        if(++info.hops > PJON_MAX_HOPS) return PJON_FAIL;
      #endif
      uint16_t result = send_packet_blocking(info, payload, length);
      tx = original_end_point;
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
      PJON_Packet_Info info = fill_info(rx_id, header, packet_id, rx_port);
      return dispatch(info, payload, length, timing);
    };

    uint16_t send_repeatedly(
      const PJON_Packet_Info &info,
      const void *payload,
      uint16_t length,
      uint32_t timing
    ) {
      return dispatch(info, payload, length, timing);
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
      PJON_Packet_Info info = fill_info(rx_id, header, packet_id, rx_port);
      if(!(length = compose_packet(info, data, payload, length)))
        return PJON_FAIL;
      return send_packet(data, length);
    };

    uint16_t send_packet(
      const PJON_Packet_Info &info,
      const void *payload,
      uint16_t length
    ) {
      if(!(length = compose_packet(info, data, payload, length)))
        return PJON_FAIL;
      return send_packet(data, length);
    };

    /* Transmit a packet without using the packet's buffer. Tries to transmit
       a packet multiple times within an internal cycle until the packet is
       delivered, or timing limit is reached. */

    uint16_t send_packet_blocking(
      const PJON_Packet_Info &packet_info,
      const void *payload,
      uint16_t length,
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
        if(!(length = compose_packet(packet_info, data, payload, old_length))) {
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
      uint32_t timeout = 3500000
    ) {
      PJON_Packet_Info info = fill_info(rx_id, header, packet_id, rx_port);
      return send_packet_blocking(info, payload, length, timeout);
    };

    /* In router mode, the receiver function can acknowledge
       for selected receiver device ids for which the route is known */

    void send_acknowledge() {
      strategy.send_response(PJON_ACK);
    };

    /* Set the config bit state: */

    void set_config_bit(bool new_state, uint8_t bit) {
      if(new_state) config |= bit;
      else config &= ~bit;
    };

    /* Configure acknowledge:
       state = true  -> Request acknowledgement
       state = false -> Do not request acknowledgement */

    void set_acknowledge(bool state) {
      set_config_bit(state, PJON_ACK_REQ_BIT);
    };

    /* Configure CRC selected for packet checking:
       state = true  -> Use CRC32
       state = false -> Use CRC8 */

    void set_crc_32(bool state) {
      set_config_bit(state, PJON_CRC_BIT);
    };

    /* Set communication mode:
       mode = 0 or PJON_SIMPLEX     -> Communication is mono-directional
       mode = 1 or PJON_HALF_DUPLEX -> Communication is bi-directional */

    void set_communication_mode(bool mode) {
      _mode = mode;
    };

    /* Set a custom receiver callback pointer:
       (Generally needed to call a custom member function) */

    void set_custom_pointer(void *pointer) {
      _custom_pointer = pointer;
    };

    /* Set bus state default configuration: */

    void set_default() {
      _mode = PJON_HALF_DUPLEX;
      set_error(PJON_dummy_error_handler);
      set_receiver(PJON_dummy_receiver_handler);
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
      tx.id = id;
    };

    /* Setting bus id */

    void set_bus_id(const uint8_t *b_id) {
      PJONTools::copy_id(tx.bus_id, b_id, 4);
    };

    /* Configure sender's information inclusion in the packet.
       state = true -> +8 bits (device id) in local mode
                       +40 bits (bus id + device id) in shared mode
       state = false -> No overhead added

       If you don't need the sender info disable the inclusion to reduce
       overhead and higher communication speed. */

    void include_sender_info(bool state) {
      set_config_bit(state, PJON_TX_INFO_BIT);
    };

    /* Configure network interface identification inclusion in the packet.
       state = true -> +96 bits (sender's and recipient's MAC address)
       state = false -> No overhead added */

    void include_mac(bool state) {
      set_config_bit(state, PJON_MAC_BIT);
    };

    #if(PJON_INCLUDE_MAC)

      /* Returns a pointer to the mac address used by the instance: */

      const uint8_t *get_mac() const {
        return tx.mac;
      };

      /* Set the mac address used by the instance:
         It receives a pointer to the mac address */

      void set_mac(const uint8_t *mac) {
        PJONTools::copy_id(tx.mac, mac, 6);
      };

    #endif

    /* Configure the bus network behaviour.
       state = true  -> Include 32 bits bus id or group identification.
       state = false -> Use only a 8 bits local device identification. */

    void set_shared_network(bool state) {
      set_config_bit(state, PJON_MODE_BIT);
    };

    /* Set if packets are automatically deleted in case of success or failure:
       state = true  -> Packets are deleted automatically
       state = false -> Packets are not deleted */

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
       state = true  -> All packets are received (acknowledgement not sent)
       state = false -> Normal operation */

    void set_router(bool state) {
      _router = state;
    };

    /* Update the state of the send list:
       Checks if there are packets to be sent or to be erased if correctly
       delivered. Returns the actual number of packets to be sent. */

    uint16_t update() {
      uint16_t packets_count = 0;
      for(uint16_t i = 0; i < PJON_MAX_PACKETS; i++) {
        if(packets[i].state == 0) continue;
        packets_count++;

        if(
          (uint32_t)(PJON_MICROS() - packets[i].registration) >
          (uint32_t)(
            packets[i].timing +
            strategy.back_off(packets[i].attempts)
          )
        ) {
          if(packets[i].state != PJON_ACK)
            packets[i].state =
              send_packet(packets[i].content, packets[i].length);
        } else continue;

        packets[i].attempts++;

        if(packets[i].state == PJON_ACK) {
          packets_count -= reset_packet(i);
          continue;
        }

        if(packets[i].state != PJON_FAIL && packets[i].state != PJON_ACK)
          strategy.handle_collision();

        if(packets[i].attempts > strategy.get_max_attempts()) {
          _error(PJON_CONNECTION_LOST, i, _custom_pointer);
          packets_count -= reset_packet(i);
        }
      }
      return packets_count;
    };

    #if(PJON_INCLUDE_PACKET_ID)

      /* Checks if the packet id and its transmitter info are already present
         in the known packets buffer, if not add it to the buffer */

      bool known_packet_id(const PJON_Packet_Info &info) {
        for(uint8_t i = 0; i < PJON_MAX_RECENT_PACKET_IDS; i++)
          if(
            info.id == recent_packet_ids[i].id &&
            info.tx.id == recent_packet_ids[i].sender_id && (
              (
                (info.header & PJON_MODE_BIT) &&
                (recent_packet_ids[i].header & PJON_MODE_BIT) &&
                PJONTools::id_equality(
                  (uint8_t *)info.tx.bus_id,
                  (uint8_t *)recent_packet_ids[i].sender_bus_id,
                  4
                )
              ) || (
                !(info.header & PJON_MODE_BIT) &&
                !(recent_packet_ids[i].header & PJON_MODE_BIT)
              )
            )
          ) return true;
        save_packet_id(info);
        return false;
      };

      /* Save packet id in the buffer: */

      void save_packet_id(const PJON_Packet_Info &info) {
        for(uint8_t i = PJON_MAX_RECENT_PACKET_IDS - 1; i > 0; i--)
          recent_packet_ids[i] = recent_packet_ids[i - 1];
        recent_packet_ids[0].id = info.id;
        recent_packet_ids[0].header = info.header;
        recent_packet_ids[0].sender_id = info.tx.id;
        PJONTools::copy_id(
          recent_packet_ids[0].sender_bus_id,
          info.tx.bus_id,
          4
        );
      };

      /* Configure packet id presence:
         state = true  -> Include 16 bits packet id
         state = false -> Avoid packet id inclusion */

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
    bool          _auto_delete = true;
    void         *_custom_pointer;
    PJON_Error    _error;
    bool          _mode;
    uint16_t      _packet_id_seed = 0;
    PJON_Receiver _receiver;
    uint8_t       _recursion = 0;
    bool          _router = false;
};
