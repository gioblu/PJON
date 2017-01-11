
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

Credits to contributors:
- Fred Larsen (Systems engineering, header driven communication, debugging)
- budaics github user (ATtiny85 16Mhz external clock testing and wiki page enhancement)
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
- pacproduct github user (Added missing mode configuration SIMPLEX example)
- elusive-code github user (PJONMaster reset bug)
- Franketto arduino forum user (PJON ThroughSerial over RS485 delay issue)
- Zbigniew Zasieczny (header reference inconsistency report)
- DanRoad reddit user (can_start ThroughSerial bugfix)
- Remo Kallio (Packet index 0 bugfix)
- Emanuele Iannone (Forcing SIMPLEX in OverSamplingSimplex)
- Christian Pointner (Fixed compiler warnings)
- Andrew Grande (ESP8266 example watchdog error bug fix)
- Fabian Gärtner (receive function and big packets bugfix)
- Mauro Mombelli (Code cleanup)
- Shachar Limor (Blink example pinMode bugfix)
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

#ifndef PJON_h
  #define PJON_h
  #include <Arduino.h>
  #include <PJONDefines.h>
  #include "strategies/OverSampling/OverSampling.h"
  #include "strategies/SoftwareBitBang/SoftwareBitBang.h"
  #include "strategies/ThroughSerial/ThroughSerial.h"
  /* Avoid ATtiny 45/85 error missing inclusion error */
  #if !defined(__AVR_ATtiny45__) && !defined(__AVR_ATtiny85__)
    #include "strategies/EthernetTCP/EthernetTCP.h"
    #include "strategies/LocalUDP/LocalUDP.h"
  #endif

  template<typename Strategy = SoftwareBitBang>
  class PJON {
    public:
      /* Abstract data-link layer class */
      Strategy strategy;

      uint16_t config = SENDER_INFO_BIT | ACK_REQUEST_BIT;
      uint8_t bus_id[4] = {0, 0, 0, 0};
      const uint8_t localhost[4] = {0, 0, 0, 0};

      /* Data buffers */
      uint8_t data[PACKET_MAX_LENGTH];
      PacketInfo last_packet_info;
      PJON_Packet packets[MAX_PACKETS];
      #if(INCLUDE_ASYNC_ACK)
        PJON_Packet_Record recent_packet_ids[MAX_RECENT_PACKET_IDS];
      #endif

      /* PJON bus default initialization:
         State: Local (bus_id: 0.0.0.0)
         Acknowledge: true (Acknowledge is requested)
         device id: NOT_ASSIGNED (255)
         Mode: HALF_DUPLEX
         Sender info: true (Sender info are included in the packet)
         Strategy: SoftwareBitBang */

      PJON() : strategy(Strategy()) {
        _device_id = NOT_ASSIGNED;
        set_default();
      };


      /* PJON initialization passing device id:
         PJON bus(1); */

      PJON(uint8_t device_id) : strategy(Strategy()) {
        _device_id = device_id;
        set_default();
      };


      /* PJON initialization passing bus and device id:
         uint8_t my_bus = {1, 1, 1, 1};
         PJON bus(my_bys, 1); */

      PJON(const uint8_t *b_id, uint8_t device_id) : strategy(Strategy()) {
        copy_bus_id(bus_id, b_id);
        _device_id = device_id;
        set_default();
      };


      /* Begin function to be called in setup: */

      void begin() {
        uint8_t device_id_seed = (_device_id != NOT_ASSIGNED) ? _device_id : 0;
        randomSeed(analogRead(_random_seed) + device_id_seed);
        strategy.begin(device_id_seed);
        #if(INCLUDE_ASYNC_ACK)
          _packet_id_seed = random(65535) + device_id_seed;
        #endif
      };


      /* Compose packet in PJON format: */

      uint16_t compose_packet(
        const uint8_t id,
        const uint8_t *b_id,
        char *destination,
        const char *source,
        uint16_t length,
        uint16_t header = NOT_ASSIGNED,
        uint16_t p_id = 0
      ) {
        if(header == NOT_ASSIGNED) header = config;
        if(header > 255) header |= EXTEND_HEADER_BIT;
        if(length > 255) header |= (EXTEND_LENGTH_BIT | CRC_BIT);
        if(id == BROADCAST) header &= ~(ACK_REQUEST_BIT | ACK_MODE_BIT);
        uint16_t new_length = length + packet_overhead(header);
        bool extended_header = header & EXTEND_HEADER_BIT;
        bool extended_length = header & EXTEND_LENGTH_BIT;

        #if(INCLUDE_ASYNC_ACK)
          bool async_ack = (header & ACK_MODE_BIT) && (header & SENDER_INFO_BIT);
          if(!p_id && async_ack) p_id = new_packet_id();
        #endif

        if(new_length > 255 && !extended_length) {
          header |= (EXTEND_LENGTH_BIT | CRC_BIT);
          new_length = (uint16_t)(length + packet_overhead(header));
        }

        if(new_length >= PACKET_MAX_LENGTH) {
          _error(CONTENT_TOO_LONG, new_length);
          return 0;
        }

        destination[0] = id;
        if(extended_header) {
          destination[1] = (uint16_t)header;
          destination[2] = (uint16_t)header >> 8;
        } else destination[1] = header;
        if(extended_length) {
          destination[2 + extended_header] = new_length >> 8;
          destination[3 + extended_header] = new_length & 0xFF;
        } else destination[2 + extended_header] = new_length;
        if(header & MODE_BIT) {
          copy_bus_id((uint8_t*) &destination[3 + extended_header + extended_length], b_id);
          if(header & SENDER_INFO_BIT) {
            copy_bus_id((uint8_t*) &destination[7 + extended_header + extended_length], bus_id);
            destination[11 + extended_header + extended_length] = _device_id;
            #if(INCLUDE_ASYNC_ACK)
              if(async_ack) memcpy(destination + 12 + extended_header + extended_length, &p_id, 2);
            #endif
          }
        } else if(header & SENDER_INFO_BIT) {
          destination[3 + extended_header + extended_length] = _device_id;
          #if(INCLUDE_ASYNC_ACK)
            if(async_ack) memcpy(destination + 4 + extended_header + extended_length, &p_id, 2);
          #endif
        }

        memcpy(destination + (new_length - length - (header & CRC_BIT ? 4 : 1)), source, length);
        if(header & CRC_BIT) {
          uint32_t CRC = crc32::compute((uint8_t *)destination, new_length - 4);
          destination[new_length - 4] = (uint32_t)(CRC) >> 24;
          destination[new_length - 3] = (uint32_t)(CRC) >> 16;
          destination[new_length - 2] = (uint32_t)(CRC) >>  8;
          destination[new_length - 1] = (uint32_t)(CRC);
        } else destination[new_length - 1] = crc8::compute((uint8_t *)destination, new_length - 1);
        return new_length;
      };


      /* Get the device id, returning a single byte: */

      uint8_t device_id() const {
        return _device_id;
      };


      /* Add a packet to the send list ready to be delivered by the next update() call: */

      uint16_t dispatch(
        uint8_t id,
        const uint8_t *b_id,
        const char *packet,
        uint16_t length,
        uint32_t timing,
        uint16_t header = NOT_ASSIGNED,
        uint16_t p_id = 0
      ) {
        for(uint8_t i = 0; i < MAX_PACKETS; i++)
          if(packets[i].state == 0) {
            if(!(length = compose_packet(
              id, b_id, packets[i].content, packet, length, header, p_id
            ))) return FAIL;
            packets[i].length = length;
            packets[i].state = TO_BE_SENT;
            packets[i].registration = micros();
            packets[i].timing = timing;
            return i;
          }

        _error(PACKETS_BUFFER_FULL, MAX_PACKETS);
        return FAIL;
      };


      /* Get count of the packets for a device_id:
         Don't pass any parameter to count all packets
         Pass a device id to count all it's related packets */

      uint8_t get_packets_count(uint8_t device_id = NOT_ASSIGNED) const {
        uint8_t packets_count = 0;
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          if(packets[i].state == 0) continue;
          if(device_id == NOT_ASSIGNED || packets[i].content[0] == device_id) packets_count++;
        }
        return packets_count;
      };


      /* Generate a new packet id: */

      uint16_t new_packet_id() {
        _packet_id_seed += 1;
        if(!_packet_id_seed) _packet_id_seed = 1;
        return _packet_id_seed;
      };


      /* Calculate the packet's overhead: */

      uint8_t packet_overhead(uint16_t header = NOT_ASSIGNED) const {
        header = (header == NOT_ASSIGNED) ? config : header;
        return (
          (
            (header & MODE_BIT) ?
              (header & SENDER_INFO_BIT    ? 10 : 5) :
              (header & SENDER_INFO_BIT    ?  2 : 1)
          ) + (header & EXTEND_LENGTH_BIT  ?  2 : 1)
            + (header & EXTEND_HEADER_BIT  ?  2 : 1)
            + (header & CRC_BIT            ?  4 : 1)
            + (header & ACK_MODE_BIT       ?  2 : 0)
        );
      };


      /* Fill in a PacketInfo struct by parsing a packet: */

      void parse(const uint8_t *packet, PacketInfo &packet_info) const {
        packet_info.receiver_id = packet[0];
        bool extended_header = packet[1] & EXTEND_HEADER_BIT;
        bool extended_length = packet[1] & EXTEND_LENGTH_BIT;
        packet_info.header = (extended_header) ? packet[2] << 8 | packet[1] : packet[1];
        uint8_t offset = extended_header + extended_length;
        if((packet_info.header & MODE_BIT) != 0) {
          copy_bus_id(packet_info.receiver_bus_id, packet + 3 + offset);
          if((packet_info.header & SENDER_INFO_BIT) != 0) {
            copy_bus_id(packet_info.sender_bus_id, packet + 7 + offset);
            packet_info.sender_id = packet[11 + offset];
            #if(INCLUDE_ASYNC_ACK)
              if(packet_info.header & ACK_MODE_BIT)
                packet_info.id = packet[13 + offset] << 8 | packet[12 + offset] & 0xFF;
            #endif
          }
        } else if((packet_info.header & SENDER_INFO_BIT) != 0) {
          packet_info.sender_id = packet[3 + offset];
          #if(INCLUDE_ASYNC_ACK)
            if(packet_info.header & ACK_MODE_BIT)
              packet_info.id = packet[5 + offset] << 8 | packet[4 + offset] & 0xFF;
          #endif
        }
      };


      uint16_t receive() {
        uint16_t state;
        uint16_t length = PACKET_MAX_LENGTH;
        bool CRC = 0;
        bool extended_header = false;
        bool extended_length = false;
        for(uint16_t i = 0; i < length; i++) {
          data[i] = state = strategy.receive_byte();
          if(state == FAIL) return FAIL;

          if(i == 0)
            if(data[i] != _device_id && data[i] != BROADCAST && !_router)
              return BUSY;

          if(i == 1) {
            if(((data[i] & MODE_BIT) != (config & MODE_BIT)) && !_router)
              return BUSY;
            extended_length = data[i] & EXTEND_LENGTH_BIT;
            extended_header = data[i] & EXTEND_HEADER_BIT;
          }

          if((i == (2 + extended_header)) && !extended_length) {
            length = data[i];
            if(length < 5 || length > PACKET_MAX_LENGTH) return FAIL;
          }

          if((i == (3 + extended_header)) && extended_length) {
            length = data[i - 1] << 8 | data[i] & 0xFF;
            if(length < 5 || length > PACKET_MAX_LENGTH) return FAIL;
          }

          if((config & MODE_BIT) && (data[1] & MODE_BIT) && !_router)
            if((i > (2 + extended_header + extended_length)))
              if((i < (7 + extended_header + extended_length)))
                if(bus_id[i - 3 - extended_header - extended_length] != data[i])
                  return BUSY;
        }

        if(data[1] & CRC_BIT)
          CRC = crc32::compare(crc32::compute(data, length - 4), data + (length - 4));
        else CRC = !crc8::compute(data, length);

        if(data[1] & ACK_REQUEST_BIT && data[0] != BROADCAST)
          if(_mode != SIMPLEX && !_router)
            if(!(config & MODE_BIT) || (
              (config & MODE_BIT) && (data[1] & MODE_BIT) &&
              bus_id_equality(data + 3 + extended_length + extended_header, bus_id)
            )) strategy.send_response(!CRC ? NAK : ACK);

        if(!CRC) return NAK;
        parse(data, last_packet_info);

        #if(INCLUDE_ASYNC_ACK)
          /* If a packet requesting asynchronous acknowledment is received
             send the acknowledment packet back to the packet's transmitter */
          if((data[1] & ACK_MODE_BIT) && (data[1] & SENDER_INFO_BIT)) {
            if(_auto_delete && length == packet_overhead(data[1]))
              if(handle_asynchronous_acknowledgment(last_packet_info))
                return ACK;

            if(length > packet_overhead(data[1])) {
              dispatch(
                last_packet_info.sender_id,
                (uint8_t *)last_packet_info.sender_bus_id,
                NULL,
                0,
                0,
                config | ACK_MODE_BIT | SENDER_INFO_BIT,
                last_packet_info.id
              );
              update();
              if(known_packet_id(last_packet_info))
                return ACK;
            }
          }
        #endif

        _receiver(
          data + (packet_overhead(data[1]) - (data[1] & CRC_BIT ? 4 : 1)),
          length - packet_overhead(data[1]),
          last_packet_info
        );

        return ACK;
      };


      /* Try to receive a packet repeatedly with a maximum duration: */

      uint16_t receive(uint32_t duration) {
        uint16_t response;
        uint32_t time = micros();
        while((uint32_t)(micros() - time) <= duration) {
          response = receive();
          if(response == ACK)
            return ACK;
        }
        return response;
      };


      /* Remove a packet from the send list: */

      void remove(uint16_t index) {
        packets[index].attempts = 0;
        packets[index].length = 0;
        packets[index].registration = 0;
        packets[index].state = 0;
      };


      /* Remove a packet from the packet's buffer passing its id as reference: */

      boolean handle_asynchronous_acknowledgment(PacketInfo packet_info) {
        PacketInfo actual_info;
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          parse((uint8_t *)packets[i].content, actual_info);
          if(actual_info.id == packet_info.id)
            if(actual_info.receiver_id == packet_info.sender_id && (
              (!(actual_info.header & MODE_BIT) && !(packet_info.header & MODE_BIT)) ? true :
                bus_id_equality(actual_info.receiver_bus_id, packet_info.sender_bus_id)
            )) {
              if(packets[i].timing) {
                uint8_t offset = packet_overhead(actual_info.header);
                uint8_t crc_offset = ((actual_info.header & CRC_BIT) ? 4 : 1);
                dispatch(
                  actual_info.receiver_id,
                  (uint8_t *)actual_info.receiver_bus_id,
                  packets[i].content + (offset - crc_offset),
                  packets[i].length - offset,
                  packets[i].timing,
                  actual_info.header
                );
              }
              remove(i);
              return true;
            }
        }
        return false;
      };


      /* Remove all packets from the list:
         Don't pass any parameter to delete all packets
         Pass a device id to delete all it's related packets  */

      void remove_all_packets(uint8_t device_id = 0) {
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          if(packets[i].state == 0) continue;
          if(!device_id || packets[i].content[0] == device_id) remove(i);
        }
      };


      /* Send a packet to the sender of the last packet received.
         This function is typically called from with the receive
         callback function to deliver a response to a request. */

      uint16_t reply(const char *packet, uint16_t length, uint16_t header = NOT_ASSIGNED) {
        if(last_packet_info.sender_id != BROADCAST)
          return dispatch(
            last_packet_info.sender_id,
            last_packet_info.sender_bus_id,
            packet,
            length,
            0,
            header
          );
        return false;
      };


      /* Insert a packet in the send list: */

      uint16_t send(uint8_t id, const char *string, uint16_t length, uint16_t header = NOT_ASSIGNED) {
        return dispatch(id, bus_id, string, length, 0, header);
      };

      uint16_t send(
        uint8_t id,
        const uint8_t *b_id,
        const char *string,
        uint16_t length,
        uint16_t header = NOT_ASSIGNED
      ) {
        return dispatch(id, b_id, string, length, 0, header);
      };


      /* IMPORTANT: send_repeatedly timing maximum is 4293014170 microseconds or 71.55 minutes */
      uint16_t send_repeatedly(
        uint8_t id,
        const char *string,
        uint16_t length,
        uint32_t timing,
        uint16_t header = NOT_ASSIGNED
      ) {
        return dispatch(id, bus_id, string, length, timing, header);
      };

      /* IMPORTANT: send_repeatedly timing maximum is 4293014170 microseconds or 71.55 minutes */
      uint16_t send_repeatedly(
        uint8_t id,
        const uint8_t *b_id,
        const char *string,
        uint16_t length,
        uint32_t timing,
        uint16_t header = NOT_ASSIGNED
      ) {
        return dispatch(id, b_id, string, length, timing, header);
      };


      /* Send an already composed packet:  */

      uint16_t send_packet(const char *string, uint16_t length) {
        if(!string) return FAIL;
        if(_mode != SIMPLEX && !strategy.can_start()) return BUSY;
        strategy.send_string((uint8_t *)string, length);
        if(string[0] == BROADCAST || !(config & ACK_REQUEST_BIT) || _mode == SIMPLEX)
          return ACK;
        uint16_t response = strategy.receive_response();
        if(response == ACK || response == NAK || response == FAIL) return response;
        else return BUSY;
      };


      /* Compose and send a packet passing its info as parameters: */

      uint16_t send_packet(uint8_t id, char *string, uint16_t length, uint16_t header = NOT_ASSIGNED) {
        if(!(length = compose_packet(id, bus_id, (char *)data, string, length, header)))
          return FAIL;
        return send_packet((char *)data, length);
      };

      uint16_t send_packet(
        uint8_t id,
        const uint8_t *b_id,
        char *string,
        uint16_t length,
        uint16_t header = NOT_ASSIGNED
      ) {
        if(!(length = compose_packet(id, b_id, (char *)data, string, length, header)))
          return FAIL;
        return send_packet((char *)data, length);
      };


      /* Send a packet without using the send list. It is called send_packet_blocking
         because it tries to transmit a packet multiple times within an internal cycle
         until the packet is delivered, or timing limit is reached. */

      uint16_t send_packet_blocking(
        uint8_t id,
        const uint8_t *b_id,
        const char *string,
        uint16_t length,
        uint16_t header = NOT_ASSIGNED,
        uint32_t timeout = 3000000
      ) {
        if(!(length = compose_packet(
          id,
          b_id,
          (char *)data,
          string,
          length,
          header
        ))) return FAIL;
        uint16_t state = FAIL;
        uint32_t attempts = 0;
        uint32_t time = micros(), start = time;
        while(
          (state != ACK) && (attempts <= strategy.get_max_attempts()) &&
          (uint32_t)(micros() - start) <= timeout
        ) {
          state = send_packet((char*)data, length);
          if(state == ACK) return state;
          attempts++;
          if(state != FAIL) strategy.handle_collision();
          while((uint32_t)(micros() - time) < strategy.back_off(attempts));
          time = micros();
        }
        return state;
      };

      uint16_t send_packet_blocking(
        uint8_t id,
        const char *string,
        uint16_t length,
        uint16_t header = NOT_ASSIGNED
      ) {
        return send_packet_blocking(id, bus_id, string, length, header);
      };


      /* In router mode, the receiver function can ack for selected receiver
         device ids for which the route is known */

      void send_synchronous_acknowledge() {
        strategy.send_response(ACK);
      };


      /* Set the config bit state: */

      void set_config_bit(boolean new_state, uint16_t bit) {
        if(new_state) config |= bit;
        else config &= ~bit;
      };


      /* Configure synchronous acknowledge presence:
         TRUE: Send back synchronous acknowledge when a packet is correctly received
         FALSE: Avoid acknowledge transmission */

      void set_synchronous_acknowledge(boolean state) {
        set_config_bit(state, ACK_REQUEST_BIT);
      };


      /* Configure asynchronous acknowledge presence:
         TRUE: Send back asynchronous acknowledge packet
         FALSE: Avoid acknowledge packet transmission */

      void set_asynchronous_acknowledge(boolean state) {
        set_config_bit(state, ACK_MODE_BIT);
      };


      /* Configure CRC selected for packet checking:
         TRUE:  CRC32
         FALSE: CRC8 */

      void set_crc_32(boolean state) {
        set_config_bit(state, CRC_BIT);
      };


      /* Set communication mode: */

      void set_communication_mode(uint8_t mode) {
        _mode = mode;
      };


      /* Set bus state default configuration: */

      void set_default() {
        _mode = HALF_DUPLEX;
        if(!bus_id_equality(bus_id, localhost)) set_shared_network(true);
        set_error(dummy_error_handler);
        set_receiver(dummy_receiver_handler);
        for(int i = 0; i < MAX_PACKETS; i++) {
          packets[i].state = 0;
          packets[i].timing = 0;
          packets[i].attempts = 0;
        }
      };


      /* Pass as a parameter a void function you previously defined in your code.
         This will be called when an error in communication occurs

      void error_handler(uint8_t code, uint8_t data) {
        Serial.print(code);
        Serial.print(" ");
        Serial.println(data);
      };

      bus.set_error(error_handler); */

      void set_error(error e) {
        _error = e;
      };


      /* Set the device id, passing a single byte (watch out to id collision): */

      void set_id(uint8_t id) {
        _device_id = id;
      };


      /* Configure sender's information inclusion in the packet.
         TRUE: Includes 1 byte (device id) in local or 5 (bus id + device id) in shared
         FALSE: No inclusion (-1 byte overhead in local / -5 in shared)

         If you don't need the sender info disable the inclusion to reduce overhead and
         higher communication speed. */

      void include_sender_info(bool state) {
        set_config_bit(state, SENDER_INFO_BIT);
      };


      /* Configure the bus network behaviour.
         TRUE: Enable communication to devices part of other bus ids (on a shared medium).
         FALSE: Isolate communication from external/third-party communication. */

      void set_shared_network(boolean state) {
        set_config_bit(state, MODE_BIT);
      };


      /* Set if delivered or undeliverable packets are auto deleted:
         TRUE: Automatic deletion
         FALSE: No packet deletion from buffer. Manual packet deletion from buffer is needed.  */

      void set_packet_auto_deletion(boolean state) {
        _auto_delete = state;
      };


      /* Set the pin used as a seed for random generator: */

      void set_random_seed(uint8_t random_seed) {
        _random_seed = random_seed;
      };


      /* Pass as a parameter a void function you previously defined in your code.
         This will be called when a correct message will be received.
         Inside there you can code how to react when data is received.

        void receiver_function(uint8_t *payload, uint16_t length, const PacketInfo &packet_info) {
          for(int i = 0; i < length; i++)
            Serial.print((char)payload[i]);
          Serial.print(" ");
          Serial.println(length);
        };

        bus.set_receiver(receiver_function); */

      void set_receiver(receiver r) {
        _receiver = r;
      };


      /* Configure if device will act as a router:
         FALSE: device receives messages only for its bus and device id
         TRUE:  The receiver function is always called if data is received */

      void set_router(boolean state) {
        _router = state;
      };


      /* Update the state of the send list:
         Check if there are packets to be sent or to be erased if correctly delivered.
         Returns the actual number of packets to be sent. */

      uint8_t update() {
        uint8_t packets_count = 0;
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          if(packets[i].state == 0) continue;
          packets_count++;

          #if(ORDERED_SENDING)
            if(!first_packet_to_be_sent(i)) continue;
          #endif

          bool async_ack = (packets[i].content[1] & ACK_MODE_BIT) &&
            (packets[i].content[1] & SENDER_INFO_BIT);

          if(
            (uint32_t)(micros() - packets[i].registration) >
            (uint32_t)(packets[i].timing + strategy.back_off(packets[i].attempts))
          ) packets[i].state = send_packet(packets[i].content, packets[i].length);
          else continue;

          packets[i].attempts++;

          if(packets[i].state == ACK) {
            if(!packets[i].timing) {
              if(
                _auto_delete && (
                  (packets[i].length == packet_overhead(packets[i].content[1]) && async_ack
                ) || !(packets[i].content[1] & ACK_MODE_BIT))
              ) {
                remove(i);
                packets_count--;
              }
            } else {
              packets[i].attempts = 0;
              packets[i].registration = micros();
              packets[i].state = TO_BE_SENT;
            } if(!async_ack) continue;
          }

          if(packets[i].state != FAIL) strategy.handle_collision();

          if(packets[i].attempts > strategy.get_max_attempts()) {
            _error(CONNECTION_LOST, packets[i].content[0]);
            if(!packets[i].timing) {
              if(_auto_delete) {
                remove(i);
                packets_count--;
              }
            } else {
              packets[i].attempts = 0;
              packets[i].registration = micros();
              packets[i].state = TO_BE_SENT;
            }
          }
        }
        return packets_count;
      };


      /* Check if the packet index passed is the first to be sent: */

      boolean first_packet_to_be_sent(uint8_t index) {
        PacketInfo actual_info;
        PacketInfo tested_info;
        parse((uint8_t *)packets[index].content, actual_info);
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          parse((uint8_t *)packets[i].content, tested_info);
          if(
            actual_info.receiver_id == tested_info.receiver_id &&
            bus_id_equality(actual_info.receiver_bus_id, tested_info.receiver_bus_id)
          ) if(packets[i].registration < packets[index].registration) return false;
        }
        return true;
      };


      /* Check if the packet id and its transmitter info are already present in the
         buffer of recently received packets, if not add it to the buffer. */

      bool known_packet_id(PacketInfo info) {
        #if(INCLUDE_ASYNC_ACK)
          for(uint8_t i = 0; i < MAX_RECENT_PACKET_IDS; i++)
            if(
              info.id == recent_packet_ids[i].id &&
              info.sender_id == recent_packet_ids[i].sender_id &&
              ((
                (info.header & MODE_BIT) && (recent_packet_ids[i].header & MODE_BIT) &&
                bus_id_equality((uint8_t *)info.sender_bus_id, (uint8_t *)recent_packet_ids[i].sender_bus_id)
              ) || !(info.header & MODE_BIT) && !(recent_packet_ids[i].header & MODE_BIT))
            ) return true;

          save_packet_id(info);
          return false;
        #endif
      };


      /* Save packet id in the buffer: */

      void save_packet_id(PacketInfo info) {
        #if(INCLUDE_ASYNC_ACK)
          for(uint8_t i = MAX_RECENT_PACKET_IDS - 1; i > 0; i--)
            recent_packet_ids[i] = recent_packet_ids[i - 1];
          recent_packet_ids[0].id = info.id;
          recent_packet_ids[0].header = info.header;
          recent_packet_ids[0].sender_id = info.sender_id;
          copy_bus_id(recent_packet_ids[0].sender_bus_id, info.sender_bus_id);
        #endif
      };

      /* Check equality between two bus ids */

      static boolean bus_id_equality(const uint8_t *name_one, const uint8_t *name_two) {
        for(uint8_t i = 0; i < 4; i++)
          if(name_one[i] != name_two[i])
            return false;
        return true;
      };

      /* Copy a bus id: */

      static void copy_bus_id(uint8_t dest[], const uint8_t src[]) { memcpy(dest, src, 4); };
            
    private:
      boolean   _auto_delete = true;
      error     _error;
      uint8_t   _mode;
      uint16_t  _packet_id_seed = 0;
      uint8_t   _random_seed = A0;
      receiver  _receiver;
      boolean   _router = false;
    protected:
      uint8_t   _device_id;
  };
#endif
