
/* LEDAR open-source reflectomoter
   Giovanni Blu Mitolo 2017 */

#define LEDAR_VERSION           0

#define PJON_PACKET_MAX_LENGTH 20
#define PJON_MAX_PACKETS        2
#include <PJON.h>
#include <EEPROM.h>

#define LEDAR_EMITTER_PIN       0
#define LEDAR_RECEIVER_PIN     A1

// LEDAR by default in active mode
#define LEDAR_MODE              1
// Transmission interval by default 0.1 seconds
#define LEDAR_INTERVAL        100
// Reading iterations by default 500
#define LEDAR_READINGS        500
// Presence detection threshold 2500
#define LEDAR_THRESHOLD      5000
// Block incoming configuration
#define LEDAR_ACCEPT_CONFIG  true

bool mode;
bool accept_config_change;
uint16_t interval;
uint16_t readings;
uint8_t  recipient_id;
uint16_t threshold;
uint32_t reading = 0;
char packet[4] = {0, 0, 0, 0};

// <Strategy name> bus(selected device id)
PJON<SoftwareBitBang> bus(PJON_NOT_ASSIGNED);

void setup() {
  // Writing default configuration in EEPROM
  if(
    EEPROM.read(11) != 'L' ||
    EEPROM.read(12) != 'E' ||
    EEPROM.read(13) != 'D' ||
    EEPROM.read(14) != 'A' ||
    EEPROM.read(15) != 'R' ||
    EEPROM.read(16) != LEDAR_VERSION
  ) EEPROM_write_default_configuration();
  else EEPROM_read_configuration();
  // Emitter and receiver pin mode configuration
  pinMode(LEDAR_EMITTER_PIN, OUTPUT);
  digitalWrite(LEDAR_EMITTER_PIN, LOW);
  pinMode(LEDAR_RECEIVER_PIN, INPUT);
  digitalWrite(LEDAR_RECEIVER_PIN, LOW);
  // Internal analog reference 1.1v
  analogReference(INTERNAL);
  // PJON bus Initialization
  bus.strategy.set_pin(1);
  bus.begin();
  bus.set_receiver(receiver_function);
  // Initial long reception timeframe to accept incoming configuration
  bus.receive(2000000);
};

void EEPROM_read_configuration() {
  mode = EEPROM.read(0);
  bus.set_id(EEPROM.read(1));
  recipient_id = EEPROM.read(2);
  readings = EEPROM.read(3) << 8 | EEPROM.read(4) & 0xFF;
  interval = EEPROM.read(5) << 8 | EEPROM.read(6) & 0xFF;
  threshold = EEPROM.read(7) << 8 | EEPROM.read(8) & 0xFF;
  bus.config = EEPROM.read(9);
  accept_config_change = EEPROM.read(10);
};

void EEPROM_write_default_configuration() {
  // Mode
  EEPROM.update(0, LEDAR_MODE);
  // LEDAR ID
  EEPROM.update(1, PJON_NOT_ASSIGNED);
  // Recipient ID
  EEPROM.update(2, PJON_MASTER_ID);
  // Reading iterations
  EEPROM.update(3, (uint16_t)LEDAR_READINGS >> 8);
  EEPROM.update(4, (uint16_t)LEDAR_READINGS & 0xFF);
  // Transmission interval
  EEPROM.update(5, (uint16_t)LEDAR_INTERVAL >> 8);
  EEPROM.update(6, (uint16_t)LEDAR_INTERVAL & 0xFF);
  // Detection threshold
  EEPROM.update(7, (uint16_t)LEDAR_THRESHOLD >> 8);
  EEPROM.update(8, (uint16_t)LEDAR_THRESHOLD & 0xFF);
  // Default PJON configuration
  EEPROM.update(9, PJON_TX_INFO_BIT | PJON_ACK_REQ_BIT);
  // Accept incoming configuration
  EEPROM.update(10, LEDAR_ACCEPT_CONFIG);
  // LEDAR footprint
  EEPROM.update(11, 'L');
  EEPROM.update(12, 'E');
  EEPROM.update(13, 'D');
  EEPROM.update(14, 'A');
  EEPROM.update(15, 'R');
  EEPROM.update(16, LEDAR_VERSION);
};

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
  if(
      accept_config_change && (
      packet_info.sender_id == recipient_id ||
      packet_info.sender_id == PJON_MASTER_ID
    )
  ) {
    // Configuration update
    if(payload[0] == 'C') {
      bus.config = payload[1];
      EEPROM.update(9, payload[1]);
    }
    // Detection threshold update
    if(payload[0] == 'D') {
      threshold = payload[1] << 8 | payload[2] & 0xFF;
      EEPROM.put(7, (uint16_t)threshold);
    }
    // Execute reading request
    if(payload[0] == 'E') {
      get_reflex(readings);
      bus.send_packet(recipient_id, packet, 2);
    }
    // Id configuration update
    if(payload[0] == 'I') {
      bus.set_id(payload[1]);
      EEPROM.update(1, payload[1]);
    }
    // Reading iterations update
    if(payload[0] == 'L') {
      readings = payload[1] << 8 | payload[2] & 0xFF;
      EEPROM.update(3, payload[1] << 8);
      EEPROM.update(4, payload[2] & 0xFF);
    }
    // Mode configuration
    if(payload[0] == 'M') {
      mode = payload[1];
      EEPROM.update(0, mode);
    }
    // STOP ACCEPTING INCONMING CONFIGURATION UPDATE
    if(payload[0] == 'Q') {
      EEPROM.update(10, payload[1]);
      accept_config_change = payload[1];
    }
    // Recipient id configuration
    if(payload[0] == 'R') {
      recipient_id = payload[1];
      EEPROM.update(2, recipient_id);
    }
    // Interval configuration update
    if(payload[0] == 'T') {
      interval = payload[1] << 8 | payload[2] & 0xFF;
      EEPROM.update(5, payload[1] << 8);
      EEPROM.update(6, payload[2] & 0xFF);
    }
    // Configuration reset to default
    if(payload[0] == 'X') {
      EEPROM_write_default_configuration();
      EEPROM_read_configuration();
    }
  }
};

void get_reflex(uint16_t iterations) {
  uint32_t ambient = 0;
  uint32_t reflex = 0;
  for (uint16_t i = 0; i < iterations; i++)
    ambient = ambient + analogRead(LEDAR_RECEIVER_PIN);
  digitalWrite(LEDAR_EMITTER_PIN, HIGH);
  for (uint16_t i = 0; i < iterations; i++)
    reflex = reflex + analogRead(LEDAR_RECEIVER_PIN);
  digitalWrite(LEDAR_EMITTER_PIN, LOW);
  if(ambient > reflex) reflex = 0;
  else reflex = reflex - ambient;
  packet[0] = (uint16_t)reflex >> 8;
  packet[1] = (uint16_t)reflex & 0xff;
  reading = reflex;
};

void loop() {
  get_reflex(readings);
  if(mode) {
    bus.send_packet(recipient_id, packet, 2);
    bus.receive((uint32_t)(interval) * 1000);
  } else {
    if(reading > threshold)
      bus.send_packet(recipient_id, packet, 2);
    bus.receive((uint32_t)(interval) * 1000);
  }
};
