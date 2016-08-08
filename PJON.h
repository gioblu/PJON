
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \| v4.1
   |y"s§+`\     Giovanni Blu Mitolo 2012-2016
  /so+:-..`\    gioscarab@gmail.com
  |+/:ngr-*.`\
  |5/:%&-a3f.:;\    PJON is a multi-master, multi-media, device communications bus
  \+//u/+g%{osv,,\   system framework able to connect up to 255 arduino boards over
    \=+&/osw+olds.\\   one or two wires up to 5.95kB/s.
       \:/+-.-°-:+oss\
        | |       \oy\\
        > <
  _____-| |-________________________________________________________________________

Copyright 2012-2016 Giovanni Blu Mitolo gioscarab@gmail.com

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
  #include <digitalWriteFast.h>

  /* Communication modes */
  #define SIMPLEX     150
  #define HALF_DUPLEX 151

  /* Protocol symbols */
  #define ACK         6
  #define ACQUIRE_ID  63
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

  #include "strategies/OverSampling/OverSampling.h"
  #include "strategies/SoftwareBitBang/SoftwareBitBang.h"
  #include "strategies/ThroughHardwareSerial/ThroughHardwareSerial.h"

  /* Errors */
  #define CONNECTION_LOST     101
  #define PACKETS_BUFFER_FULL 102
  #define MEMORY_FULL         103
  #define CONTENT_TOO_LONG    104
  #define ID_ACQUISITION_FAIL 105

  /* Constraints:
  Max attempts before throwing CONNECTON_LOST error */
  #ifndef MAX_ATTEMPTS
    #define MAX_ATTEMPTS        125
  #endif
  
  /* Packets buffer length, if full PACKETS_BUFFER_FULL error is thrown */
  #ifndef MAX_PACKETS
    #define MAX_PACKETS       10
  #endif

  /* Max packet length, higher if necessary (and you have free memory) */
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
    uint8_t  header;
    uint8_t  device_id;
    char     *content;
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


      /* Look for a free id:
         All ids are scanned looking for a free one. If no answer is received after
         MAX_ATTEMPTS attempts, id is acquired and used as new id by the scanning device. */

      void acquire_id() {
        uint32_t time = micros();
        uint8_t ping_id;
        char msg = ACQUIRE_ID;

        for(uint8_t id = 1; id < 255 && (uint32_t)(micros() - time) < MAX_ID_SCAN_TIME; id++) {
          ping_id = send(id, &msg, 1);

          while(packets[ping_id].state != 0 && (uint32_t)(micros() - time) < MAX_ID_SCAN_TIME)
            update();

          if(_device_id != NOT_ASSIGNED) return;
        }
        _error(ID_ACQUISITION_FAIL, FAIL);
      };


      /* Initial random delay to avoid startup collision */

      void begin() {
        randomSeed(analogRead(A0));
        delay(random(0, INITIAL_MAX_DELAY));
      };


      /* Check equality between two bus ids */

      boolean bus_id_equality(const uint8_t *name_one, const uint8_t *name_two) {
        for(uint8_t i = 0; i < 4; i++)
          if(name_one[i] != name_two[i])
            return false;
        return true;
      };


      /* Copy a bus id: */

      void copy_bus_id(uint8_t dest[], const uint8_t src[]) const { memcpy(dest, src, 4); };


      /* Compute CRC8 with a table-less implementation: */

      uint8_t compute_crc_8(char input_byte, uint8_t crc) {
        for (uint8_t i = 8; i; i--, input_byte >>= 1) {
          uint8_t result = (crc ^ input_byte) & 0x01;
          crc >>= 1;
          if(result) crc ^= 0x8C;
        }
        return crc;
      };


      /* Get the device id, returning a single byte (watch out to id collision): */

      uint8_t device_id() {
        return _device_id;
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

      /* Try to receive a packet: */

      uint16_t receive() {
        uint16_t state;
        uint16_t packet_length = PACKET_MAX_LENGTH;
        uint8_t CRC = 0;
        bool shared = false;
        bool includes_sender_info = false;
        bool acknowledge_requested = false;

        for(uint8_t i = 0; i < packet_length; i++) {
          data[i] = state = strategy.receive_byte(_input_pin, _output_pin);
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
            if(bus_id[i - 3] != data[i])
              return BUSY;

          CRC = compute_crc_8(data[i], CRC);
        }
        if(!CRC) {
          if(acknowledge_requested && data[0] != BROADCAST && _mode != SIMPLEX)
            if(!_shared || (_shared && shared && bus_id_equality(data + 3, bus_id)))
              strategy.send_response(ACK, _input_pin, _output_pin);

          get_packet_info(data, last_packet_info);
          uint8_t payload_offset = 3 + (shared ? (includes_sender_info ? 9 : 4) : (includes_sender_info ? 1 : 0));
          _receiver(data + payload_offset, data[3] - payload_offset - 1, last_packet_info);
          return ACK;
        } else {
          if(acknowledge_requested && data[0] != BROADCAST && _mode != SIMPLEX)
            if(!_shared || (_shared && shared && bus_id_equality(data + 3, bus_id)))
              strategy.send_response(NAK, _input_pin, _output_pin);
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
        free(packets[id].content);
        packets[id].attempts = 0;
        packets[id].device_id = 0;
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
          if(!device_id || packets[i].device_id == device_id) remove(i);
        }
      };


      /* Get count of the packets for a device_id:
         Don't pass any parameter to count all packets
         Pass a device id to count all it's related packets */

      uint8_t get_packet_count(uint8_t device_id = 0) {
        uint8_t packet_count = 0;
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          if(packets[i].state == 0) continue;
          if(!device_id || packets[i].device_id == device_id) packet_count++;
        }
        return packet_count;
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

       bus.remove(hi); // Stop repeated sending
       _________________________________________________________________________
      |           |        |         |       |          |        |              |
      | device_id | length | content | state | attempts | timing | registration |
      |___________|________|_________|_______|__________|________|______________| */

      uint16_t send(uint8_t id, const char *packet, uint8_t length) {
        return dispatch(id, bus_id, packet, length, 0);
      };

      uint16_t send(uint8_t id, uint8_t *b_id, const char *packet, uint8_t length) {
        return dispatch(id, b_id, packet, length, 0);
      };

      uint16_t send_repeatedly(uint8_t id, const char *packet, uint8_t length, uint32_t timing) {
        return dispatch(id, bus_id, packet, length, timing);
      };

      uint16_t send_repeatedly(uint8_t id, uint8_t *b_id, const char *packet, uint8_t length, uint32_t timing) {
        return dispatch(id, b_id, packet, length, timing);
      };


      /* Send a packet to the sender of the last packet received.
         This function is typically called from with the receive
         callback function to deliver a response to a request. */

      uint16_t reply(const char *packet, uint8_t length) {
        if(last_packet_info.sender_id != BROADCAST)
          return dispatch(last_packet_info.sender_id, last_packet_info.sender_bus_id, packet, length, 0);
        return false;
      };


      uint16_t dispatch(uint8_t id, uint8_t *b_id, const char *packet, uint8_t length, uint32_t timing, uint8_t header = 0) {
        uint8_t new_length = _shared ? (length + (_sender_info ? 9 : 4)) : (length + (_sender_info ? 1 : 0));

        // Compose PJON 1 byte header from internal configuration
        header |= (_shared ? MODE_BIT : 0);
        header |= (_sender_info ? SENDER_INFO_BIT : 0);
        header |= (_acknowledge ? ACK_REQUEST_BIT : 0);

        if(new_length >= PACKET_MAX_LENGTH) {
          _error(CONTENT_TOO_LONG, new_length);
          return FAIL;
        }

        char *str = (char *) malloc(new_length);

        if(str == NULL) {
          _error(MEMORY_FULL, 0);
          return FAIL;
        }

        if(_shared) {
          copy_bus_id((uint8_t*) str, b_id);
          if(_sender_info) {
            copy_bus_id((uint8_t*) &str[4], bus_id);
            str[8] = _device_id;
          }
        } else if(_sender_info) str[0] = _device_id;

        memcpy(str + (_shared ? (_sender_info ? 9 : 4) : (_sender_info ? 1 : 0)), packet, length);

        for(uint8_t i = 0; i < MAX_PACKETS; i++)
          if(packets[i].state == 0) {
            packets[i].header = header;
            packets[i].content = str;
            packets[i].device_id = id;
            packets[i].length = new_length;
            packets[i].state = TO_BE_SENT;
            packets[i].registration = micros();
            packets[i].timing = timing;
            return i;
          }

        _error(PACKETS_BUFFER_FULL, MAX_PACKETS);
        return FAIL;
      };


  /* An Example of how the string "@" is formatted and sent:

  RECIPIENT ID 12   LENGTH 6          HEADER 00000100  SENDER ID 11      CONTENT 64       CRC
   ________________ _________________ ________________ _________________ ________________ __________________
  |Sync | Byte     |Sync | Byte      |Sync | Byte     |Sync | Byte      |Sync | Byte     |Sync | Byte       |
  |___  |     __   |___  |      _   _|___  |      _   |___  |     _   __|___  |  _       |___  |  _      _  |
  |   | |    |  |  |   | |     | | | |   | |     | |  |   | |    | | |  |   | | | |      |   | | | |    | | |
  | 1 |0|0000|11|00| 1 |0|00000|1|0|1| 1 |0|00000|1|00| 1 |0|0000|1|0|11| 1 |0|0|1|000000| 1 |0|0|1|0000|1|0|
  |___|_|____|__|__|___|_|_____|_|_|_|___|_|_____|_|__|___|_|____|_|_|__|___|_|_|_|______|___|_|_|_|____|_|_|

  A standard packet transmission is a bidirectional communication between
  two devices that can be divided in 3 different phases:

  Channel analysis   Transmission                                                 Response
      _____           ____________________________________________________         _____
     | C-A |         | ID | LENGTH | HEADER |  SENDER ID  | CONTENT | CRC |       | ACK |
  <--|-----|---< >---|----|--------|--------|-------------|---------|-----|--> <--|-----|
     |  0  |         | 12 |   5    |  001   |    ID 11    |   64    |     |       |  6  |
     |_____|         |____|________|________|_____________|_________|_____|       |_____|

  DEFAULT HEADER CONFIGURATION:
  [0, 1, 1]: Local bus | Sender info included | Acknowledge requested

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
  [0, 0, 0]: Local bus | Sender info included | Acknowledge requested

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
  [1, 1, 1]: Local bus | Sender info included | Acknowledge requested - DEFAULT

  BUS CONFIGURATION:
  bus.set_acknowledge(true);
  bus.include_sender_info(true);

  High overhead, low bandwidth availability setup. Can be used sharing the medium
  with many other buses with transmission certainty through synchronous acknowledge
  and sender info to easy reply to packets with the reply() function. */

      uint16_t send_string(uint8_t id, char *string, uint8_t length, uint8_t header = 0) {
        if(!string) return FAIL;
        if(_mode != SIMPLEX && !strategy.can_start(_input_pin, _output_pin)) return BUSY;

        uint8_t CRC = 0;

        // Transmit recipient device id
        strategy.send_byte(id, _input_pin, _output_pin);
        CRC = compute_crc_8(id, CRC);

        // Transmit packet length
        strategy.send_byte(length + 4, _input_pin, _output_pin);
        CRC = compute_crc_8(length + 4, CRC);

        // Transmit header header
        strategy.send_byte(header, _input_pin, _output_pin);
        CRC = compute_crc_8(header, CRC);

        /* If an id is assigned to the bus, the packet's content is prepended by
           the ricipient's bus id. This opens up the possibility to have more than
           one bus sharing the same medium. */

        for(uint8_t i = 0; i < length; i++) {
          strategy.send_byte(string[i], _input_pin, _output_pin);
          CRC = compute_crc_8(string[i], CRC);
        }

        strategy.send_byte(CRC, _input_pin, _output_pin);

        if(!_acknowledge || id == BROADCAST || _mode == SIMPLEX) return ACK;

        uint16_t response = strategy.receive_response(_input_pin, _output_pin);

        if(response == ACK) return ACK;

        /* Random delay if NAK, corrupted ACK/NAK or collision */
        if(response != FAIL)
          delayMicroseconds(random(0, COLLISION_MAX_DELAY));

        if(response == NAK) return NAK;

        return FAIL;
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


      /* Set the communicaton pin: */

      void set_pin(uint8_t pin) {
        _input_pin = pin;
        _output_pin = pin;
      };


      /* Set a pair of communication pins: */

      void set_pins(uint8_t input_pin = NOT_ASSIGNED, uint8_t output_pin = NOT_ASSIGNED) {
        _input_pin = input_pin;
        _output_pin = output_pin;

        if(input_pin == NOT_ASSIGNED || output_pin == NOT_ASSIGNED)
          _mode = SIMPLEX;
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
        uint8_t packet_count = 0;
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          if(packets[i].state == 0) continue;

          packet_count++;

          if((uint32_t)(micros() - packets[i].registration) > packets[i].timing + pow(packets[i].attempts, 3))
            packets[i].state = send_string(packets[i].device_id, packets[i].content, packets[i].length, packets[i].header);
          else continue;

          if(packets[i].state == ACK) {
            if(!packets[i].timing) {
              if(_auto_delete) {
                remove(i);
                packet_count--;
              }
            } else {
              packets[i].attempts = 0;
              packets[i].registration = micros();
              packets[i].state = TO_BE_SENT;
            }
          }

          if(packets[i].state == FAIL) {
            packets[i].attempts++;
            if(packets[i].attempts > MAX_ATTEMPTS) {
              if(packets[i].content[0] == ACQUIRE_ID) {
                _device_id = packets[i].device_id;
                remove(i);
                packet_count--;
                continue;
              } else _error(CONNECTION_LOST, packets[i].device_id);
              if(!packets[i].timing) {
                if(_auto_delete) {
                  remove(i);
                  packet_count--;
                }
              } else {
                packets[i].attempts = 0;
                packets[i].registration = micros();
                packets[i].state = TO_BE_SENT;
              }
            }
          }
        }
        return packet_count;
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
      uint8_t   _input_pin;
      boolean   _shared = false;
      boolean   _sender_info = true;
      uint8_t   _mode;
      uint8_t   _output_pin;
      receiver  _receiver;
      boolean   _router = false;
      error     _error;
  };
#endif
