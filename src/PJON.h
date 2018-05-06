
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ | ®
   |!y°o:\      |  __| |__| | \| v11.0
   |y"s§+`\     multi-master, multi-media bus network protocol
  /so+:-..`\    Copyright 2010-2018 by Giovanni Blu Mitolo gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\
  \+//u/+g%{osv,,\
    \=+&/osw+olds.\\
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
 ______-| |-__________________________________________________________________

For the PJON® Protocol specification see the specification directory.

PJON® Standard compliant tools:
- ModuleInterface - Easy config and value sync between IOT modules
  https://github.com/fredilarsen/ModuleInterface
- Command line PJON wrapper over unnamed pipes by Zbigniew Zasieczny
  https://github.com/Girgitt/PJON-piper
- PJON-python - PJON running on Python by Zbigniew Zasieczny
  https://github.com/Girgitt/PJON-python
- PJON-gRPC - gRPC server-client by Oleg Galitskiy
  https://github.com/Galitskiy/PJON-gRPC

Credits to contributors:
- Fred Larsen. Systems engineering, header driven communication, debugging
- Zbigniew Zasieczny. WINX86 interface
- Matheus Garbelini. ThroughLora strategy
- Wilfried Klaas ATtiny44/84 porting
- 4ib3r github user. Memory optimization configurable strategies inclusion
- budaics github user. ATtiny85 16MHz external clock testing and wiki page
- Pantovich github user. Update returning number of packets to be delivered
- Adrian Sławiński. Fix to enable SimpleModbusMasterV2 compatibility
- SticilFace github user. Teensy porting
- Esben Soeltoft. Arduino Zero porting
- Alex Grishin. ESP8266 porting
- Andrew Grande. Testing, support, bugfix
- Mauro Zancarlin. Systems engineering, testing, bugfix
- Michael Teeww. Callback based reception, debugging
- PaoloP74 github user. Library conversion to 1.x Arduino IDE

Bug reports:
- bryant1410 github user. Debug readme format
- pacproduct github user. Added missing mode configuration PJON_SIMPLEX ex.
- elusive-code github user. PJONMaster reset bug
- Franketto arduino forum user. PJON ThroughSerial over RS485 delay issue
- Zbigniew Zasieczny. Header reference inconsistency report
- DanRoad reddit user. can_start ThroughSerial bugfix
- Remo Kallio. Packet index 0 bugfix
- Emanuele Iannone. Forcing PJON_SIMPLEX in OverSamplingSimplex
- Christian Pointner. Fixed compiler warnings
- Andrew Grande. ESP8266 example watchdog error bug fix
- Fabian Gärtner. receive function and big packets bugfix
- Mauro Mombelli. Code cleanup
- Shachar Limor. Blink example pinMode bugfix

If you believe in this project and you appreciate our work, please, make a
donation. The PJON Foundation is entirely financed by contributions of wise
people like you and its resources are solely invested to cover the development
and maintainance costs.
- Paypal:   https://www.paypal.me/PJON
- Bitcoin:  1FupxAyDTuAMGz33PtwnhwBm4ppc7VLwpD
- Ethereum: 0xf34AEAF3B149454522019781668F9a2d1762559b
Thank you and happy tinkering!
 _____________________________________________________________________________

Copyright 2010-2018 by Giovanni Blu Mitolo gioscarab@gmail.com

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
#include <interfaces/PJON_Interfaces.h>
#include <PJONDefines.h>
#include <strategies/PJON_Strategies.h>

template<typename Strategy>
class PJON {
  public:
    Strategy strategy;
    uint16_t config = PJON_TX_INFO_BIT | PJON_ACK_REQ_BIT;
    uint8_t bus_id[4] = {0, 0, 0, 0};
    const uint8_t localhost[4] = {0, 0, 0, 0};
    uint8_t data[PJON_PACKET_MAX_LENGTH];
    PJON_Packet_Info last_packet_info;
    PJON_Packet packets[PJON_MAX_PACKETS];
    uint16_t port = PJON_BROADCAST;
    uint8_t random_seed = A0;

    #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
      PJON_Packet_Record recent_packet_ids[PJON_MAX_RECENT_PACKET_IDS];
    #endif

    /* PJON bus default initialization:
       State: Local (bus_id: 0.0.0.0)
       Acknowledge: true (Acknowledge is requested)
       device id: PJON_NOT_ASSIGNED (255)
       Mode: PJON_HALF_DUPLEX
       Sender info: true (Sender info are included in the packet)
       Strategy: SoftwareBitBang */

    PJON() : strategy(Strategy()) {
      _device_id = PJON_NOT_ASSIGNED;
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
      const uint8_t id,
      const uint8_t *b_id,
      char *destination,
      const char *source,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      uint8_t index = 0;
      if(header == PJON_FAIL) header = config;
      if(length > 255) header |= PJON_EXT_LEN_BIT;
      if((config & PJON_PORT_BIT) || (requested_port != PJON_BROADCAST))
        header |= PJON_PORT_BIT;

      if(
        (header & PJON_PORT_BIT) &&
        (requested_port == PJON_BROADCAST) &&
        (port == PJON_BROADCAST)
      ) header &= ~PJON_PORT_BIT;

      if(id == PJON_BROADCAST)
        header &= ~(PJON_ACK_REQ_BIT | PJON_ACK_MODE_BIT);
      uint16_t new_length = length + packet_overhead(header);
      bool extended_length = header & PJON_EXT_LEN_BIT;

      #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
        bool add_packet_id =
          ((header & PJON_ACK_MODE_BIT) && (header & PJON_TX_INFO_BIT)) ||
          (header & PJON_PACKET_ID_BIT);
        if(!p_id && add_packet_id) p_id = new_packet_id();
      #else
        (void)p_id; // Avoid unused variable compiler warning
      #endif

      if(new_length > 15 && !(header & PJON_CRC_BIT)) {
        header |= PJON_CRC_BIT;
        new_length = (uint16_t)(length + packet_overhead(header));
      }

      if(new_length > 255 && !extended_length) {
        header |= PJON_EXT_LEN_BIT;
        new_length = (uint16_t)(length + packet_overhead(header));
      }

      if(new_length >= PJON_PACKET_MAX_LENGTH) {
        _error(PJON_CONTENT_TOO_LONG, new_length, _custom_pointer);
        return 0;
      }

      destination[index++] = id;
      destination[index++] = (uint8_t)header;
      if(extended_length) {
        destination[index++] = (uint8_t)(new_length >> 8);
        destination[index++] = (uint8_t)new_length;
        destination[index++] = PJON_crc8::compute((uint8_t *)destination, 4);
      } else {
        destination[index++] = (uint8_t)new_length;
        destination[index++] = PJON_crc8::compute((uint8_t *)destination, 3);
      }
      if(header & PJON_MODE_BIT) {
        copy_bus_id((uint8_t*) &destination[index], b_id);
        index += 4;
        if(header & PJON_TX_INFO_BIT) {
          copy_bus_id((uint8_t*) &destination[index], bus_id);
          index += 4;
        }
      }
      if(header & PJON_TX_INFO_BIT) destination[index++] = _device_id;

      #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
        if(add_packet_id) {
          destination[index++] = (uint8_t)(p_id >> 8);
          destination[index++] = (uint8_t)p_id;
        }
      #endif

      if(header & PJON_PORT_BIT) {
        if(requested_port != PJON_BROADCAST) {
          destination[index++] = (uint8_t)(requested_port >> 8);
          destination[index++] = (uint8_t)requested_port;
        } else if(port != PJON_BROADCAST) {
          destination[index++] = (uint8_t)(port >> 8);
          destination[index++] = (uint8_t)port;
        }
      }

      memcpy(
        destination + (new_length - length - (header & PJON_CRC_BIT ? 4 : 1)),
        source,
        length
      );

      if(header & PJON_CRC_BIT) {
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

    /* Get device id: */

    uint8_t device_id() const {
      return _device_id;
    };

    /* Add packet to buffer (delivery attempt by the next update() call): */

    uint16_t dispatch(
      uint8_t id,
      const uint8_t *b_id,
      const char *packet,
      uint16_t length,
      uint32_t timing,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST,
      uint16_t p_index = PJON_FAIL
    ) {
      bool req_index = (p_index != PJON_FAIL);
      for(uint16_t i = ((req_index) ? p_index : 0); i < PJON_MAX_PACKETS; i++)
        if(packets[i].state == 0 || req_index) {
          if(!(length = compose_packet(
            id,
            b_id,
            packets[i].content,
            packet,
            length,
            header,
            p_id,
            requested_port
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
              bus_id_equality(
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

    /* Generate a new packet id: */

    uint16_t new_packet_id() {
      _packet_id_seed += 1;
      if(!_packet_id_seed) _packet_id_seed = 1;
      return _packet_id_seed;
    };

    /* Calculate packet overhead: */

    uint8_t packet_overhead(uint16_t header = PJON_FAIL) const {
      header = (header == PJON_FAIL) ? config : header;
      return (
        (
          (header & PJON_MODE_BIT) ?
            (header & PJON_TX_INFO_BIT   ? 10 : 5) :
            (header & PJON_TX_INFO_BIT   ?  2 : 1)
        ) + (header & PJON_EXT_LEN_BIT   ?  2 : 1)
          + (header & PJON_CRC_BIT       ?  4 : 1)
          + (header & PJON_PORT_BIT      ?  2 : 0)
          + (
              (
                (header & PJON_ACK_MODE_BIT) ||
                (header & PJON_PACKET_ID_BIT)
              ) ? 2 : 0
            )
          + 1 // Header
          + 1 // Header CRC
      );
    };

    /* Fill a PJON_Packet_Info struct with data parsing a packet: */

    void parse(const uint8_t *packet, PJON_Packet_Info &packet_info) const {
      memset(&packet_info, 0, sizeof packet_info);
      uint8_t index = 0;
      packet_info.receiver_id = packet[index++];
      bool extended_length = packet[index] & PJON_EXT_LEN_BIT;
      packet_info.header = packet[index++];
      index += extended_length + 2; // + LENGTH + HEADER CRC
      if(packet_info.header & PJON_MODE_BIT) {
        copy_bus_id(packet_info.receiver_bus_id, packet + index);
        index += 4;
        if(packet_info.header & PJON_TX_INFO_BIT) {
          copy_bus_id(packet_info.sender_bus_id, packet + index);
          index += 4;
        }
      }
      if(packet_info.header & PJON_TX_INFO_BIT)
        packet_info.sender_id = packet[index++];
      #if(PJON_INCLUDE_ASYNC_ACK || PJON_INCLUDE_PACKET_ID)
        if(((packet_info.header & PJON_ACK_MODE_BIT) &&
            (packet_info.header & PJON_TX_INFO_BIT)
          ) || packet_info.header & PJON_PACKET_ID_BIT
        ) {
          packet_info.id =
            (packet[index] << 8) | (packet[index + 1] & 0xFF);
          index += 2;
        }
      #endif
      if(packet_info.header & PJON_PORT_BIT)
        packet_info.port = (packet[index] << 8) | (packet[index + 1] & 0xFF);
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
          batch_length = strategy.receive_string(data + i, length - i);
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
              ((data[1] & PJON_MODE_BIT) != (config & PJON_MODE_BIT))
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

        if((i >= 1) && (data[1] & PJON_MODE_BIT) && !_router)
          if(
            (i > (uint8_t)(3 + extended_length)) &&
            (i < (uint8_t)(8 + extended_length))
          ) if(bus_id[i - 4 - extended_length] != data[i])
              return PJON_BUSY;
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
        if(filter && known_packet_id(last_packet_info))
          return PJON_ACK;
      #endif

      if((port != PJON_BROADCAST) && (port != last_packet_info.port))
        return PJON_BUSY;

      _receiver(
        data + (overhead - (data[1] & PJON_CRC_BIT ? 4 : 1)),
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
              bus_id_equality(
                actual_info.receiver_bus_id,
                packet_info.sender_bus_id
              )
          )) {
            if(packets[i].timing) {
              uint8_t offset = packet_overhead(actual_info.header);
              uint8_t crc_offset =
                ((actual_info.header & PJON_CRC_BIT) ? 4 : 1);
              dispatch(
                actual_info.receiver_id,
                (uint8_t *)actual_info.receiver_bus_id,
                packets[i].content + (offset - crc_offset),
                packets[i].length - offset,
                packets[i].timing,
                actual_info.header,
                new_packet_id(),
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
      const char *packet,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      if(last_packet_info.sender_id != PJON_BROADCAST)
        return dispatch(
          last_packet_info.sender_id,
          last_packet_info.sender_bus_id,
          packet,
          length,
          0,
          header,
          p_id,
          requested_port
        );
      return false;
    };

    /* Schedule a packet sending: */

    uint16_t send(
      uint8_t id,
      const char *string,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      return dispatch(
        id, bus_id, string, length, 0, header, p_id, requested_port
      );
    };

    uint16_t send(
      uint8_t id,
      const uint8_t *b_id,
      const char *string,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      return dispatch(
        id, b_id, string, length, 0, header, p_id, requested_port
      );
    };

    /* Schedule a packet sending configuring its sender info:  */

    uint16_t send_from_id(
      uint8_t sender_id,
      const uint8_t *sender_bus_id,
      uint8_t id,
      const uint8_t *b_id,
      const char *string,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      uint8_t original_device_id = _device_id;
      uint8_t original_bus_id[4];
      copy_bus_id(original_bus_id, bus_id);
      set_id(sender_id);
      copy_bus_id(bus_id, sender_bus_id);
      uint16_t result = PJON_FAIL;
      #if(PJON_MAX_PACKETS > 0)
        result = dispatch(
          id, b_id, string, length, 0, header, p_id, requested_port
        );
      #endif
      if(result == PJON_FAIL)
        result = send_packet_blocking(
          id, b_id, string, length, header, p_id, requested_port
        );
      copy_bus_id(bus_id, original_bus_id);
      set_id(original_device_id);
      return result;
    };

    /* IMPORTANT: send_repeatedly timing maximum
       is 4293014170 microseconds or 71.55 minutes */

    uint16_t send_repeatedly(
      uint8_t id,
      const char *string,
      uint16_t length,
      uint32_t timing,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      return dispatch(
        id, bus_id, string, length, timing, header, p_id, requested_port
      );
    };

    /* IMPORTANT: send_repeatedly timing maximum
       is 4293014170 microseconds or 71.55 minutes */

    uint16_t send_repeatedly(
      uint8_t id,
      const uint8_t *b_id,
      const char *string,
      uint16_t length,
      uint32_t timing,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      return dispatch(
        id, b_id, string, length, timing, header, p_id, requested_port
      );
    };

    /* Transmit an already composed packet:  */

    uint16_t send_packet(const char *string, uint16_t length) {
      if(!string) return PJON_FAIL;
      if(_mode != PJON_SIMPLEX && !strategy.can_start()) return PJON_BUSY;
      strategy.send_string((uint8_t *)string, length);
      if(
        string[0] == PJON_BROADCAST ||
        !(string[1] & PJON_ACK_REQ_BIT) ||
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
      uint8_t id,
      char *string,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      if(!(length = compose_packet(
        id, bus_id, (char *)data, string, length, header, p_id, requested_port
      ))) return PJON_FAIL;
      return send_packet((char *)data, length);
    };

    uint16_t send_packet(
      uint8_t id,
      const uint8_t *b_id,
      char *string,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST
    ) {
      if(!(length = compose_packet(
        id, b_id, (char *)data, string, length, header, p_id, requested_port
      ))) return PJON_FAIL;
      return send_packet((char *)data, length);
    };

    /* Transmit a packet without using the packet's buffer. Tries to transmit
       a packet multiple times within an internal cycle until the packet is
       delivered, or timing limit is reached. */

    uint16_t send_packet_blocking(
      uint8_t id,
      const uint8_t *b_id,
      const char *string,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST,
      uint32_t timeout = 3000000
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
          id,
          b_id,
          (char *)data,
          string,
          old_length,
          header,
          p_id,
          requested_port
        ))) return PJON_FAIL;
        state = send_packet((char*)data, length);
        if(state == PJON_ACK) {
          _recursion--;
          return state;
        }
        attempts++;
        if(state != PJON_FAIL) strategy.handle_collision();
        if(_recursion <= 1) receive(strategy.back_off(attempts));
        else PJON_DELAY_MICROSECONDS(strategy.back_off(attempts));
      }
      _recursion--;
      return state;
    };

    uint16_t send_packet_blocking(
      uint8_t id,
      const char *string,
      uint16_t length,
      uint16_t header = PJON_FAIL,
      uint16_t p_id = 0,
      uint16_t requested_port = PJON_BROADCAST,
      uint32_t timeout = 3000000
    ) {
      return send_packet_blocking(
        id, bus_id, string, length, header, p_id, requested_port, timeout
      );
    };

    /* In router mode, the receiver function can acknowledge
       for selected receiver device ids for which the route is known */

    void send_synchronous_acknowledge() {
      strategy.send_response(PJON_ACK);
    };

    /* Set the config bit state: */

    void set_config_bit(bool new_state, uint16_t bit) {
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
       TRUE: include packet id
       FALSE: Avoid packet id inclusion */

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
      if(!bus_id_equality(bus_id, localhost)) set_shared_network(true);
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

    /* Pass as a parameter a function you previously defined in your
       code that is called when a message is received.
       Inside there you can code how to react when data is received.

      void receiver_function(
        uint8_t *payload,
        uint16_t length,
        const PJON_Packet_Info &packet_info
      ) {
        for(int i = 0; i < length; i++)
          Serial.print((char)payload[i]);
        Serial.print(" ");
        Serial.println(length);
      };

      bus.set_receiver(receiver_function); */

    void set_receiver(PJON_Receiver r) {
      _receiver = r;
    };

    /* Configure if device acts as a router:
       FALSE: device receives messages only for its bus and device id
       TRUE:  The receiver function is always called if data is received */

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
          if(!(sync_ack && async_ack && packets[i].state == PJON_ACK))
            packets[i].state = // Avoid resending sync-acked async ack packets
              send_packet(packets[i].content, packets[i].length);
        } else continue;

        packets[i].attempts++;

        if(packets[i].state == PJON_ACK) {
          if(!packets[i].timing) {
            if(
              _auto_delete && (
                (packets[i].length == packet_overhead(packets[i].content[1])
              && async_ack ) || !(packets[i].content[1] & PJON_ACK_MODE_BIT))
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
                bus_id_equality(
                  (uint8_t *)info.sender_bus_id,
                  (uint8_t *)recent_packet_ids[i].sender_bus_id
                )
              ) ||
              !(info.header & PJON_MODE_BIT) &&
              !(recent_packet_ids[i].header & PJON_MODE_BIT)
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
        copy_bus_id(recent_packet_ids[0].sender_bus_id, info.sender_bus_id);
      #endif
    };

    /* Check equality between two bus ids */

    static bool bus_id_equality(const uint8_t *n_one, const uint8_t *n_two) {
      for(uint8_t i = 0; i < 4; i++)
        if(n_one[i] != n_two[i])
          return false;
      return true;
    };

    /* Copy a bus id: */

    static void copy_bus_id(uint8_t dest[], const uint8_t src[]) {
      memcpy(dest, src, 4);
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
