
 /*-O//\             __     __
   |-gfo\           |__| | |  | |\ |
   |!y°o:\          |  __| |__| | \| v5.0 beta
   |y"s§+`\         multi-master, multi-media communications bus system framework
  /so+:-..`\        Copyright 2010-2016 by Giovanni Blu Mitolo gioscarab@gmail.com
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
- DanRoad reddit user (can_start ThroughSerial bugfix)
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

PJON is a self-funded, no-profit project created and mantained by Giovanni Blu Mitolo
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

#ifndef PJON_h
  #define PJON_h
  #include <Arduino.h>
  #include <PJONDefines.h>

  #include "strategies/OverSampling/OverSampling.h"
  #include "strategies/SoftwareBitBang/SoftwareBitBang.h"
  #include "strategies/ThroughSerial/ThroughSerial.h"

  template<typename Strategy = SoftwareBitBang>
  class PJON {
    public:
      Strategy strategy;

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


      /* Initial random delay to avoid startup collision */

      void begin() {
        randomSeed(analogRead(A0));
        delay(random(0, INITIAL_MAX_DELAY));
      };


      /* Compose packet in PJON format: */

      uint8_t compose_packet(
        const uint8_t id,
        const uint8_t *b_id,
        char *destination,
        const char *source,
        uint8_t length,
        uint8_t header = FROM_CONFIG
      ) const {
        if(header == FROM_CONFIG) header = get_header();
        uint8_t new_length = length + packet_overhead(header);

        if(new_length >= PACKET_MAX_LENGTH) {
          _error(CONTENT_TOO_LONG, new_length);
          return 0;
        }

        destination[0] = id;
        destination[1] = new_length;
        destination[2] = header;

        if(_shared) {
          copy_bus_id((uint8_t*) &destination[3], b_id);
          if(_sender_info) {
            copy_bus_id((uint8_t*) &destination[7], bus_id);
            destination[11] = _device_id;
          }
        } else if(_sender_info) destination[3] = _device_id;

        memcpy(destination + (packet_overhead() - 1), source, length);
        destination[new_length - 1] = compute_crc_8((uint8_t *)destination, new_length - 1);
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
        uint8_t length,
        uint32_t timing,
        uint8_t header = FROM_CONFIG
      ) {
         for(uint8_t i = 0; i < MAX_PACKETS; i++)
          if(packets[i].state == 0) {
            if(!(length = compose_packet(id, b_id, packets[i].content, packet, length, header)))
              return FAIL;
            packets[i].length = length;
            packets[i].state = TO_BE_SENT;
            packets[i].registration = micros();
            packets[i].timing = timing;
            return i;
          }

        _error(PACKETS_BUFFER_FULL, MAX_PACKETS);
        return FAIL;
      };


      /* Return the header byte based on current configuration: */

      uint8_t get_header() const {
        // Compose PJON 1 byte header from internal configuration
        return (_shared ? MODE_BIT : 0) |
               (_sender_info ? SENDER_INFO_BIT : 0) |
               (_acknowledge ? ACK_REQUEST_BIT : 0);
      };


      /* Fill in a PacketInfo struct by parsing a packet: */

      void get_packet_info(const uint8_t *packet, PacketInfo &packet_info) const {
        packet_info.receiver_id = packet[0];
        packet_info.header = packet[2];

        if((packet_info.header & MODE_BIT) != 0) {
          copy_bus_id(packet_info.receiver_bus_id, packet + 3);
          if((packet_info.header & SENDER_INFO_BIT) != 0) {
            copy_bus_id(packet_info.sender_bus_id, packet + 7);
            packet_info.sender_id = packet[11];
          }
        } else if((packet_info.header & SENDER_INFO_BIT) != 0) packet_info.sender_id = packet[3];
      };


      /* Get count of the packets for a device_id:
         Don't pass any parameter to count all packets
         Pass a device id to count all it's related packets */

      uint8_t get_packets_count(uint8_t device_id = 0) const {
        uint8_t packets_count = 0;
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          if(packets[i].state == 0) continue;
          if(!device_id || packets[i].content[0] == device_id) packets_count++;
        }
        return packets_count;
      };


      /* Calculate the packet's overhead: */

      uint8_t packet_overhead(uint8_t header) const {
        return (header & MODE_BIT) ?
        (header & SENDER_INFO_BIT ? 13 : 8) :
        (header & SENDER_INFO_BIT ? 5 : 4);
      };


      /* Try to receive a packet: */

      uint16_t receive() {
        uint16_t state;
        uint16_t packet_length = PACKET_MAX_LENGTH;
        uint8_t CRC = 0;
        bool shared = false;
        bool includes_sender_info = false;
        bool acknowledge_requested = false;

        for(uint8_t i = 0; i < packet_length; i++) {
          data[i] = state = strategy.receive_byte();
          if(state == FAIL) return FAIL;

          if(i == 0 && data[i] != _device_id && data[i] != BROADCAST && !_router)
            return BUSY;

          if(i == 1) {
            if(data[i] > 4 && data[i] < PACKET_MAX_LENGTH)
              packet_length = data[i];
            else return FAIL;
          }

          if(i == 2) { // Packet header
            shared = data[2] & MODE_BIT;
            includes_sender_info = data[2] & SENDER_INFO_BIT;
            acknowledge_requested = data[2] & ACK_REQUEST_BIT;
            if((shared != _shared) && !_router) return BUSY; // Keep private and shared buses apart
          }

          /* If an id is assigned to this bus it means that is potentially
             sharing its medium, or the device could be connected in parallel
             with other buses. Bus id equality is checked to avoid collision
             i.e. id 1 bus 1, should not receive a message for id 1 bus 2. */

          if(_shared && shared && !_router && i > 2 && i < 7)
            if(bus_id[i - 3] != data[i]) return BUSY;

          CRC = roll_crc_8(data[i], CRC);
        }
        if(!CRC) {
          if(acknowledge_requested && data[0] != BROADCAST && _mode != SIMPLEX)
            if(!_shared || (_shared && shared && bus_id_equality(data + 3, bus_id)))
              strategy.send_response(ACK);

          get_packet_info(data, last_packet_info);
          uint8_t payload_offset = 3 + (shared ? (includes_sender_info ? 9 : 4) : (includes_sender_info ? 1 : 0));
          _receiver(data + payload_offset, data[1] - payload_offset - 1, last_packet_info);
          return ACK;
        } else {
          if(acknowledge_requested && data[0] != BROADCAST && _mode != SIMPLEX)
            if(!_shared || (_shared && shared && bus_id_equality(data + 3, bus_id)))
              strategy.send_response(NAK);
          return NAK;
        }
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

      void remove(uint16_t id) {
        packets[id].attempts = 0;
        packets[id].length = 0;
        packets[id].registration = 0;
        packets[id].state = 0;
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

      uint16_t reply(const char *packet, uint8_t length, uint8_t header = FROM_CONFIG) {
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


      /* Insert a packet in the send list:
       The added packet will be sent in the next update() call.
       Using the timing parameter you can set the delay between every
       transmission cyclically sending the packet (use remove() function stop it)

       LOCAL TRANSMISSION -> ISOLATED BUS

       int hi = bus.send(99, "HI!", 3);
       // Send hi once to device 99

       int hi = bus.send_repeatedly(99, "HI!", 3, 1000000);
       // Send HI! to device 99 every second (1.000.000 microseconds)

       NETWORK TRANSMISSION -> SHARED MEDIUM

       int hi = bus.send(99, {127, 0, 0, 1}, 3);
       // Send hi once to device 99 on bus id 127.0.0.1

       int hi = bus.send_repeatedly(99, {127, 0, 0, 1}, "HI!", 3, 1000000);
       // Send HI! to device 99 on bus id 127.0.0.1 every second (1.000.000 microseconds)

       bus.remove(hi); // Stop repeated sending */

      uint16_t send(uint8_t id, const char *string, uint8_t length, uint8_t header = FROM_CONFIG) {
        return dispatch(id, bus_id, string, length, 0, header);
      };

      uint16_t send(
        uint8_t id,
        const uint8_t *b_id,
        const char *string,
        uint8_t length,
        uint8_t header = FROM_CONFIG
      ) {
        return dispatch(id, b_id, string, length, 0, header);
      };

      /* IMPORTANT: send_repeatedly timing parameter maximum is 4294 microseconds or 71.56 minutes */
      uint16_t send_repeatedly(
        uint8_t id,
        const char *string,
        uint8_t length,
        uint32_t timing,
        uint8_t header = FROM_CONFIG
      ) {
        return dispatch(id, bus_id, string, length, timing, header);
      };

      /* IMPORTANT: send_repeatedly timing parameter maximum is 4294 microseconds or 71.56 minutes */
      uint16_t send_repeatedly(
        uint8_t id,
        const uint8_t *b_id,
        const char *string,
        uint8_t length,
        uint32_t timing,
        uint8_t header = FROM_CONFIG
      ) {
        return dispatch(id, b_id, string, length, timing, header);
      };


  /* An Example of how the packet "@" is formatted and sent:

  RECIPIENT ID 12   LENGTH 6          HEADER 00000110  SENDER ID 11      CONTENT 64       CRC
   ________________ _________________ _________________ _________________ ________________ __________________
  |Sync | Byte     |Sync | Byte      |Sync | Byte      |Sync | Byte      |Sync | Byte     |Sync | Byte       |
  |___  |     __   |___  |      _   _|___  |      _ _  |___  |     _   __|___  |  _       |___  |  _      _  |
  |   | |    |  |  |   | |     | | | |   | |     | | | |   | |    | | |  |   | | | |      |   | | | |    | | |
  | 1 |0|0000|11|00| 1 |0|00000|1|0|1| 1 |0|00000|1|1|0| 1 |0|0000|1|0|11| 1 |0|0|1|000000| 1 |0|0|1|0000|1|0|
  |___|_|____|__|__|___|_|_____|_|_|_|___|_|_____|_|_|_|___|_|____|_|_|__|___|_|_|_|______|___|_|_|_|____|_|_|

  A standard packet transmission is a bidirectional communication between
  two devices that can be divided in 3 different phases:

  Channel analysis   Transmission                                                 Response
      _____           ____________________________________________________         _____
     | C-A |         | ID | LENGTH | HEADER |  SENDER ID  | CONTENT | CRC |       | ACK |
  <--|-----|---< >---|----|--------|--------|-------------|---------|-----|--> <--|-----|
     |  0  |         | 12 |   5    |  001   |    ID 11    |   64    |     |       |  6  |
     |_____|         |____|________|________|_____________|_________|_____|       |_____|

  DEFAULT HEADER CONFIGURATION:
  00000110: Acknowledge requested | Sender info included | Local bus

  BUS CONFIGURATION:
  bus.set_acknowledge(true);
  bus.include_sender_info(true);

  Average overhead, average bandwidth availability setup. Can be used only in an isolated
  medium (i.e. isolated wire) and with up to 254 devices with transmission certainty through
  synchronous acknowledge, and sender info to easy reply to packets with the reply() function
  __________________________________________________________________________________________

  A local packet transmission handled in SIMPLEX mode is a monodirectional communication
  between two devices dispatched in a single phase:

     Transmission
      ______________________________________
     | ID | LENGTH | HEADER | CONTENT | CRC |
  >--|----|--------|--------|---------|-----|-->
     | 12 |   5    |  000   |   64    |     |
     |____|________|________|_________|_____|

  HEADER CONFIGURATION:
  00000000: Acknowledge not requested | Sender info not included | Local bus

  BUS CONFIGURATION:
  bus.set_acknowledge(false);
  bus.include_sender_info(false);

  Low overhead, high bandwidth availability setup. Can be used only in an isolated
  medium (i.e. isolated wire) and with up to 254 devices.
  _________________________________________________________________________________________

  A Shared packet transmission example handled in HALF_DUPLEX mode, with acknowledge
  request, including the sender info:

 Channel analysis                         Transmission                                      Response
    _____         __________________________________________________________________         _____
   | C-A |       | ID | LENGTH | HEADER |    BUS ID   | BUS ID | ID | CONTENT | CRC |       | ACK |
 <-|-----|--< >--|----|--------|--------|-------------|--------|----|---------|-----|--> <--|-----|
   |  0  |       | 12 |   5    |  111   |     0001    |  0001  | 11 |   64    |     |       |  6  |
   |_____|       |____|________|________|_____________|________|____|_________|_____|       |_____|
                                        |Receiver info| Sender info |
  HEADER CONFIGURATION:
  00000111: Acknowledge requested | Sender info included | Shared bus

  BUS CONFIGURATION:
  bus.set_acknowledge(true);
  bus.include_sender_info(true);

  High overhead, low bandwidth availability setup. Can be used sharing the medium
  with many other buses with transmission certainty through synchronous acknowledge
  and sender info to easy reply to packets with the reply() function. */

      uint16_t send_packet(const char *string, uint8_t length) {
        if(!string) return FAIL;
        if(_mode != SIMPLEX && !strategy.can_start()) return BUSY;
        strategy.send_string((uint8_t *)string, length);
        if(string[0] == BROADCAST || !_acknowledge || _mode == SIMPLEX) return ACK;
        uint16_t response = strategy.receive_response();
        if(response == ACK || response == NAK || response == FAIL) return response;
        else return BUSY;
      };


      /* Send a packet passing its info as parameters: */

      uint16_t send_packet(uint8_t id, char *string, uint8_t length, uint8_t header = FROM_CONFIG) {
        if(!(length = compose_packet(id, bus_id, (char *)data, string, length, header)))
          return FAIL;
        return send_packet((char *)data, length);
      };


      uint16_t send_packet(
        uint8_t id,
        const uint8_t *b_id,
        char *string,
        uint8_t length,
        uint8_t header = FROM_CONFIG
      ) {
        if(!(length = compose_packet(id, b_id, (char *)data, string, length, header)))
          return FAIL;
        return send_packet((char *)data, length);
      };


      /* Send a packet without using the send list. It is called send_packet_blocking
         because the code execution is stuck inside this function until the packet is
         delivered or the timeout is reached: */

      uint16_t send_packet_blocking(
        uint8_t id,
        const uint8_t *b_id,
        const char *string,
        uint8_t length,
        uint32_t timeout,
        uint8_t header = 0
      ) {
        if(!(length = compose_packet(id, bus_id, (char *)data, string, length, header))) return FAIL;
        uint16_t status = FAIL;
        uint32_t attempts = 0;
        uint32_t time = micros();
        while(status != ACK && (uint32_t)(micros() - time) < timeout) {
          status = send_packet((char*)data, length);
          if(status == ACK) return status;
          attempts++;
          delayMicroseconds(attempts *attempts *attempts);
        }
        return status;
      };

      uint16_t send_packet_blocking(uint8_t id, const char *string, uint8_t length, uint32_t timeout, uint8_t header = 0) {
        return send_packet_blocking(id, bus_id, string, length, timeout, header);
      };


      /* In router mode, the receiver function can ack for selected receiver
         device ids for which the route is known */

      void send_acknowledge() {
        strategy.send_response(ACK);
      };


      /* Configure synchronous acknowledge presence:
         TRUE: Send back synchronous acknowledge when a packet is correctly received
         FALSE: Avoid acknowledge transmission */

      void set_acknowledge(boolean state) {
        _acknowledge = state;
      };


      /* Set communication mode: */

      void set_communication_mode(uint8_t mode) {
        _mode = mode;
      };


      /* Set bus state default configuration: */

      void set_default() {
        _mode = HALF_DUPLEX;

        if(!bus_id_equality(bus_id, localhost))
          _shared = true;

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
        _sender_info = state;
      };


      /* Configure the bus network behaviour.
         TRUE: Enable communication to devices part of other bus ids (on a shared medium).
         FALSE: Isolate communication from external/third-party communication. */

      void set_shared_network(boolean state) {
        _shared = state;
      }


      /* Set if delivered or undeliverable packets are auto deleted:
         TRUE: Automatic deletion
         FALSE: No packet deletion from buffer. Manual packet deletion from buffer is needed.  */

      void set_packet_auto_deletion(boolean state) {
        _auto_delete = state;
      };


      /* Pass as a parameter a void function you previously defined in your code.
         This will be called when a correct message will be received.
         Inside there you can code how to react when data is received.

        void receiver_function(uint8_t *payload, uint8_t length, const PacketInfo &packet_info) {
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
        uint32_t back_off;
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          if(packets[i].state == 0) continue;
          packets_count++;
          back_off = packets[i].attempts;
          back_off = back_off * back_off * back_off;
          if((uint32_t)(micros() - packets[i].registration) - back_off > packets[i].timing)
            packets[i].state = send_packet(packets[i].content, packets[i].length);
          else continue;

          if(packets[i].state == ACK) {
            if(!packets[i].timing) {
              if(_auto_delete) {
                remove(i);
                packets_count--;
              }
            } else {
              packets[i].attempts = 0;
              packets[i].registration = micros();
              packets[i].state = TO_BE_SENT;
            } continue;
          }

          packets[i].attempts++;
          if(packets[i].attempts > MAX_ATTEMPTS) {
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

      uint8_t data[PACKET_MAX_LENGTH];
      PJON_Packet packets[MAX_PACKETS];

      /* A bus id is an array of 4 bytes containing a unique set.
          The default setting is to run a local bus (0.0.0.0), in this
          particular case the obvious bus id is omitted from the packet
          content to reduce overhead. */

      const uint8_t localhost[4] = {0, 0, 0, 0};
      uint8_t bus_id[4] = {0, 0, 0, 0};

      /* Last received packet Metainfo */
      PacketInfo last_packet_info;

    private:
      boolean   _acknowledge = true;
      boolean   _auto_delete = true;
      uint8_t   _device_id;
      boolean   _shared = false;
      boolean   _sender_info = true;
      uint8_t   _mode;
      receiver  _receiver;
      boolean   _router = false;
      error     _error;
  };
#endif
