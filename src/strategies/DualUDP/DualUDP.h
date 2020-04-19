
/* DualUDP is a Strategy for the PJON framework.
   It supports delivering PJON packets through Ethernet UDP to a registered
   list of devices on the LAN, WAN or Internet. Each device must be registered
   with its device id, IP address and listening port number.

   Autopopulating the node table based on received packets and broadcasts is
   enabled by default. This requires only remote (non-LAN) devices to be
   manually registered.

   So this strategy combines the manually populated node list from the
   GlobalUDP strategy with the broadcast based autodiscovery of devices
   on the LAN in the LocalUDP strategy. If improves on LocalUDP by
   stepping from UDP broadcasts to directed UDP packets, creating less
   noise for other devices that may be listening to the same port.
   It can communicate with devices outside the LAN if they are added manually
   to the list, or if they send a packet to this device first.

   Note that this strategy cannot send and receive any contents, only
   packets with a valid PJON header.
   ___________________________________________________________________________

    DualUDP strategy proposed and developed by Fred Larsen 01/12/2018

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

#ifdef HAS_ETHERNETUDP
  #include <interfaces/ARDUINO/UDPHelper_ARDUINO.h>
#else
  #include <interfaces/LINUX/UDPHelper_POSIX.h>
#endif

#include <PJONDefines.h>

// Timeout waiting for an ACK. This can be increased if the latency is high
#ifndef DUDP_RESPONSE_TIMEOUT
  #define DUDP_RESPONSE_TIMEOUT          50000ul
#endif

// Minimum time interval in ms between send attempts. Some devices go into 
// contention if sending too fast. This can be overridden in an interface
// for a device type, or in user sketches.
#ifndef DUDP_MINIMUM_SEND_INTERVAL_MS 
  #define DUDP_MINIMUM_SEND_INTERVAL_MS  8
#endif

// Backoff function that can be overridden depending on network and devices
#ifndef DUDP_BACKOFF
  #define DUDP_BACKOFF(attempts) (1000ul * attempts + PJON_RANDOM(500))
#endif

// Max number of retries
#ifndef DUDP_MAX_RETRIES
  #define DUDP_MAX_RETRIES 5
#endif

// The size of the node table
#ifndef DUDP_MAX_REMOTE_NODES
  #define DUDP_MAX_REMOTE_NODES               10
#endif

// Remove automatically registered nodes after this number of send failures
#ifndef DUDP_MAX_FAILURES
  #define DUDP_MAX_FAILURES                   10
#endif

#define DUDP_DEFAULT_PORT                   7500
#define DUDP_MAGIC_HEADER  (uint32_t) 0x0EFA23FF

//#define DUDP_DEBUG_PRINT

class DualUDP {
    bool _udp_initialized = false;
    bool _auto_registration = true, // Add all sender devices to node table
         _auto_discovery = true,    // Use UDP broadcast to locate LAN devices
         _did_broadcast = false;    // Whether last send was a broadcast

    uint16_t _port = DUDP_DEFAULT_PORT;
    uint8_t  _unremovable_node_count = PJON_NOT_ASSIGNED;

    // Remember the details of the last outgoing packet
    uint8_t          _last_out_receiver_id = 0;
    uint8_t          _last_out_sender_id = 0;
    uint32_t         _last_out_time = 0;

    // Remember the details of the last incoming packet
    PJON_Packet_Info _packet_info; // Also used for last outgoing
    uint16_t         _last_in_sender_port = 0;
    uint8_t          _last_in_sender_ip[4];
    uint8_t          _last_in_receiver_id = 0;
    uint8_t          _last_in_sender_id = 0;

    // Remote nodes table
    uint8_t  _remote_node_count = 0;
    uint8_t  _remote_id[DUDP_MAX_REMOTE_NODES];
    uint8_t  _remote_ip[DUDP_MAX_REMOTE_NODES][4];
    uint16_t _remote_port[DUDP_MAX_REMOTE_NODES];
    uint8_t  _send_attempts[DUDP_MAX_REMOTE_NODES];

    UDPHelper udp;

    bool check_udp() {
      if(!_udp_initialized) {
        udp.set_magic_header(htonl(DUDP_MAGIC_HEADER));
        if(udp.begin(_port)) _udp_initialized = true;
      }
      return _udp_initialized;
    };

    int16_t find_remote_node(uint8_t id) {
      for(uint8_t i = 0; i < _remote_node_count; i++)
        if(_remote_id[i] == id)
          return i;
      return -1;
    };

    int16_t autoregister_sender() {
      // Add the last sender to the node table
      if(_auto_registration) {
        // If parsing fails, it will be 0
        if( _last_in_sender_id == 0) return -1;
        // See if PJON id is already registered, add if not
        int16_t pos = find_remote_node( _last_in_sender_id);
        if(pos == -1)
          return add_node(
            _last_in_sender_id,
            _last_in_sender_ip,
            _last_in_sender_port
          );
        else { // Update IP and port of existing node
          memcpy(_remote_ip[pos],  _last_in_sender_ip, 4);
          _remote_port[pos] =  _last_in_sender_port;
          return pos;
        }
      }
      return -1;
    };

public:

    /* Register each device we want to send to.
       If device id is set and autoregistration enabled, this table will
       be filled automatically with devices that send to this device.
       Devices that this device will send to must be registered if they are
       outside the LAN and do not send a packet to this device first.
       Devices on this LAN do not need to be manually registered. */

    int16_t add_node(
      uint8_t remote_id,
      const uint8_t remote_ip[],
      uint16_t port_number = DUDP_DEFAULT_PORT
    ) {
      if(_remote_node_count == DUDP_MAX_REMOTE_NODES) return -1;
      // Remember how many nodes were present at startup
      if(_unremovable_node_count == PJON_NOT_ASSIGNED)
        _unremovable_node_count = _remote_node_count;
      #ifdef DUDP_DEBUG_PRINT
        Serial.print("Register id "); Serial.print(remote_id);
        Serial.print(" ip "); Serial.println(remote_ip[3]);
      #endif
      // Add the new node
      _remote_id[_remote_node_count] = remote_id;
      memcpy(_remote_ip[_remote_node_count], remote_ip, 4);
      _remote_port[_remote_node_count] = port_number;
      _send_attempts[_remote_node_count] = 0;
      _remote_node_count++;
      return _remote_node_count - 1;
    };

    /* Unregister a node, if unreachable */

    bool remove_node(uint8_t pos) {
      // Only allow the automatically added nodes to be removed
      if(pos < _unremovable_node_count) return false;
      #ifdef DUDP_DEBUG_PRINT
        Serial.print("Unregistering id "); Serial.println(_remote_id[pos]);
      #endif
      for(uint8_t i = pos; i < _remote_node_count - 1; i++) {
        _remote_id[i] = _remote_id[i + 1];
        memcpy(_remote_ip[i], _remote_ip[i + 1], 4);
        _remote_port[i] = _remote_port[i + 1];
        _send_attempts[i] = _send_attempts[i + 1];
      }
      _remote_node_count--;
      return true;
    };

    /* Whether the last send was a broadcast or a directed packet.
       This is to let routers and sketches have a little insight. */

    bool did_broadcast() {
      return _did_broadcast;
    };

    /* Select if incoming packets should automatically add their sender
       as a node */

    void set_autoregistration(bool enabled = true) {
      _auto_registration = enabled;
    };

    /* Select if broadcast shall be used to reach unregistered devices
       and then add them to the node table when replying, going from
       broadcast to directed communication as soon as possible. */

    void set_autodiscovery(bool enabled = true) {
      _auto_discovery = enabled;
    };

    /* Returns the suggested delay related to attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) {
      return DUDP_BACKOFF(attempts);
    };

    /* Begin method, to be called on initialization:
       (returns always true) */

    bool begin(uint8_t device_id) {
      (void)device_id; // Avoid "unused parameter" warning
      return check_udp();
    };

    /* Check if the channel is free for transmission */

    bool can_start() {
      return check_udp() && ((uint32_t)(PJON_MILLIS() - _last_out_time) >=
        DUDP_MINIMUM_SEND_INTERVAL_MS);
    };

    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() { return DUDP_MAX_RETRIES; };

    /* Handle a collision (empty because handled on Ethernet level): */

    void handle_collision() { };

    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      uint16_t length = udp.receive_frame(data, max_length);
      // Then get the IP address and port number of the sender
      udp.get_sender( _last_in_sender_ip,  _last_in_sender_port);
      if(length != PJON_FAIL && length > 4) {
        // Extract some info from the header
        PJONTools::parse_header(data, _packet_info);
         _last_in_receiver_id = _packet_info.receiver_id;
         _last_in_sender_id = _packet_info.sender_id;
        // Autoregister sender if the packet was sent directly
        if(
          _packet_info.sender_id != PJON_NOT_ASSIGNED &&
          _last_out_sender_id != PJON_NOT_ASSIGNED &&
          _packet_info.receiver_id == _last_out_sender_id
        ) autoregister_sender();
      }
      return length;
    };

    /* Receive byte response */

    uint16_t receive_response() {
      uint32_t start = PJON_MICROS();
      uint8_t result[10];
      uint16_t reply_length = 0;
      do {
        reply_length = receive_frame(result, sizeof result);
        if(reply_length == PJON_FAIL) continue;

        // Ignore full PJON packets, we expect only a tiny response packet
        if(reply_length != 3) continue;

        // Decode response packet
        _last_in_receiver_id = result[0];
        _last_in_sender_id = result[1];
        uint8_t code = result[2];

        // Ignore packets not responding to the last outgoing packet
        if(_last_in_receiver_id != _last_out_sender_id) continue;

        // Ignore packets not from the receiver of the last outgoing packet
        // 20181205: NO, allow these ACKS even if they are delayed,
        // because it could be caused by forwarding multiple hops,
        // and a delayed ACK is still a confirmation of the correct route.
        //if(_last_in_sender_id != _last_out_receiver_id) continue;

        if(code == PJON_ACK) {
          // Autoregister sender of ACK
          int16_t pos = autoregister_sender();
          // Reset send attempt counter
          if(pos != -1) _send_attempts[pos] = 0;
          return code;
        }
      } while((uint32_t)(PJON_MICROS() - start) < DUDP_RESPONSE_TIMEOUT);
      #ifdef DUDP_DEBUG_PRINT
        Serial.println("Receive_response FAILED");
      #endif
      return PJON_FAIL;
    };

    /* Send byte response to package transmitter.
       We have the IP so we can reply directly.
       Use the receiver id of the last incoming packet instead of the id of
       this device, to function also in router mode. */

    void send_response(uint8_t response) { // Empty, PJON_ACK is always sent
      uint8_t buf[3];
      buf[0] = _last_in_sender_id;   // Send to the device last received from
      buf[1] = _last_in_receiver_id; // Send from the id last received to
      buf[2] = response;
      udp.send_response(buf, 3);
    };

    /* Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      _did_broadcast = false;
      if(length > 4) {
        // Extract some info from the header
        PJONTools::parse_header(data, _packet_info);
         _last_out_receiver_id = _packet_info.receiver_id;
         _last_out_sender_id = _packet_info.sender_id;

        // Locate receiver in table unless it is a PJON broadcast (receiver 0)
        int16_t pos = -1;
        if(_last_out_receiver_id != 0)
          pos = find_remote_node(_last_out_receiver_id);

        // Check if receiver is not responding and should be unregistered
        if(
          pos != -1 &&
          (_send_attempts[pos] > (get_max_attempts() * DUDP_MAX_FAILURES)) &&
          remove_node((uint8_t)pos)
        ) pos = -1;

        if(pos == -1) { // UDP Broadcast, send to all receivers
          if(_auto_discovery) udp.send_frame(data, length);
          _did_broadcast = true;
          #ifdef DUDP_DEBUG_PRINT
            Serial.print("Broadcast, id ");
            Serial.println(_last_out_receiver_id);
          #endif
        } else { // To a specific IP+port
          udp.send_frame(data, length, _remote_ip[pos], _remote_port[pos]);
          _send_attempts[pos]++;
        }
        _last_out_time = millis();
      }
    };

    /* Set the UDP port: */

    void set_port(uint16_t port = DUDP_DEFAULT_PORT) {
      _port = port;
    };
};
