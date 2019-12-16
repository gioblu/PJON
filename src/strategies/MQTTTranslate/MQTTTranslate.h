
/* MQTTTranslate is a Strategy for the PJON framework (included in version v5.2)
   It supports delivering PJON packets over Ethernet TCP on local network (LAN),
   as a MQTT protocol client. This strategy is using the ReconnectingMqttClient
   library: https://github.com/fredilarsen/ReconnectingMqttClient
   
   This strategy works in one of four modes.
   The first two modes are for allowing a PJON bus via MQTT, the first mode is
   "closed" and the second is "open" to use by non-PJON programs.
   The last two modes are for behaving like MQTT devices normally do.

   * "Raw bus mode" will send the binary JSON packets delivered to a topic like
     pjon/device45 (where 45 is a receiver device id). Each device
     will subscribe to a topic with its own name and will receive packets like
     from any other PJON strategy. This strategy requires that all senders and
     receivers are linked with PJON for encoding/decoding, so other systems
     are not easily connected.
   
   * "JSON bus mode" will send JSON packets with to, from and data, delivered 
     to a topic like pjon/device45 (where 45 is a receiver device id). Each 
     device will subscribe to a topic with its own name and will receive
     packets like
       {to:45,from:44,data:"message text sent from device 44 to device 45"}.

   * "Device mirror, translating" mode will not use JSON encapsulation of 
     values, and will publish to its own topic, not the receiver's. It will 
     publish to a "output" folder and subscribe to an "input" folder. An
     outgoing packet with payload "P=44.1,T=22.0" would result in the topics
       pjon/device44/output/temperature, with a value "44.1"
       pjon/device44/output/pressure, with a value "22.0"
     Likewise, a receiving an update of:
       pjon/device44/input/setpoint, with a value "45"
     would result in a packet with payload "S=45".
     This mode supports a translation table to allow short names to be used
     in packets while topic names are longer. For example "T" -> "temperature".
     If no translation table is populated, the same names will be used in
     the packets and the topics.

   * "Device mirror, direct" works like the first device mirror mode, but will
     just pass the payload on without any translation, leaving the formatting
     to the user. It will not split packets into separate topics but transfer 
     the packets as-is to one output topic and from one input topic:
       pjon/device44/output
       pjon/device44/input
     The user sketch will have control of the format used, which can be
     plain text like "P=44.1,T=22.0" or a JSON text.

   The "Translate" in the strategy name is because a translation table can be
   used to translate PJON packet contents to MQTT topics and back. This is to 
   enable PJON packets to remain small ("t=44.3") between devices with limited 
   memory, while the MQTT packets are made more explicit ("temperature") to
   support longer name syntax in external systems.
  
   Compliant with the PJON protocol layer specification v0.3
   _____________________________________________________________________________

    MQTTTranslate strategy proposed and developed by Fred Larsen 07/12/2019

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
#include <PJONDefines.h>
#include <ReconnectingMqttClient.h>

#define MQTTT_DEFAULT_PORT                   1883
#ifndef MQTTT_RESPONSE_TIMEOUT
  #define MQTTT_RESPONSE_TIMEOUT  (uint32_t) 10000
#endif

// This is the maximum size of MQTT packets after translation
#ifndef MQTTT_BUFFER_SIZE
  #define MQTTT_BUFFER_SIZE  (uint32_t) PJON_PACKET_MAX_LENGTH
#endif

// Max size of key and value in MQTTT_MODE_MIRROR_TRANSLATE mode
#ifndef MQTTT_KEY_SIZE
  #define MQTTT_KEY_SIZE 30
#endif
#ifndef MQTTT_VALUE_SIZE
  #define MQTTT_VALUE_SIZE 30
#endif

#define MQTTT_MODE_BUS_RAW 0
#define MQTTT_MODE_BUS_JSON 1
#define MQTTT_MODE_MIRROR_TRANSLATE 2
#define MQTTT_MODE_MIRROR_DIRECT 3

// Select which mode to use
#ifndef MQTTT_MODE
  #define MQTTT_MODE MQTTT_MODE_BUS_RAW
#endif

class MQTTTranslate {
    bool last_send_success = false;
    
    uint16_t incoming_packet_size = 0;
// TODO: Eliminate extra buffer -- is the on in the MqttClient not enough?    
    uint8_t packet_buffer[MQTTT_BUFFER_SIZE];
    PJON_Packet_Info _packet_info; // Used for parsing incoming and outgoing packets

    #if (MQTTT_MODE == MQTTT_MODE_MIRROR_TRANSLATE)
    char key[MQTTT_KEY_SIZE];
    char value[MQTTT_VALUE_SIZE];
    #endif

    static void static_receiver(const char *topic, const uint8_t *payload, uint16_t len, void *callback_object) {
      if (callback_object) ((MQTTTranslate*)callback_object)->receiver(topic, payload, len);
    }

    void receiver(const char *topic, const uint8_t *payload, uint16_t len) {
      #if (MQTTT_MODE == MQTTT_MODE_BUS_RAW)
      if(len <= MQTTT_BUFFER_SIZE) {
        memcpy(packet_buffer, payload, len);
        incoming_packet_size = len;
      }
      #endif
      #if (MQTTT_MODE == MQTTT_MODE_BUS_JSON)
      // Must assume that payload is text, unless UUencoding/base64encoding
      // {"to": to_id, "from": from id, "data": "payload"}
      // const char *p = (const char *)payload;
      // TODO: Finish parsing JSON
      #endif
      #if (MQTTT_MODE == MQTTT_MODE_MIRROR_TRANSLATE || MQTTT_MODE == MQTTT_MODE_MIRROR_DIRECT)
      // Parse topic to get source device id
      uint8_t receiver_id = my_id;
      const char *device_start = strstr(topic, "/device");
      if (device_start) receiver_id = (uint8_t) atoi(&device_start[7]);
      #endif
      #if (MQTTT_MODE == MQTTT_MODE_MIRROR_TRANSLATE)
      // Split into multiple topics (must assume a specific payload format to parse):
      // "T=44.1,P=1.1"  ->
      // pjon/device44/output/temperature  44.1
      // pjon/device44/output/pressure     1.1
      if (device_start) {
        // Find start of /input/
        const char *start = (const char*)memchr(device_start+1, '/', (const char*)payload-device_start+len-2);
        // Find end of /input/
        if (start) start = (const char*)memchr(start+1, '/', (const char*)payload-start+len-2);
        if (start) { // Get variable name
          uint8_t l = min(start - device_start + len -1, sizeof key -1);
          strncpy(key, start+1, l);
          key[l] = 0; // Null terminate
          l = min(len, sizeof value-1);
          strncpy(value, (const char*)payload, l);
          value[l] = 0; // Null terminate
          String s = key; s += "="; s += value;
          // Package the key=value into a PJON packet
          incoming_packet_size = PJONTools::compose_packet(receiver_id, bus_id, receiver_id,
            bus_id, packet_buffer, s.c_str(), s.length()+1, header);
        }
      }
      return;
      #endif
      #if (MQTTT_MODE == MQTTT_MODE_MIRROR_DIRECT)
      // Package the payload as it is, into a PJON packet
      incoming_packet_size = PJONTools::compose_packet(receiver_id, bus_id, receiver_id,
        bus_id, packet_buffer, payload, len, header);
      #endif
    };
public:
    ReconnectingMqttClient mqttclient;
    bool retain = false; // Leave message in broker for clients to receive at connect
    uint8_t qos = 0; // Set this to 1 to have guaranteed delivery
    String topic = "pjon"; // e.g. "pjon" without trailing slash
    uint8_t my_id = PJON_NOT_ASSIGNED;
    uint8_t bus_id[4] = {0,0,0,0};
    uint8_t header = 0;
    bool lowercase_topics = true;
    
    void set_config(uint8_t id, const uint8_t bus_id[], uint8_t header) {
      my_id = id; 
      if (bus_id != NULL) memcpy(this->bus_id, bus_id, 4);
      this->header = header;
    }
    void set_qos(uint8_t qos) { this->qos = qos; }
    void set_topic(const char *topic) { this->topic = topic; }

    /* Returns the suggested delay related to the attempts passed as parameter: */

    uint32_t back_off(uint8_t attempts) { return 10000; };


    /* Begin method, to be called on initialization */

    bool begin(uint8_t device_id = 0) {
      my_id = device_id;
      mqttclient.set_receive_callback(static_receiver, this);
      String sub_topic = topic + "/device" + device_id;
      #if (MQTTT_MODE == MQTTT_MODE_MIRROR_TRANSLATE)
      sub_topic += "/input/+"; // All input topics
      #endif
      #if (MQTTT_MODE == MQTTT_MODE_MIRROR_DIRECT)
      sub_topic += "/input"; // Only one input topic
      #endif
      mqttclient.subscribe(sub_topic.c_str(), qos);
      return mqttclient.connect();
    };


    /* Check if the channel is free for transmission */

    bool can_start() { return mqttclient.connect(); };


    /* Returns the maximum number of attempts for each transmission: */

    static uint8_t get_max_attempts() { return 0; };


    /* Handle a collision (empty because handled on Ethernet level): */

    void handle_collision() { };


    /* Receive a frame: */

    uint16_t receive_frame(uint8_t *data, uint16_t max_length) {
      if (incoming_packet_size == 0) mqttclient.update();
      if (incoming_packet_size > 0 && incoming_packet_size <= max_length) {
        memcpy(data, packet_buffer, incoming_packet_size);
        uint16_t len = incoming_packet_size;
        incoming_packet_size = 0; // Flag as handled
        return len;
      }
      return PJON_FAIL;
    }


    /* Receive byte response */

    uint16_t receive_response() {
      return last_send_success ? PJON_ACK : PJON_FAIL;
    };


    /* Send byte response to package transmitter.
       We have the IP so we can skip broadcasting and reply directly. */

    void send_response(uint8_t response) { // Empty
    };


    /* Send a frame: */

    void send_frame(uint8_t *data, uint16_t length) {
      // Extract some info from the packet header
      PJONTools::parse_header(data, _packet_info);

      // Compose topic
// TODO: Wath buffer size      
      uint8_t len = strlen(topic.c_str());
      strcpy(mqttclient.topic_buf(), topic.c_str());
      char *p = &mqttclient.topic_buf()[len];
      strcpy(p, "/device"); p += 7;
      #if (MQTTT_MODE == MQTTT_MODE_MIRROR_TRANSLATE || MQTTT_MODE == MQTTT_MODE_MIRROR_DIRECT)
      itoa(_packet_info.sender_id, p, 10);
      strcat(p, "/output"); // like pjon/device44/output
      p = &p[strlen(p)]; // End of /output
      #else // One of the bus modes, publish to receiver device
      itoa(_packet_info.receiver_id, p, 10);
      #endif
      #if (MQTTT_MODE != MQTTT_MODE_BUS_RAW)
      uint8_t overhead = PJONTools::packet_overhead(_packet_info.header);
      uint8_t crc_size = PJONTools::crc_overhead(_packet_info.header);  
      #endif
      
      // Re-compose packet in other modes than RAW
      #if (MQTTT_MODE == MQTTT_MODE_MIRROR_TRANSLATE)
      // Split into multiple topics (must assume a specific payload format to parse):
      // "T=44.1,P=1.1"  ->
      // pjon/device44/output/temperature  44.1
      // pjon/device44/output/pressure     1.1
      uint8_t send_cnt = 0;
      const char *d = (const char*)&data[overhead - crc_size], *v = d, *c, *e;
      uint16_t plen = length - overhead;
      while (v && (c = find_value_separator(v, d-v+plen))) {
        if (e = (const char *)memchr(v, '=', (uint16_t)(c-v))) {
          uint8_t l = min(e-v, sizeof key-1);
          strncpy(key, v, l); // Complete topic like /pjon/device44/output/temperature
          key[l] = 0;
          if (lowercase_topics) for (uint8_t i=0; i<l; i++) key[i] = tolower(key[i]);
          l = min(c-e-1, sizeof value-1);
          strncpy(value, e+1, l);
          value[l] = 0;
          *p = '/';
          translate(key, sizeof key, true);
          strcpy(p+1, key);
          send_cnt += mqttclient.publish(mqttclient.topic_buf(), (uint8_t*)value, strlen(value), retain, qos);
          v = c-d >= plen ? NULL : c+1;
        }
      }
      last_send_success = send_cnt > 0;
      return; // We have sent multiple smaller packets, just return
      #endif
      #if (MQTTT_MODE == MQTTT_MODE_MIRROR_DIRECT)
      // Post just the payload as it is, to the output topic
      data = &data[overhead - crc_size];
      length -= overhead;
      #endif
      #if (MQTTT_MODE == MQTTT_MODE_BUS_JSON)
      // Must assume that payload is text, unless UUencoding/base64encoding
      // {"to": to_id, "from": from id, "data": "payload"}
      p = (char *) packet_buffer;;
      strcpy(p, "{\"to\":"); p += 6;      
      itoa(_packet_info.receiver_id, p, 10);
      p = &p[strlen(p)];
      strcpy(p, ",\"from\":"); p+= 8;
      itoa(_packet_info.sender_id, p, 10);
      p = &p[strlen(p)];
      strcpy(p, ",\"data\":\""); p+= 9;
      uint8_t payload_len = length - overhead;
      strncpy(p, (const char*)&data[overhead - crc_size], payload_len); p+= payload_len;
      strcpy(p, "\"}"); p += 2;
      data = packet_buffer;
      length = ((uint8_t*)p - packet_buffer);
      #endif

      // Publish
      last_send_success = mqttclient.publish(mqttclient.topic_buf(), data, length, retain, qos);
    };

    void translate(char *key, uint8_t len, bool to_mqtt) {
    }

    const char *find_value_separator(const char *value, uint16_t len) {
      // This does the job of a strchr but accepting that null-terminator may be missing
      const char *p = value;
      while (p != NULL && (p-value < len) && *p != ',' && *p != 0) p++;
      return p;
    }
};
