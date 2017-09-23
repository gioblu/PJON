/* This WiFiLink class transfers packets of data over
   TCP/IP Ethernet connections.

  It may accept incoming connections and send ACK back for each incoming
  package, it may connect to another object in another device and deliver a
  packet and get an ACK back, or it may do both -- connect to another device
  for delivering packages and accept incoming connections and packages for
  bidirectional transfer.

  When establishing connections, the target must be registered with the add_node
  function for setting the IP and port along with a unique id that is used for
  referring to the target in all other functions.

  An WiFiLink may work in multiple ways when bidirectional:

  1. One-to-one connection with another WiFiLink. In this mode, using the
     keep_connection is recommended because it reduces network activity and
     speeds things up dramatically. In this mode, there are two options:

    A. When running on a firewall-free network one socket can be created in each
       direction, allowing for bidirectional "full-duplex" (as far as that goes
       for single-threaded code), with data being buffered in each direction. It
       requires each side to know about each other's address, but delivers
       higher throughput than a single_socket approach. An WiFiLink object
       in this mode has one node added by add_node, and both sides calls
       start_listening to accept incoming connections. If there are multiple
       objects on one device, each has to listen to a unique port number
       (specified in the start_listening function call).

    B. Set single_socket to mave a master-slave scenario where one side, the
       initiator, creates a socket connection and sends any packages it may
       have and then receives any package waiting to be sendt in the opposite
       direction. This requires the initiator to know where to connect, but not
       the other way around. This is suitable for connections through the
       Internet because a firewall opening only has to be opened in one place.
       An WiFiLink object in this mode has one node added by add_node
       (the initiator side), or calls the start_listening function to start
       listening (the receiver side). If there are multiple receiving objects
       on one device, each has to listen to a unique port number
       (specified in the start_listening function call).

  2. If running a one-to-many or many-to-many scenario the keep_connection
     should be deactivated because the Ethernet library limites the number of
     incoming connections per WiFiLink object to one, so keeping the sockets
     permanently connected prohibits this. Not using keep_connection drops
     the transfer speed a lot, though.

    A. When running on a firewall-free network each packet is delivered through
       a dedicated connection created for delivering that package. After
       delivery and ACK the socket is closed. All devices can send to and
       receive from each other.

    B. Using the single_socket approach will use a master-slave like approach
       where only the initiators need to know the address of the receiver(s),
       for easier firewall traversal. After the two-way transfer of packets on
       the same connection, the connection will be closed so that the receiver
       is ready for another connection. This requires only _one firewall
       opening_ for bidirectional transfer with 255 others.

  Limitations for the different modes when using the W5100 based Ethernet shield
  with a 4 socket limit:

  1A. Two WiFiLink objects can be used for bidirectional communication
      with two sites.

  1B. Four WiFiLink objects can be used for bidirectional communication
      with four sites.

  2A. As the sockets are only used temporarily each object can accept
      connections from 255 others, and deliver packets to 255 others. Up to
      two objects can be used in one device, if multiple listening ports are
      needed. Each object can have 1 to 255 nodes added by add_node, and can
      deliver to all. There can be up to three objects in each device if needed.
      One socket must remain free for outgoing connections.
  2B. As 2A, each receiver can receive packets from and send packets to 255
      initiators, using a _single listening port_. There can be four receiver
      objects in on device, or three receiver objects and an unlimited number
      of initiator objects that will be able to send to and receive from to up
      to 255 sites each.

  Limitations for the different modes when using the ENC28J60 based Ethernet
  shield with a 1 socket limit:

  1A. Not available.
  1B. One WiFiLink object can be used for fast bidirectional communication
      with one site.
  2A. Not available.
  2B. One receiver can receive from and deliver packets to unlimited initiators,
      _or_ unlimited initator objects can send to and receive from to up to
      255 sites each

  Note: To use the ENC28J60 shield instead of the WIZ5100 shield, include
  <UIPEthernet.h> before WiFiLink.h.

  OSPREY, PJON and WiFiLink
  The WiFiLink can be used standalone for simple delivery between two
  devices. But when using WiFiLink objects with OSPREY routing logic
  combined with PJON communication between devices, the true potential can
  be reached. A site consisting of one or more buses of devices communicating
  through PJON, wired and/or wirelessly, can be connected seamlessly with
  multiple other similar sites in other places of the world, communicating
  through Internet with minimal firewall configuration.

  Scenarios:

  - A site with multiple devices that want to communicate directly through the
    LAN: Use WiFiLink directly, with approach 1A for best performance or 1B
    for less configuration (only the initiators will need to know the address of
    the receiver(s)).

  - Multiple sites connected through a LAN, with each site having one or more
    PJON buses with mutiple devices communicating wired or wirelessly. Each site
    has at least one device running OSPREY+WiFiLink plus an Ethernet shield,
    and these OSPREY routers will make sure that every device can deliver
    packets to any other device on any site as long as the device and bus
    numbers are known. WiFiLink object with configuration 1A should be used
    for best performance, or 1B for less configuration, or if the number of
    sites exceed the limits, configuration 2A or 2B can be used.

  - A main site with a receiver, and multiple remote sites that connect through
    the Internet. Configuration 2B requires one firewall opening to be present
    on the main site, and the initators are only establishing outgoing
    connections not requiring firewall openings. Each of the WiFiLink
    objects are used in an OSPREY device. Each Internet-connected site including
    the main site can consist of one or more PJON buses connected with OSPREY
    PJONLink or through LAN based OSPREY WiFiLink routing. Each device on
    any bus can deliver packets to any device on any other bus. Devices on
    non-main sites can either open firewall openings for direct communication,
    or deliver packets to each other through the Internet-attached OSPREY router
    on the main site.

  NOTE: The W5100 full-size Ethernet shields come in multiple variants. If you
        get a problem where the card is not starting after plugging in the
        power, check if there is small resistor network behind the Ethernet
        outlet, with one resistor having printed "511" on it. If so, try
        another brand. Even startup delays do not fix this, but it can be solved
        with a capacitor+resistor, search for it.

  NOTE: If not needing SINGLE_SOCKET functionality, the program size can be
        reduced by defining NO_SINGLE_SOCKET: #define NO_SINGLE_SOCKET */

/* Internal progress notes (can be ignored):

 DONE:
 1. Bidirectional communication with single-socket connections, to utilize the
    max number of sockets better, and to make firewall traversal easier than
    with two-ways socket connections.
 2. Support ENC28J60 based Ethernet shields. Cheap and compact.
    But only one socket.

 TODO:
 1. Focus on single_socket without keep_connection from many sites to a master
    site, with the master site using OSPREY to forward packets between the
    networks! Should be possible out of the box.
 2. Make it possible to accept multiple incoming connections also
    with keep_connection. This would make it possible for one Link to accept
    connections from multiple links without having created a dedicated Link for
    each, just like without keep_connection, but with the speed gain. This
    requires modifying or skipping the WiFiServer/Client classes because
    only one socket per listening port is possible with these.
 3. Checksum. Is it needed, as this is also handled on the network layer?
 4. Retransmission if not ACKED. Or leave this to caller, as the result is
    immediately available?
 5. FIFO queue class common with PJON? Or skip built-in queue?
 6. Call error callback at appropriate times with appropriate codes. Only
    PJON_FAIL and TIMEOUT relevant?
 7. Encryption. Add extra optional encryption key parameter to add_node, plus
    dedicated function for server. */

#pragma once

#include <ESP8266WiFi.h>

// Constants
#ifndef PJON_ACK
  #define PJON_ACK                           6
#endif

#ifndef PJON_NAK
  #define PJON_NAK                          21
#endif

// Internal constants
#ifndef PJON_FAIL
  #define PJON_FAIL                      0x100
#endif

#define ETCP_MAX_REMOTE_NODES               10
#define ETCP_DEFAULT_PORT                 7000

// Magic number to verify that we are aligned with telegram start and end
#define ETCP_HEADER               0x18ABC427ul
#define ETCP_FOOTER               0x9ABE8873ul
#define ETCP_SINGLE_SOCKET_HEADER 0x4E92AC90ul
#define ETCP_SINGLE_SOCKET_FOOTER 0x7BB1E3F4ul

/* UIPEthernet library used for the ENC28J60 based Ethernet shields has the
   correct return value from the read call, while the standard Ethernet library
   does not follow the standard! */
#ifdef UIPETHERNET_H
  #define ETCP_ERROR_READ                   -1
#else
  #define ETCP_ERROR_READ                    0
#endif

typedef void (*link_receiver)(
  uint8_t id,
  const uint8_t *payload,
  uint16_t length,
  void *callback_object
);

typedef void (*link_error)(
  uint8_t code,
  uint8_t data
);

class WiFiLink {
private:
  link_receiver _receiver = NULL;
  link_error    _error = NULL;
  void *_callback_object = NULL;

  // Local node
  uint8_t  _local_id = 0;
  uint8_t  _local_ip[4];
  uint16_t _local_port = ETCP_DEFAULT_PORT;

  // Remote nodes
  uint8_t  _remote_node_count = 0;
  uint8_t  _remote_id[ETCP_MAX_REMOTE_NODES];
  uint8_t  _remote_ip[ETCP_MAX_REMOTE_NODES][4];
  uint16_t _remote_port[ETCP_MAX_REMOTE_NODES];

  WiFiServer *_server = NULL;

  WiFiClient _client_out;
  // Created as an outgoing connection

  WiFiClient _client_in;
  // Accepted incoming connection

  int16_t _current_device = -1;
  // The id of the remove device/node that we have connected to

  bool _keep_connection = false;
  // Keep sockets permanently open instead of reconnecting for each transfer

  bool _single_socket = false;
  // Do bidirectional transfer on a single socket

  void init() {
    memset(_local_ip, 0, 4);
    for(uint8_t i = 0; i < ETCP_MAX_REMOTE_NODES; i++) {
      _remote_id[i] = 0;
      memset(_remote_ip[i], 0, 4);
      _remote_port[i] = 0;
    }
  };


  int16_t find_remote_node(uint8_t id) {
    for(uint8_t i = 0; i < ETCP_MAX_REMOTE_NODES; i++)
      if(_remote_id[i] == id)
        return i;
    return -1;
  };


  int16_t read_bytes(
    WiFiClient &client,
    uint8_t *contents,
    uint16_t length
  ) {
    uint16_t total_bytes_read = 0, bytes_read;
    uint32_t start_ms = millis();
    int16_t avail;
    /* NOTE: The recv/read functions returns
       -1 if no data waiting
        0 if socket closed */
    do {
      while(
        (avail = client.available()) <= 0 &&
        client.connected() &&
        (uint32_t)(millis() - start_ms) < 10000
      );

      bytes_read = client.read(
        &contents[total_bytes_read],
        constrain(avail, 0, length - total_bytes_read)
      );

      if(bytes_read > 0)
        total_bytes_read += bytes_read;
    } while(
      bytes_read != ETCP_ERROR_READ &&
      total_bytes_read < length &&
      (uint32_t)(millis() - start_ms) < 10000
    );
    if(bytes_read == ETCP_ERROR_READ) stop(client); // Lost connection
    return total_bytes_read;
  };


  // Read a package from a connected client (incoming or outgoing) and send ACK

  uint16_t receive(WiFiClient &client) {
    int16_t return_value = PJON_FAIL;
    if(client.available() > 0) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.println("Recv from cl");
      #endif

      // Locate and read encapsulation header (4 bytes magic number)
      bool ok = read_until_header(client, ETCP_HEADER);
      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Read header, stat ");
        Serial.println(ok);
      #endif

      // Read sender device id (1 byte) and length of contents (4 bytes)
      int16_t bytes_read = 0;
      uint8_t sender_id = 0;
      uint32_t content_length = 0;
      if(ok) {
        byte buf[5];
        bytes_read = read_bytes(client, buf, 5);
        if(bytes_read != 5) ok = false;
        else {
          memcpy(&sender_id, buf, 1);
          memcpy(&content_length, &buf[1], 4);
          if(content_length == 0) ok = 0;
        }
      }

      // Read contents and footer
      uint8_t buf[content_length];
      if(ok) {
        bytes_read = read_bytes(client, buf, content_length);
        if(bytes_read != content_length) ok = false;
      }

      // Read footer (4 bytes magic number)
      if(ok) {
        uint32_t foot = 0;
        bytes_read = read_bytes(client, (byte*) &foot, 4);
        if(bytes_read != 4 || foot != ETCP_FOOTER) ok = false;
      }

      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Stat bfr send PJON_ACK: ");
        Serial.println(ok);
      #endif

      // Write PJON_ACK
      return_value = ok ? PJON_ACK : PJON_NAK;
      int8_t acklen = 0;
      if(ok) {
        acklen = client.write((byte*) &return_value, 2);
        if(acklen == 2) client.flush();
      }

      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Sent ");
        Serial.print(ok ? "PJON_ACK: " : "PJON_NAK: ");
        Serial.println(acklen);
      #endif

      // Call receiver callback function
      if(ok && content_length > 0)
        _receiver(sender_id, buf, content_length, _callback_object);
    }
    return return_value;
  };


  bool connect(uint8_t id) {
    // Locate the node's IP address and port number
    int16_t pos = find_remote_node(id);
    #ifdef ETCP_DEBUG_PRINT
      Serial.print("Send to srv pos=");
      Serial.println(pos);
    #endif

    // Break existing connection if not connected to the wanted server
    bool connected = _client_out.connected();
    if(connected && _current_device != id) {
      // Connected, but to the wrong device
      #ifdef ETCP_DEBUG_PRINT
        //if(_keep_connection && _current_device != -1)
        Serial.println("Switch conn to another srv");
      #endif
      stop(_client_out);
      _current_device = -1;
      connected = false;
    }
    if(pos < 0) return false;

    // Try to connect to server if not already connected
    if(!connected) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.println("Conn..");
      #endif
      connected = _client_out.connect(_remote_ip[pos], _remote_port[pos]);
      #ifdef ETCP_DEBUG_PRINT
        Serial.println(connected ? "Conn to srv" : "Failed conn to srv");
      #endif
      if(!connected) {
        stop(_client_out);
        _current_device = -1;
        return false; // Server is unreachable or busy
      }
      _current_device = id; // Remember who we are connected to
    }

    return connected;
  };


  void stop(WiFiClient &client) {
    client.stop();
  };


  bool accept() {
    // Accept new incoming connection if connection has been lost
    bool connected = _client_in.connected();
    if(!_keep_connection || !connected) {
      stop(_client_in);
      _client_in = _server->available();
      connected = _client_in;
      #ifdef ETCP_DEBUG_PRINT
        if(connected) Serial.println("Accepted");
      #endif
    }
    return connected;
  };


  void disconnect_out_if_needed(int16_t result) {
    if(result != PJON_ACK || !_keep_connection) {
      stop(_client_out);
      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Disconn outcl. OK=");
        Serial.println(result == PJON_ACK);
      #endif
    }
  };


  bool disconnect_in_if_needed() {
    bool connected = _client_in.connected();
    if(!_keep_connection || !connected) {
      #ifdef ETCP_DEBUG_PRINT
        if(connected) Serial.println("Disc. inclient.");
      #endif
      stop(_client_in);
    }
  };


  uint16_t send(
    WiFiClient &client,
    uint8_t id,
    const char *packet,
    uint16_t length
  ) {
    // Assume we are connected. Try to deliver the package
    uint32_t head = ETCP_HEADER, foot = ETCP_FOOTER, len = length;
    byte buf[9];
    memcpy(buf, &head, 4);
    memcpy(&buf[4], &id, 1);
    memcpy(&buf[5], &len, 4);
    bool ok = client.write(buf, 9) == 9;
    if(ok) ok = client.write((byte*) packet, length) == length;
    if(ok) ok = client.write((byte*) &foot, 4) == 4;
    if(ok) client.flush();

    #ifdef ETCP_DEBUG_PRINT
      Serial.print("Write stat: ");
      Serial.println(ok);
    #endif

    /* If the other side is sending as well, we need to allow it to be read and
       acknowledged, otherwise we have a deadlock where both are waiting for ACK
       and will time out unsuccessfully */
    if(!_single_socket && _server) receive();

    // Read ACK
    int16_t result = PJON_FAIL;
    if(ok) {
      uint16_t code = 0;
      ok = read_bytes(client, (byte*) &code, 2) == 2;
      if(ok && (code == PJON_ACK || code == PJON_NAK)) result = code;
    }

    #ifdef ETCP_DEBUG_PRINT
      Serial.print("PJON_ACK stat: ");
      Serial.println(result == PJON_ACK);
    #endif

    return result;  // PJON_FAIL, PJON_ACK or PJON_NAK
  };


  /* Do bidirectional transfer of packets over a single socket connection by
     using a master-slave mode where the master connects and delivers packets
     or a placeholder, then reads packets or placeholder back before closing
     the connection (unless letting it stay open). */

  uint16_t single_socket_transfer(
    WiFiClient &client,
    int16_t id,
    bool master,
    const char *contents,
    uint16_t length
  ) {
    #ifndef NO_SINGLE_SOCKET
    #ifdef ETCP_DEBUG_PRINT
      // Serial.print("Single-socket transfer, id=");
      // Serial.print(id);
      // Serial.print(", master=");
      // Serial.println(master);
    #endif
    if(master) { // Creating outgoing connections
      // Connect or check that we are already connected to the correct server
      bool connected = id == -1 ? _client_out.connected() : connect(id);
      #ifdef ETCP_DEBUG_PRINT
        Serial.println(connected ? "Out conn" : "No out conn");
      #endif
      if(!connected) return PJON_FAIL;

      // Send singlesocket header and number of outgoing packets
      bool ok = true;
      uint32_t head = ETCP_SINGLE_SOCKET_HEADER;
      uint8_t numpackets_out = length > 0 ? 1 : 0;
      char buf[5];
      memcpy(buf, &head, 4);
      memcpy(&buf[4], &numpackets_out, 1);
      if(ok) ok = client.write((byte*) &buf, 5) == 5;
      if(ok) client.flush();

      // Send the packet and read PJON_ACK
      if(ok && numpackets_out > 0) {
         ok = send(client, id, contents, length) == PJON_ACK;
         #ifdef ETCP_DEBUG_PRINT
           Serial.print("Sent p, ok=");
           Serial.println(ok);
         #endif
      }

      // Read number of incoming messages
      uint8_t numpackets_in = 0;
      if(ok) ok = read_bytes(client, &numpackets_in, 1) == 1;
      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Read np_in: ");
        Serial.println(numpackets_in);
      #endif

      // Read incoming packages if any
      for(uint8_t i = 0; ok && i < numpackets_in; i++) {
        while(client.available() < 1 && client.connected()) ;
        ok = receive(client) == PJON_ACK;
        #ifdef ETCP_DEBUG_PRINT
          Serial.print("Read p, ok=");
          Serial.println(ok);
        #endif
      }

      // Write singlesocket footer ("PJON_ACK" for the whole thing)
      uint32_t foot = ETCP_SINGLE_SOCKET_FOOTER;
      if(ok) ok = client.write((byte*) &foot, 4) == 4;
      if(ok) client.flush();
      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Sent ss foot, ok=");
        Serial.println(ok);
      #endif

      // Disconnect
      int16_t result = ok ? PJON_ACK : PJON_FAIL;
      disconnect_out_if_needed(result);
      return result;
    } else { // Receiving incoming connections and packets and request
      // Wait for and accept connection
      bool connected = accept();
      #ifdef ETCP_DEBUG_PRINT
        // Serial.println(connected ? "In conn" : "No in conn");
      #endif
      if(!connected) return PJON_FAIL;

      // Read singlesocket header
      bool ok = read_until_header(client, ETCP_SINGLE_SOCKET_HEADER);
      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Read ss head, ok=");
        Serial.println(ok);
      #endif

      // Read number of incoming packets
      uint8_t numpackets_in = 0;
      if(ok) ok = read_bytes(client, (byte*) &numpackets_in, 1) == 1;
      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Read np_in: ");
        Serial.println(numpackets_in);
      #endif

      // Read incoming packets if any, send ACK for each
      for(uint8_t i = 0; ok && i < numpackets_in; i++) {
        while(client.available() < 1 && client.connected()) ;
        ok = receive(client) == PJON_ACK;
        #ifdef ETCP_DEBUG_PRINT
          Serial.print("Read p, ok=");
          Serial.println(ok);
        #endif
      }

      // Write number of outgoing packets
      uint8_t numpackets_out = length > 0 ? 1 : 0;
      if(ok) ok = client.write((byte*) &numpackets_out, 1) == 1;
      if(ok) client.flush();

      // Write outgoing packets if any
      if(ok && numpackets_out > 0) {
        ok = send(client, id, contents, length) == PJON_ACK;
        #ifdef ETCP_DEBUG_PRINT
           Serial.print("Sent p, ok=");
           Serial.println(ok);
        #endif
      }

      // Read singlesocket footer
      if(ok) {
        uint32_t foot = 0;
        ok = read_bytes(client, (byte*) &foot, 4) == 4;
        if(foot != ETCP_SINGLE_SOCKET_FOOTER) ok = 0;
        #ifdef ETCP_DEBUG_PRINT
          Serial.print("Read ss foot, ok=");
          Serial.println(ok);
        #endif
      }

      // Disconnect
      disconnect_in_if_needed();

      return ok ? PJON_ACK : PJON_FAIL;
    }
    #endif
    return PJON_FAIL;
  };


  /* Read until a specific 4 byte value is found.
     This will resync if stream position is lost. */

  bool read_until_header(WiFiClient &client, uint32_t header) {
    uint32_t head = 0;
    int8_t bytes_read = 0;
    bytes_read = read_bytes(client, (byte*) &head, 4);
    if(bytes_read != 4 || head != header) {
      // Did not get header. Lost position in stream?
      do { /* Try to resync if we lost position in the stream
              (throw avay all until ETCP_HEADER found) */
        head = head >> 8;
        // Make space for 8 bits to be read into the most significant byte
        bytes_read = read_bytes(client, &((byte*) &head)[3], 1);
        if(bytes_read != 1) break;
      } while(head != header);
    }
    return head == header;
  };

public:

  WiFiLink() {
    init();
  };


  WiFiLink(uint8_t id) {
    init();
    set_id(id);
  };


  int16_t add_node(
    uint8_t remote_id,
    const uint8_t remote_ip[],
    uint16_t port_number = ETCP_DEFAULT_PORT
  ) {
    // Find free slot
    int16_t remote_id_index = find_remote_node(0);
    if(remote_id_index < 0) return remote_id_index; // All slots taken
    _remote_id[remote_id_index] = remote_id;
    memcpy(_remote_ip[remote_id_index], remote_ip, 4);
    _remote_port[remote_id_index] = port_number;
    _remote_node_count++;
    return remote_id_index;
  };


  void start_listening(uint16_t port_number = ETCP_DEFAULT_PORT) {
    // Do not call for single_socket initiator
    if(_server != NULL) return; // Already started

    #ifdef ETCP_DEBUG_PRINT
      Serial.print("Lst on port ");
      Serial.println(port_number);
    #endif
    _server = new WiFiServer(port_number);
    _server->begin();
  };


  /* Whether to keep outgoing connection live until we need connect to
     another WiFiLink node */

  void keep_connection(bool keep) {
    _keep_connection = keep;
  };


  /* Whether to do bidirectional data transfer on a single socket or use one
     socket for each direction. Single socket transfer is slower but more
     firewall-friendly. Client connects to server, and packets are exchanged in
     both directions on the same socket. If using this, _only one_ of the sides
     should call start_listening, and that side will be receiver with the other
     initiator (establishing connections) */

  void single_socket(bool single_socket) {
    _single_socket = single_socket;
  };


  // Keep trying to send for a maximum duration

  int16_t send_with_duration(
    uint8_t id,
    const char *packet,
    uint16_t length,
    uint32_t duration_us
  ) {
   uint32_t start = PJON_MICROS();
    int16_t result = PJON_FAIL;
    do {
      result = send(id, packet, length);
    } while(
      result != PJON_ACK &&
      (uint32_t)(PJON_MICROS() - start) <= duration_us
    );
    return result;
  };


  /* In single-socket mode and acting as initiator, connect and check for
     incoming packets from a specific device */

  uint16_t poll_receive(uint8_t remote_id) {
    /* Create connection if needed but only poll for incoming packet
       without delivering any */
    if(_single_socket) {
      if(!_server)
        return single_socket_transfer(_client_out, remote_id, true, NULL, 0);
    } else { // Just do an ordinary receive without using the id
      return receive();
    }
    return PJON_FAIL;
  };


  uint8_t get_id() const {
    return _local_id;
  };


  // Overridden functions below -----------------------------------------------

  // Connect to a server if needed, then read incoming package and send ACK

  uint16_t receive() {
    if(_server == NULL) { // Not listening for incoming connections
      if(_single_socket) { // Single-socket mode.
        /* Only read from already established outgoing socket, or create
        connection if there is only one remote node configured (no doubt about
        which node to connect to). */
        int16_t remote_id = _remote_node_count == 1 ? _remote_id[0] : -1;
        return single_socket_transfer(_client_out, remote_id, true, NULL, 0);
      }
    } else {
      // Accept new incoming connection if connection has been lost
      if(_single_socket)
        return single_socket_transfer(_client_in, -1, false, NULL, 0);
      else {
        // Accept incoming connected and receive a single incoming packet
        if(!accept()) return PJON_FAIL;
        uint16_t result = receive(_client_in);
        disconnect_in_if_needed();
        return result;
      }
    }
    return PJON_FAIL;
  };


  uint16_t receive(uint32_t duration_us) {
    uint32_t start = PJON_MICROS();
    int16_t result = PJON_FAIL;
    do {
      result = receive();
    } while(
      result != PJON_ACK &&
      (uint32_t)(PJON_MICROS() - start) <= duration_us
    );
    return result;
  };


  uint16_t send(
    uint8_t id,
    const char *packet,
    uint16_t length,
    uint32_t timing_us = 0
  ) {
    // Special algorithm for single-socket transfers
    if(_single_socket)
      return single_socket_transfer(
        _server ? _client_in : _client_out,
        id,
        _server ? false : true,
        packet,
        length
      );

    // Connect or check that we are already connected to the correct server
    bool connected = connect(id);

    // Send the packet and read PJON_ACK
    int16_t result = PJON_FAIL;
    if(connected) result = send(_client_out, id, packet, length);

    // Disconnect
    disconnect_out_if_needed(result);

    return result;
  };


  uint8_t device_id() {
    return _local_id;
  };


  uint8_t acquire_id() { // Not supported yet
    return 0;
  };


  void set_error(link_error e) {
    _error = e;
  };


  void set_id(uint8_t id) {
    _local_id = id;
  };


  void set_receiver(link_receiver r, void *callback_object) {
    _receiver = r;
    _callback_object = callback_object;
  };


  void update() { };

};
