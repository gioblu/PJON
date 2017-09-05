/* EthernetLink and Ethernet strategies contributed by Fred Larsen

  This EthernetLink class transfers packets of data over
  TCP/IP Ethernet connections.

  It may accept incoming connections and send ACK back for each incoming
  package, it may connect to another object in another device and deliver a
  packet and get an ACK back, or it may do both -- connect to another device
  for delivering packages and accept incoming connections and packages for
  bidirectional transfer.

  When establishing connections target must be registered with the add_node
  function for setting the IP and port along with a unique id that is used for
  referring to the target in all other functions.

  An EthernetLink may work in multiple ways when bidirectional:

  1. One-to-one connection with another EthernetLink. In this mode, using the
     keep_connection is recommended because it reduces network activity and
     speeds things up dramatically. In this mode, there are three options:

    A. When running on a firewall-free network one socket can be created in
       each direction, allowing for bidirectional "full-duplex" (as far as
       that goes for single-threaded code), with data being buffered in each
       direction. It requires each side to know about each other's address
       but delivers higher throughput than a single_socket approach.
       An EthernetLink object in this mode has one node added by add_node and
       both sides calls start_listening to accept incoming connections.
       If there are multiple objects on one device, each has to listen to a
       unique port number (specified in the start_listening function call).

    B. Set single_socket to mave a master-slave scenario where one side, the
       initiator, creates a socket connection and sends any packages it may
       have and then receives any package waiting to be sendt in the opposite
       direction. This requires the initiator to know where to connect but not
       the other way around. This is suitable for connections through the
       Internet because a firewall opening only has to be opened in one place.
       An EthernetLink object in this mode has one node added by add_node
       (the initiator side), or calls the start_listening function to start
       listening (the receiver side). If there are multiple receiving objects
       on one device, each has to listen to a unique port number
       (specified in the start_listening function call).

    C. Set single_initiate_direction to have one device create two sockets to
       the other device using one for each packet direction. This is a variant
       of B) that is firewall friendly and more efficient, but uses one more
       socket, and cannot be run on the simplest Ethernet cards with only one
       available socket. This mode has 3-4 times the speed of B) and does not
       do polling so it will not generate network traffic when idle.

  2. If running a one-to-many or many-to-many scenario the keep_connection
     should be deactivated because then one incoming socket would block others
     from connecting. Not using keep_connection drops transfer speed though.

    A. When running on a firewall-free network each packet is delivered
       through a dedicated connection created for delivering that package.
       After delivery and ACK the socket is closed. All devices can send to
       and receive from each other.

    B. Using the single_socket approach will use a master-slave like approach
       where only the initiators need to know the address of the receiver(s),
       for easier firewall traversal. After the two-way transfer of packets on
       the same connection, the connection will be closed so that the receiver
       is ready for another connection. This requires only _one firewall
       opening_ for bidirectional transfer with 255 others.

  Limitations for the different modes when using the W5100 based Ethernet
  shield with a 4 socket limit:

  1A. Two EthernetLink objects can be used for bidirectional communication
      with two sites.

  1B. Four EthernetLink objects can be used for bidirectional communication
      with four sites.

  2A. As the sockets are only used temporarily each object can accept
      connections from 255 others, and deliver packets to 255 others. Up to
      two objects can be used in one device, if multiple listening ports are
      needed. Each object can have 1 to 255 nodes added by add_node, and can
      deliver to all. There can be up to three objects in each device if
      needed. One socket must remain free for outgoing connections.
  2B. As 2A, each receiver can receive packets from and send packets to 255
      initiators, using a _single listening port_. There can be four receiver
      objects in on device, or three receiver objects and an unlimited number
      of initiator objects that will be able to send to and receive from to up
      to 255 sites each.

  Limitations for the different modes when using the ENC28J60 based Ethernet
  shield with a 1 socket limit:

  1A. Not available.
  1B. One EthernetLink object can be used for fast bidirectional communication
      with one site.
  1C. Not available.
  2A. Not available.
  2B. One receiver can receive from and deliver packets to unlimited
      initiators, _or_ unlimited initator objects can send to and receive
      from to up to 255 sites each

  Note: To use the ENC28J60 shield instead of the WIZ5100 shield, include
  <UIPEthernet.h> before EthernetLink.h.

  PJON and EthernetLink
  The EthernetLink can be used standalone for simple delivery between two
  devices. But the EthernetLink is also used as a tool by the EthernetTCP
  strategy of PJON. A site consisting of one or more buses of devices
  communicating through PJON, wired and/or wirelessly, can be connected
  seamlessly with multiple other similar sites in other places of the
  world, communicating through Internet with minimal firewall configuration.

  NOTE: The W5100 full-size Ethernet shields come in multiple variants. If you
        get a problem where the card is not starting after plugging in the
        power, check if there is small resistor network behind the Ethernet
        outlet, with one resistor having printed "511" on it. If so, try
        another brand. Even startup delays do not fix this, but it can be
        solved with a capacitor + resistor, search for it.

  NOTE: If needing single_socket functionality with ACK (polling mode),
        define ETCP_SINGLE_SOCKET_WITH_ACK. The program size has been reduced
        by only including this when needed.

        The same goes for ETCP_SINGLE_DIRECTION. It is not included by default
        to reduce program size. Define this when needed.
        */

#pragma once

#ifndef UIPETHERNET_H
  #include <Ethernet.h>
#endif

// Constants
#ifndef PJON_ACK
  #define PJON_ACK                           6
#endif

// Internal constants
#ifndef PJON_FAIL
  #define PJON_FAIL                      0x100
#endif

#ifndef ETCP_MAX_REMOTE_NODES
  #define ETCP_MAX_REMOTE_NODES             10
#endif

#define ETCP_DEFAULT_PORT                 7000

/* The maximum packet size to be transferred, this protects again buffer
   overflow. Set this to a size that is guaranteed to be available in RAM
   during runtime, depending on the hardware and software. */
#ifndef ETCP_MAX_PACKET_SIZE
  #define ETCP_MAX_PACKET_SIZE             300
#endif


/* If an incoming packet has not arrived for some time, disconnect the socket
   so it will be reconnected on demand. The timeout is in ms.
   The reason for this is that in some cases an idle socket may have gotten
   disconnected without it being detected, unless trying to write to it.
   So we could be waiting for data that never arrives. */
#ifndef ETCP_IDLE_TIMEOUT
  #define ETCP_IDLE_TIMEOUT 30000ul
#endif

// Magic number to verify that we are aligned with telegram start and end
#define ETCP_HEADER               0x18ABC427ul
#define ETCP_FOOTER               0x9ABE8873ul
#define ETCP_SINGLE_SOCKET_HEADER 0x4E92AC90ul
#define ETCP_SINGLE_SOCKET_FOOTER 0x7BB1E3F4ul

// Primary socket, packets in initiated direction
#define ETCP_CONNECTION_HEADER_A      0xFEDFED67ul
// Same, but request ACK for all packets
#define ETCP_CONNECTION_HEADER_A_ACK  0xFEDFED68ul
// Reverse socket, packets in reverse direction
#define ETCP_CONNECTION_HEADER_B      0xFEDFED77ul

/* UIPEthernet library used for the ENC28J60 based Ethernet shields has the
   correct return value from the read call while the standard Ethernet library
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


class EthernetLink {
private:
  // Dynamic members

  EthernetServer *_server = NULL;

  // Connection for writing outgoing packets
  EthernetClient _client_out;

  // Connection for reading incoming packets
  EthernetClient _client_in;

  // The id of the remove device/node that we have connected to
  int16_t _current_device = -1;

  // When a socket is received, connection header specifies if ACKs are wanted
  bool _ack_requested = false;

  // Remember the connection statistics
  uint32_t _connection_time = 0;
  uint32_t _connection_count = 0;
  uint32_t _last_receive_time = 0;

  // Configuration
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

  // Keep sockets permanently open instead of reconnecting for each transfer
  bool _keep_connection = false;

  // Do bidirectional transfer on a single socket
  bool _single_socket = false;

  // Request immediate ACK for each packet to ensure guaranteed delivery
  bool _request_ack = true;

  /* With single_socket = false there is one socket for each packet direction.
     Normally the sockets are initiated from the side sending the packet.
     By setting initiate_both_sockets_in_same direction, both sockets can be
     initiated from one of the devices, to simplify firewall setup, or for
     letting only one of the devices have a static IP address.
     This should only be used with _keep_connection = true, and is meant for
     permanent one-to-one links. */
  bool _initiate_both_sockets_in_same_direction = false;
  // Whether to be the side initiating both sockets or not
  bool _initiator = true;

public:

  void init() {
    memset(_local_ip, 0, 4);
  };


  int16_t find_remote_node(uint8_t id) {
    for(uint8_t i = 0; i < _remote_node_count; i++)
      if(_remote_id[i] == id)
        return i;
    return -1;
  };


  int16_t read_bytes(
    EthernetClient &client,
    uint8_t *contents,
    uint16_t length
  ) {
    uint16_t total_bytes_read = 0, bytes_read;
    uint32_t start_ms = PJON_MILLIS();
    int16_t avail;
    /* NOTE: The recv/read functions returns
       -1 if no data waiting
        0 if socket closed */
    do {
      while(
        (avail = client.available()) <= 0 &&
        client.connected() &&
        (uint32_t)(PJON_MILLIS() - start_ms) < 10000
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
      (uint32_t)(PJON_MILLIS() - start_ms) < 10000
    );
    if(bytes_read == ETCP_ERROR_READ) {
      stop(client); // Lost connection
      #ifdef ETCP_DEBUG_PRINT
        Serial.println(F("Read failed, closing cl"));
      #endif
    }
    return total_bytes_read;
  };


  // Read package from a connected client (incoming or outgoing) and send ACK
  uint16_t receive(EthernetClient &client) {
    int16_t return_value = PJON_FAIL;
    if(client.available() > 0) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.println(F("Recv from cl"));
      #endif

      // Locate and read encapsulation header (4 bytes magic number)
      bool ok = read_until_header(client, ETCP_HEADER);
      #ifdef ETCP_DEBUG_PRINT
        Serial.print(F("Read header, stat "));
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

      // Protect against too large packets
      if(content_length > ETCP_MAX_PACKET_SIZE) return PJON_FAIL;

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
        Serial.print(F("Stat rec: "));
        Serial.print(ok);
        Serial.print(" len: ");
        Serial.println(content_length);
      #endif

      return_value = ok ? PJON_ACK : PJON_FAIL;
      if(ok) _last_receive_time = PJON_MILLIS();
      if(_ack_requested) {
        // Write PJON_ACK
        int8_t acklen = 0;
        if(ok) {
          acklen = client.write((byte*) &return_value, 2);
          if(acklen == 2) client.flush();
        }

        #ifdef ETCP_DEBUG_PRINT
          Serial.print("Sent ");
          Serial.print(ok ? "PJON_ACK: " : "PJON_FAIL: ");
          Serial.println(acklen);
        #endif
      }

      // Call receiver callback function
      if(ok && content_length > 0)
        _receiver(sender_id, buf, content_length, _callback_object);

      if(!ok) disconnect_in();
    }
    return return_value;
  };


  bool connect(uint8_t id) {
    // Locate the node's IP address and port number
    int16_t pos = find_remote_node(id);

    // Determine if to disconnect
    bool disconnect = !_keep_connection,
         reverse = _initiate_both_sockets_in_same_direction && _initiator;

     // Break existing connection if not connected to the wanted server
     if(!disconnect && _client_out && _current_device != id) {
       // Connected, but to the wrong device
       #ifdef ETCP_DEBUG_PRINT
         Serial.println(F("Switch conn to another srv"));
       #endif
       disconnect = true;
       _current_device = -1;
     }

    // Check if established sockets have been disconnected
    if(!disconnect && _client_out && !_client_out.connected())
      disconnect = true;
    #ifdef ETCP_SINGLE_DIRECTION
    if(!disconnect && reverse && _client_in && !_client_in.connected())
      disconnect = true;
    if(!disconnect && reverse && (!_client_in || !_client_out))
      disconnect = true;
    if(!disconnect && reverse && _client_in && got_receive_timeout()) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.println(F("Receive timeout, disconn."));
      #endif
      disconnect = true;
    }
    #endif
    if(disconnect) disconnect_out();
    // NOTE: From this point we can avoid using the expensive connected() call

    bool connected = _client_out;
    #ifdef ETCP_DEBUG_PRINT
    if(!connected) {
      Serial.print(F("Conn to srv pos="));
      Serial.println(pos);
    }
    #endif

    if(pos < 0) return false;

    // Try to connect to server if not already connected
    bool did_connect = false;
    if(!connected) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.println("Conn..");
      #endif
      connected = _client_out.connect(_remote_ip[pos], _remote_port[pos]);
      #ifdef ETCP_DEBUG_PRINT
        Serial.println(connected ? "Conn to srv" : "Failed conn to srv");
      #endif
      if(connected) {
        // Adjust connection header A to include ACK request flag
        uint32_t conn_header =
          _request_ack ?
            ETCP_CONNECTION_HEADER_A_ACK :
            ETCP_CONNECTION_HEADER_A;
        if(_client_out.write((uint8_t*) &conn_header, 4) != 4) {
          connected = false;
          #ifdef ETCP_DEBUG_PRINT
            Serial.println(F("Disconn. failed write connhead"));
          #endif
        }
      }
      if(!connected) stop(_client_out); else did_connect = true;
    }

    bool connected_rev = false;
    #ifdef ETCP_SINGLE_DIRECTION
    if(connected && reverse) {
      connected_rev = _client_in;
      if(!connected_rev) {
        #ifdef ETCP_DEBUG_PRINT
          Serial.println("Conn rev..");
        #endif
        uint32_t start = PJON_MILLIS();
        do {
          connected_rev =
            _client_in.connect(_remote_ip[pos], _remote_port[pos]);
        } while (!connected_rev && (uint32_t)(PJON_MILLIS()-start) < 2000);
        #ifdef ETCP_DEBUG_PRINT
          Serial.println(
            connected_rev ? F("Conn rev to srv") : F("Failed rev conn to srv")
          );
        #endif
        if(connected_rev) {
           uint32_t conn_header = ETCP_CONNECTION_HEADER_B;
           if(_client_in.write((uint8_t*) &conn_header, 4) != 4) {
             connected_rev = false;
             #ifdef ETCP_DEBUG_PRINT
               Serial.println(F("Disconn rev. failed write connhead"));
             #endif
           }
        }
        if(connected_rev) {
          // ACK active on both sockets or none in this mode
          _ack_requested = _request_ack;
          did_connect = true;
          // Count the connection as a receive action
          _last_receive_time = PJON_MILLIS();
        }
      }
    }
    #endif

    if(did_connect) { // Gather a litte connection information
      _connection_time = PJON_MILLIS();
      _connection_count++;
      if(_single_socket)
        _ack_requested = _request_ack; // Same mode in both directions
    }

    if(!connected || (reverse && !connected_rev)) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.print(F("Fail conn, closing "));
        Serial.print(connected);
        Serial.println(connected_rev);
      #endif
      disconnect_out();
      _current_device = -1;
      delay(1000);  // Slow down if failure
      return false; // Server is unreachable or busy
    }
    else _current_device = id; // Remember who we are connected to

    return true;
  };


  void stop(EthernetClient &client) {
    client.stop();
  };

  void disconnect_in() {
    #ifdef ETCP_DEBUG_PRINT
      if(
        _client_in ||
        (_initiate_both_sockets_in_same_direction && _client_out)
      ) Serial.println(F("Disconn in&rev"));
    #endif
    #ifdef ETCP_SINGLE_DIRECTION
    if(_initiate_both_sockets_in_same_direction && _client_out)
      stop(_client_out);
    #endif
    if(_client_in) stop(_client_in);
  }

  void disconnect_out() {
    #ifdef ETCP_DEBUG_PRINT
      if(
        _client_out ||
        (_initiate_both_sockets_in_same_direction && _client_in)
      ) Serial.println(F("Disconn out&rev"));
    #endif
    if(_client_out) stop(_client_out);
    #ifdef ETCP_SINGLE_DIRECTION
    if(_initiate_both_sockets_in_same_direction && _client_in)
      stop(_client_in);
    #endif
  }

  bool accept() {
    // Determine if to disconnect
    bool disconnect = !_keep_connection,
         reverse = _initiate_both_sockets_in_same_direction && !_initiator;
    if(!disconnect && _client_in && !_client_in.connected())
      disconnect = true;
    #ifdef ETCP_SINGLE_DIRECTION
    if(!disconnect && reverse && _client_out && !_client_out.connected())
      disconnect = true;
    if(!disconnect && reverse && (!_client_in || !_client_out))
      disconnect = true;
    #endif
    if(!disconnect && _client_in && got_receive_timeout()) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.println(F("Receive timeout, disconn."));
      #endif
      disconnect = true;
    }
    if(disconnect) disconnect_in();
    // NOTE: From this point we can avoid using the expensive connected() call

    // Accept new incoming connection
    bool did_connect = false, connected = _client_in;
    if(!connected) {
      _client_in = _server->available();
      connected = _client_in;
      #ifdef ETCP_DEBUG_PRINT
        if(connected) Serial.println(F("Accepted"));
      #endif
      if(connected) {
        uint32_t connection_header = 0;
        bool header_ok = false;
        if(read_bytes(_client_in, (uint8_t*) &connection_header, 4) == 4) {
          if(connection_header == ETCP_CONNECTION_HEADER_A) {
            header_ok = true;
            _ack_requested = false;
          } else if(connection_header == ETCP_CONNECTION_HEADER_A_ACK) {
            header_ok = true;
            _ack_requested = true;
          }
        }
        if(header_ok) did_connect = true;
        else {
          disconnect_in();
          connected = false;
          #ifdef ETCP_DEBUG_PRINT
            Serial.println(F("Disconn. no connhead"));
          #endif
        }
      }
    }

    // Accept reverse connection if relevant
    #ifdef ETCP_SINGLE_DIRECTION
    if(connected && reverse && !_client_out) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.println(F("Lst rev"));
      #endif
      // ACK active on both sockets or none in this mode
      _request_ack = _ack_requested;

      bool connected_reverse = false;
      uint32_t start = PJON_MILLIS();
      do {
        _client_out = _server->available();
      } while (!_client_out && (uint32_t)(PJON_MILLIS()-start) < 2000);

      if(_client_out) {
        #ifdef ETCP_DEBUG_PRINT
          Serial.println("Accept rev OK");
        #endif
        uint32_t connection_header = 0;
        if(
          read_bytes(_client_out, (uint8_t*) &connection_header, 4) == 4 &&
          connection_header == ETCP_CONNECTION_HEADER_B
        ) connected_reverse = true;
        else {
          #ifdef ETCP_DEBUG_PRINT
            Serial.println(F("Disconn. rev no connhead"));
          #endif
        }
      } else {
        #ifdef ETCP_DEBUG_PRINT
          Serial.println(F("Accept rev timed out"));
        #endif
      }

      if(connected_reverse) did_connect = true;
      else {
        #ifdef ETCP_DEBUG_PRINT
          Serial.print(F("Fail accept, closing, "));
          Serial.print(connected);
          Serial.println(connected_reverse);
        #endif
        connected = false;
        disconnect_in();
      }
    }
    #endif

    if(did_connect) {
      _connection_time = PJON_MILLIS();
      _connection_count++;
      // Count the connection as a receive action
      _last_receive_time = PJON_MILLIS();
    }

    return connected;
  };


  void disconnect_out_if_needed(int16_t result) {
    //bool connected = _client_out.connected();
    if(_client_out && (result != PJON_ACK || !_keep_connection)) {
      stop(_client_out);
      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Disconn outcl. OK=");
        Serial.println(result == PJON_ACK);
      #endif
    }
  };

  bool got_receive_timeout() {
    return (uint32_t)(PJON_MILLIS() - _last_receive_time) > ETCP_IDLE_TIMEOUT;
  };

  bool disconnect_in_if_needed() {
    if(_client_in && !_keep_connection) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.println("Disc. inclient.");
      #endif
      stop(_client_in);
    }
  };


  uint16_t send(
    EthernetClient &client,
    uint8_t id,
    const char *packet,
    uint16_t length
  ) {
    if(!_single_socket) while (receive() == PJON_ACK) ;

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

    /* If the other side is sending as well, we need to allow it to be read
       and acknowledged, otherwise we have a deadlock where both are waiting
       for ACK and will time out unsuccessfully */
    if(!_single_socket) while (receive() == PJON_ACK) ;

    int16_t result = PJON_FAIL;
    if(_request_ack) {
      // Read ACK
      if(ok) {
        uint16_t code = 0;
        ok = read_bytes(client, (byte*) &code, 2) == 2;
        if(ok && (code == PJON_ACK)) result = code;
      }

      #ifdef ETCP_DEBUG_PRINT
        Serial.print("PJON_ACK stat: ");
        Serial.print(result == PJON_ACK);
        Serial.println(ok ? " OK" : " FAIL");
      #endif
    } else result = ok ? PJON_ACK : PJON_FAIL;

    return result;  // PJON_FAIL, PJON_ACK
  };


  /* Do ACKed bidirectional transfer of packets over a single socket
     connection by using a master-slave mode where the master connects and
     delivers packets or a placeholder, then reads packets or placeholder
     back before closing the connection (unless letting it stay open). */

  uint16_t single_socket_transfer(
    EthernetClient &client,
    int16_t id,
    bool master,
    const char *contents,
    uint16_t length
  ) {
    #ifdef ETCP_SINGLE_SOCKET_WITH_ACK
    #ifdef ETCP_DEBUG_PRINT
      // Serial.print("Single-socket transfer, id=");
      // Serial.print(id);
      // Serial.print(", master=");
      // Serial.println(master);
    #endif
    if(master) { // Creating outgoing connections
      // Connect or check that we are already connected to the correct server
      bool connected = connect(id);
      #ifdef ETCP_DEBUG_PRINT
//        Serial.println(connected ? "Out conn" : "No out conn");
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
           Serial.print(F("++++Sent p, ok="));
           Serial.println(ok);
         #endif
      }

      // Read number of incoming messages
      uint8_t numpackets_in = 0;
      if(ok) ok = read_bytes(client, &numpackets_in, 1) == 1;
      #ifdef ETCP_DEBUG_PRINT
        if(!ok || numpackets_in > 0) {
          Serial.print("Read np_in: ");
          Serial.print(numpackets_in);
          Serial.println(ok ? " OK" : " FAIL");
      }
      #endif

      // Read incoming packages if any
      for(uint8_t i = 0; ok && i < numpackets_in; i++) {
        while(client.available() < 1 && client.connected()) ;
        ok = receive(client) == PJON_ACK;
        #ifdef ETCP_DEBUG_PRINT
          Serial.print(F("------->Read p ")); Serial.print(i);
          Serial.print(F(", ok=")); Serial.println(ok);
        #endif
      }

      // Write singlesocket footer for the whole thing
      uint32_t foot = ETCP_SINGLE_SOCKET_FOOTER;
      if(ok) ok = client.write((byte*) &foot, 4) == 4;
      if(ok) client.flush();
      #ifdef ETCP_DEBUG_PRINT
//        Serial.print("Sent ss foot, ok=");
//        Serial.println(ok);
      #endif

      // Disconnect
      int16_t result = ok ? PJON_ACK : PJON_FAIL;
      disconnect_out_if_needed(result);

      #ifdef ETCP_DEBUG_PRINT
      if(numpackets_in > 0 || numpackets_out > 0) {
        Serial.print("INOUTm ");
        Serial.print(numpackets_in);
        Serial.print(numpackets_out);
        Serial.println(ok ? " OK" : " FAIL");
      }
      #endif

      // Return PJON_ACK if successfully sent or received a packet
      return contents == NULL ?
        (numpackets_in > 0 ? result : PJON_FAIL) :
        result;

    } else { // Receiving incoming connections and packets and request

      // Wait for and accept connection
      bool connected = accept();
      #ifdef ETCP_DEBUG_PRINT
        //Serial.println(connected ? "In conn" : "No in conn");
      #endif
      if(!connected) return PJON_FAIL;

      // Read singlesocket header
      bool ok = read_until_header(client, ETCP_SINGLE_SOCKET_HEADER);
      #ifdef ETCP_DEBUG_PRINT
        //Serial.print("Read ss head, ok=");
        //Serial.println(ok);
      #endif

      // Read number of incoming packets
      uint8_t numpackets_in = 0;
      if(ok) ok = read_bytes(client, (byte*) &numpackets_in, 1) == 1;
      #ifdef ETCP_DEBUG_PRINT
        if(!ok || numpackets_in > 0) {
          Serial.print("Read np_in: ");
          Serial.print(numpackets_in);
          Serial.println(ok ? " OK" : " FAIL");
        }
      #endif

      // Read incoming packets if any, send ACK for each
      for(uint8_t i = 0; ok && i < numpackets_in; i++) {
        while(client.available() < 1 && client.connected()) ;
        ok = receive(client) == PJON_ACK;
        #ifdef ETCP_DEBUG_PRINT
          Serial.print(F("Read p "));
          Serial.print(i);
          Serial.print(F(", ok="));
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
           Serial.print(F("Sent p, ok="));
           Serial.println(ok);
        #endif
      }

      // Read singlesocket footer
      if(ok) {
        uint32_t foot = 0;
        ok = read_bytes(client, (byte*) &foot, 4) == 4;
        if(foot != ETCP_SINGLE_SOCKET_FOOTER) ok = 0;
        #ifdef ETCP_DEBUG_PRINT
          //Serial.print(F("Read ss foot, ok="));
          //Serial.println(ok);
        #endif
      }

      // Disconnect
      disconnect_in_if_needed();

      #ifdef ETCP_DEBUG_PRINT
      if(numpackets_in > 0 || numpackets_out > 0) {
        Serial.print("INOUT ");
        Serial.print(numpackets_in);
        Serial.print(numpackets_out);
        Serial.println(ok ? " OK" : " FAIL");
      }
      #endif

      // Return PJON_ACK if successfully sent or received a packet
      uint16_t result = ok ? PJON_ACK : PJON_FAIL;
      return contents == NULL ?
        (numpackets_in > 0 ? result : PJON_FAIL) :
        result;
    }
    #endif
    return PJON_FAIL;
  };


  /* Read until a specific 4 byte value is found.
     This will resync if stream position is lost. */

  bool read_until_header(EthernetClient &client, uint32_t header) {
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

  EthernetLink() {
    init();
  };


  EthernetLink(uint8_t id) {
    init();
    set_id(id);
  };

  #ifndef ARDUINO
  // Destructor not needed on Arduino, uses a lot of program space
  ~EthernetLink() {
    if(_server) delete _server;
  }
  #endif

  int16_t add_node(
    uint8_t remote_id,
    const uint8_t remote_ip[],
    uint16_t port_number = ETCP_DEFAULT_PORT
  ) {
    if(_remote_node_count == ETCP_MAX_REMOTE_NODES) return -1;
    _remote_id[_remote_node_count] = remote_id;
    memcpy(_remote_ip[_remote_node_count], remote_ip, 4);
    _remote_port[_remote_node_count] = port_number;
    _remote_node_count++;
    return _remote_node_count - 1;
  };


  /* This function must be called after configuration before running,
     for ALL configurations EXCEPT when initiator in single_socket
     and single_initiate_direction modes.
     In single_socket and single_initiate_direction modes, calling this
     function sets it as receiver, NOT initiator. */

  void start_listening(uint16_t port_number = ETCP_DEFAULT_PORT) {
    _local_port = port_number;
    _initiator = false; // If we are listening, we are not the initiator
    if(
      _server == NULL &&
      (!_initiate_both_sockets_in_same_direction || !_initiator)
    ) {
      #ifdef ETCP_DEBUG_PRINT
        Serial.print("Lst on port ");
        Serial.println(port_number);
      #endif
      _server = new EthernetServer(port_number);
      _server->begin();
    }
  };


  /* Whether to keep outgoing connection live until we need connect to
     another EthernetLink node */

  void keep_connection(bool keep) {
    _keep_connection = keep;
  };


  /* Whether to do bidirectional data transfer on a single socket or use one
     socket for each direction. Single socket transfer is slower but more
     firewall-friendly. Client connects to server, and packets are exchanged
     in both directions on the same socket. If using this, _only one_ of the
     sides should call start_listening, and that side will be receiver with
     the other initiator (establishing connections) */
  #ifdef ETCP_SINGLE_SOCKET_WITH_ACK
  void single_socket(bool single_socket) {
    _single_socket = single_socket;
  };
  #endif

  /* With single_socket = false, there is one socket for each packet
     direction. Normally the sockets are initiated from the side sending the
     packet. By setting initiate_both_sockets_in_same direction, both sockets
     can be initiated from one of the devices, to simplify firewall setup, or
     for letting only one of the devices have a static IP address.
     This should only be used with _keep_connection = true, and is meant for
     permanent one-to-one links. */
  #ifdef ETCP_SINGLE_DIRECTION
  void single_initiate_direction(bool single_initiate_direction) {
    _initiate_both_sockets_in_same_direction = single_initiate_direction;
  };
  #endif

  /* Request an explicit immediate ACK for each packet being sent.
     This make it possible with guearanteed delivery.
     Without ACK, a packet may be lost before a dead socket is discovered,
     but for some shemes this is acceptable. */

  void request_ack(bool request_ack) {
    _request_ack = request_ack;
  }

  /* Some connection statistics */
  uint32_t get_connection_time() const { return _connection_time; }
  uint32_t get_connection_count() const { return _connection_count; }


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


  // Overridden functions below ----------------------------------------------

  // Connect to a server if needed, then read incoming package and send ACK

  uint16_t receive() {
    if(_server == NULL) { // Not listening for incoming connections
      int16_t remote_id = _remote_node_count == 1 ? _remote_id[0] : -1;
      if(_single_socket) { // Single-socket mode.
        /* Only read from already established outgoing socket, or create
        connection if there is only one remote node configured (no doubt about
        which node to connect to). */
        return single_socket_transfer(_client_out, remote_id, true, NULL, 0);
      }
      #ifdef ETCP_SINGLE_DIRECTION
      else if(_initiate_both_sockets_in_same_direction && _initiator) {
        bool connected = connect(remote_id);
        uint16_t result = receive(_client_in);
        // Do not disconnect if nothing to read
        disconnect_out_if_needed(PJON_ACK);
        disconnect_in_if_needed();
        return result;
      }
      #endif
    } else {
      // Accept new incoming connection if connection has been lost
      if(_single_socket)
        return single_socket_transfer(_client_in, -1, false, NULL, 0);
      else {
        // Accept incoming connection(s)
        if(!accept()) return PJON_FAIL;

        uint16_t result = receive(_client_in);
        disconnect_in_if_needed();
        // Do not disconnect if nothing to read
        disconnect_out_if_needed(PJON_ACK);
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
    bool connected = false;
    #ifdef ETCP_SINGLE_DIRECTION
    if(_initiate_both_sockets_in_same_direction && !_initiator)
      connected = accept();
    else
    #endif
    connected = connect(id);

    // Send the packet and read PJON_ACK
    int16_t result = PJON_FAIL;
    if(connected) result = send(_client_out, id, packet, length);

    // Disconnect
    disconnect_out_if_needed(result);
    disconnect_in_if_needed();

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
