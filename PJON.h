
 /*-O//\         __     __
   |-gfo\       |__| | |  | |\ |
   |!y°o:\      |  __| |__| | \| v3.0
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
  #define ACK           6
  #define ACQUIRE_ID    63
  #define BUSY          666
  #define NAK           21

  /* Reserved addresses */
  #ifndef BROADCAST
    #define BROADCAST     0
  #endif
  #ifndef NOT_ASSIGNED
    #define NOT_ASSIGNED  255
  #endif

  #if BROADCAST == NOT_ASSIGNED
    #error BROADCAST and NOT_ASSIGNED point the same address
  #endif

  /* Internal constants */
  #define FAIL          0x100
  #define TO_BE_SENT    74
  #define SHARED_BIT    0x10
  
  #include "strategies/SoftwareBitBang/SoftwareBitBang.h"
  #include "strategies/OverSampling/OverSampling.h"

  /* Errors */
  #define CONNECTION_LOST     101
  #define PACKETS_BUFFER_FULL 102
  #define MEMORY_FULL         103
  #define CONTENT_TOO_LONG    104
  #define ID_ACQUISITION_FAIL 105

  /* Constraints:
  Max attempts before throwing CONNECTON_LOST error */
  #define MAX_ATTEMPTS        125
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
  #define COLLISION_MAX_DELAY 16
  /* Maximum id scan time (5 seconds) */
  #define MAX_ID_SCAN_TIME    5000000

  struct PJON_Packet {
    uint8_t  attempts;
    uint8_t  type;
    uint8_t  device_id;
    char     *content;  // if shared medium, the contents starts with receiver and sender bus ids
    uint8_t  length;
    uint32_t registration;
    uint16_t state;
    uint32_t timing;
  };  
  
  /* Metainfo about the last received packet (type, sender id and bus). 
   The bus ids will only be available if (type & SHARED_BIT != 0). */
  struct PacketInfo {
    uint8_t type = 0;
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

    Strategy strategy;

    public:

      /* PJON bus default initialization:
         State: Local (bus_id: 0.0.0.0)
         Acknowledge: true
         device id: NOT_ASSIGNED (255)
         Mode: HALF_DUPLEX
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

      boolean bus_id_equality(uint8_t *name_one, uint8_t *name_two) {
        for(uint8_t i = 0; i < 4; i++)
          if(name_one[i] != name_two[i])
            return false;
        return true;        
      };

      /* Copy a bus id */

      void copy_bus_id(uint8_t dest[], uint8_t src[]) { memcpy(dest, src, 4); }  

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


      /* Try to receive a packet: */

      uint16_t receive() {
        uint16_t state;
        uint16_t packet_length = PACKET_MAX_LENGTH;
        uint8_t CRC = 0;
        uint8_t sender_id = 0;
        uint8_t sender_bus_id[4] = {0,0,0,0};
        uint8_t packet_type = 0;
        bool bus_present = false;

        for(uint8_t i = 0; i < packet_length; i++) {
          data[i] = state = Strategy::receive_byte(_input_pin, _output_pin);
          if(state == FAIL) return FAIL;

          if(i == 0) {
            packet_type = data[i];
            bus_present = (packet_type & SHARED_BIT != 0);
            if ((bus_present != _shared) && !_router) return BUSY; // Keep private and shared buses apart
          }

          if(i == 1 && data[i] != _device_id && data[i] != BROADCAST && !_router)
            return BUSY;

          /* If an id is assigned to this bus it means that is potentially
             sharing its medium, or the device could be connected in parallel
             with other buses. Bus id equality is checked to avoid collision
             i.e. id 1 bus 1, should not receive a message for id 1 bus 2. */

          if(i == 3) {
            if(data[i] > 5 && data[i] < PACKET_MAX_LENGTH)
              packet_length = data[i];
            else return FAIL;
          }

          if(_shared && bus_present && !_router && i > 3 && i < 8)
            if(bus_id[i - 4] != data[i])
              return BUSY;
            
          CRC = compute_crc_8(data[i], CRC);
        }
        if(!CRC) {
          if(_acknowledge && data[1] != BROADCAST && _mode != SIMPLEX)
            if(!_shared || (_shared && bus_present && bus_id_equality(data + 4, bus_id)))
              Strategy::send_response(ACK, _input_pin, _output_pin);
          last_packet_info.type = packet_type;
          last_packet_info.receiver_id = data[1];
          last_packet_info.sender_id = data[2];
          if (bus_present) {
            copy_bus_id(last_packet_info.receiver_bus_id, data + 4);
            copy_bus_id(last_packet_info.sender_bus_id, data + 8);
          }
          _receiver(data + (bus_present ? 12 : 4), data[3] - (bus_present ? 13 : 5), last_packet_info);
          return ACK;
        } else {
          if(_acknowledge && data[1] != BROADCAST && _mode != SIMPLEX)
            if(!_shared || (_shared && bus_present && bus_id_equality(data + 4, bus_id)))
              Strategy::send_response(NAK, _input_pin, _output_pin);
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

      uint16_t send(uint8_t id, const char *packet, uint8_t length, uint8_t custom_type = 0) {
        dispatch(id, bus_id, packet, length, 0, custom_type & 0x0F);
      };

      uint16_t send(uint8_t id, uint8_t b_id, const char *packet, uint8_t length, uint8_t custom_type = 0) {
        dispatch(id, b_id, packet, length, 0, custom_type & 0x0F);
      };

      uint16_t send_repeatedly(uint8_t id, const char *packet, uint8_t length, uint32_t timing, uint8_t custom_type = 0) {
        dispatch(id, bus_id, packet, length, timing, custom_type & 0x0F);
      };

      uint16_t send_repeatedly(uint8_t id, uint8_t b_id, const char *packet, uint8_t length, uint32_t timing, uint8_t custom_type = 0) {
        dispatch(id, b_id, packet, length, timing, custom_type & 0x0F);
      };

    /* Send a packet to the sender of the last packet received.
     This function is typically called from with the receive callback function to deliver a response to
     a request. */
      uint16_t reply(const char *packet, uint8_t length, uint8_t custom_type = 0) {
        if (last_packet_info.sender_id > 0)
          dispatch(last_packet_info.sender_id, last_packet_info.sender_bus_id, packet, length, 0, custom_type);
      }
      
      uint16_t dispatch(uint8_t id, uint8_t *b_id, const char *packet, uint8_t length, uint32_t timing, uint8_t custom_type = 0) {
        length = (_shared) ? length + 8 : length;

        if(length >= PACKET_MAX_LENGTH) {
          _error(CONTENT_TOO_LONG, length);
          return FAIL;
        }

        char *str = (char *) malloc(length);

        if(str == NULL) {
          _error(MEMORY_FULL, 0);
          return FAIL;
        }

        if(_shared) {
          copy_bus_id(str, b_id);
          copy_bus_id(&str[4], bus_id);
        }
        memcpy((_shared) ? str + 8 : str, packet, length);

        for(uint8_t i = 0; i < MAX_PACKETS; i++)
          if(packets[i].state == 0) {
            packets[i].type = custom_type | (_shared ? SHARED_BIT : 0);
            packets[i].content = str;
            packets[i].device_id = id;
            packets[i].length = length;
            packets[i].state = TO_BE_SENT;
            packets[i].registration = micros();
            packets[i].timing = timing;
            return i;
          }

        _error(PACKETS_BUFFER_FULL, MAX_PACKETS);
        return FAIL;
      };


      /* An Example of how the string "@" is formatted and sent:

       ID 12            LENGTH 4         CONTENT 64       CRC 130
       ________________ ________________ ________________ __________________
      |Sync | Byte     |Sync | Byte     |Sync | Byte     |Sync | Byte       |
      |___  |     __   |___  |      _   |___  |  _       |___  |  _      _  |
      |   | |    |  |  |   | |     | |  |   | | | |      |   | | | |    | | |
      | 1 |0|0000|11|00| 1 |0|00000|1|00| 1 |0|0|1|000000| 1 |0|0|1|0000|1|0|
      |___|_|____|__|__|___|_|_____|_|__|___|_|_|_|______|___|_|_|_|____|_|_|

      A standard packet transmission is a bidirectional communication between
      two devices that can be divided in 3 different phases:

      Channel analysis   Transmission                            Response
          _____           _____________________________           _____
         | C-A |         | ID | LENGTH | CONTENT | CRC |         | ACK |
      <--|-----|---< >---|----|--------|---------|-----|--> <----|-----|
         |  0  |         | 12 |   4    |   64    | 130 |         |  6  |
         |_____|         |____|________|_________|_____|         |_____|  */

      uint16_t send_string(uint8_t id, char *string, uint8_t length, uint8_t custom_type = 0) {
        if(!string) return FAIL;
        if(_mode != SIMPLEX && !Strategy::can_start(_input_pin, _output_pin)) return BUSY;

        uint8_t CRC = 0;
        uint8_t type = (custom_type & 0x0F) | (_shared ? SHARED_BIT : 0);
        Strategy::send_byte(type, _input_pin, _output_pin);         // content type
        CRC = compute_crc_8(type, CRC);
        Strategy::send_byte(id, _input_pin, _output_pin);           // receiver id
        CRC = compute_crc_8(id, CRC);
        Strategy::send_byte(_device_id, _input_pin, _output_pin);   // sender id
        CRC = compute_crc_8(_device_id, CRC);
        Strategy::send_byte(length + 5, _input_pin, _output_pin);   // length
        CRC = compute_crc_8(length + 5, CRC);

        /* If an id is assigned to the bus, the packet's content is prepended by
           the ricipient's bus id. This opens up the possibility to have more than
           one bus sharing the same medium. */

        for(uint8_t i = 0; i < length; i++) {
          Strategy::send_byte(string[i], _input_pin, _output_pin);
          CRC = compute_crc_8(string[i], CRC);
        }

        Strategy::send_byte(CRC, _input_pin, _output_pin);

        if(!_acknowledge || id == BROADCAST || _mode == SIMPLEX) return ACK;

        uint16_t response = Strategy::receive_response(_input_pin, _output_pin);

        if(response == ACK) return ACK;

        /* Random delay if NAK, corrupted ACK/NAK or collision */
        if(response != FAIL)
          delayMicroseconds(random(0, COLLISION_MAX_DELAY));

        if(response == NAK) return NAK;

        return FAIL;
      };


      /* Configure sincronous acknowledge presence:
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


      /* Configure the bus network behaviour.
         TRUE: Enable communication to devices part of other bus ids (on a shared medium).
         FALSE: Isolate communication from external/third-party communication. */

      void set_shared_network(boolean state) {
        _shared = state;
      }


      /* Set if delivered or undeliverable packets are auto deleted:
         TRUE: manual packet deletion from buffer is needed
         FALSE: Automatic deletion in case o  */

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

        void receiver_function(uint8_t id, uint8_t *payload, uint8_t length) {
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
         check if there are packets to be sent or to be erased
         if correctly delivered */

      void update() {
        for(uint8_t i = 0; i < MAX_PACKETS; i++) {
          if(packets[i].state == 0) continue;
          if((uint32_t)(micros() - packets[i].registration) > packets[i].timing + pow(packets[i].attempts, 3))
            packets[i].state = send_string(packets[i].device_id, packets[i].content, packets[i].length, packets[i].type);
          else continue;

          if(packets[i].state == ACK) {
            if(!packets[i].timing) {
              if(_auto_delete) remove(i);
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
                continue;
              } else _error(CONNECTION_LOST, packets[i].device_id);
              if(!packets[i].timing) {
                if(_auto_delete) remove(i);
              } else {
                packets[i].attempts = 0;
                packets[i].registration = micros();
                packets[i].state = TO_BE_SENT;
              }
            }
          }
        }
      };

      uint8_t data[PACKET_MAX_LENGTH];
      PJON_Packet packets[MAX_PACKETS];

      /* A bus id is an array of 4 bytes containing a unique set.
          The default setting is to run a local bus (0.0.0.0), in this
          particular case the obvious bus id is omitted from the packet
          content to reduce overhead. */

      /* Remember some Metainfo about the last received packet (type, sender and receiver id and bus) */
      PacketInfo last_packet_info;
      
      uint8_t localhost[4] = {0, 0, 0, 0};
      uint8_t bus_id[4] = {0, 0, 0, 0};    
      
    private:
      boolean   _acknowledge = true;
      boolean   _auto_delete = true;
      uint8_t   _device_id;
      uint8_t   _input_pin;
      boolean   _shared = false;
      uint8_t   _mode;
      uint8_t   _output_pin;
      receiver  _receiver;
      boolean   _router = false;
      error     _error;
  };
#endif
