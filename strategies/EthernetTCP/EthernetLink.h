/* This EthernetLink class transfers packets of data over TCP/IP Ethernet connections.
   
   It may accept incoming connections and send ACK back for each incoming package,
   it may connect to another object in another device and deliver a packet and get an ACK back,
   or it may do both -- connect to another device for delivering packages and accept incoming connections
   and packages for bidirectional transfer.
   
   When establishing connections, the target must be registered with the add_node function for setting the IP and port
   along with a unique id that is used for referring to the target in all other functions.
   
   An EthernetLink may work in multiple ways when bidirectional:
   
   1. One-to-one connection with another EthernetLink. In this mode, using the keep_connection is recommended
      because it reduces network activity and speeds things up dramatically.
      In this mode, there are two options:
      
      A. When running on a firewall-free network one socket can be created in each direction, allowing for 
         bidirectional "full-duplex" (as far as that goes for single-threaded code), with data being buffered
         in each direction. It requires each side to know about each other's address, but delivers higher throughput
         than a single_socket approach.
         An EthernetLink object in this mode has one node added by add_node, and both sides calls start_listening
         to accept incoming connections. If there are multiple objects on one device, each has to listen to a unique 
         port number (specified in the start_listening function call).
         
      B. Set single_socket to mave a master-slave scenario where one side, the initiator, creates a socket connection
         and sends any packages it may have and then receives any package waiting to be sendt in the opposite direction.
         This requires the initiator to know where to connect, but not the other way around. This is suitable for
         connections through the Internet because a firewall opening only has to be opened in one place.
         An EthernetLink object in this mode has one node added by add_node (the initiator side), or calls the
         start_listening function to start listening (the receiver side).
         If there are multiple receiving objects on one device, each has to listen to a unique 
         port number (specified in the start_listening function call).
         
  2. If running a one-to-many or many-to-many scenario the keep_connection should be deactivated because the Ethernet
     library limites the number of incoming connections per EthernetLink object to one, so keeping the sockets
     permanently connected prohibits this. Not using keep_connection drops the transfer speed a lot, though.
     
     A. When running on a firewall-free network each packet is delivered through a dedicated connection created
        for delivering that package. After delivery and ACK the socket is closed. All devices can send to and
        receive from each other.
        
     B. Using the single_socket approach will use a master-slave like approach where only the initiators need 
        to know the address of the receiver(s), for easier firewall traversal. After the two-way transfer of 
        packets on the same connection, the connection will be closed so that the receiver is ready for another
        connection. This requires only _one firewall opening_ for bidirectional transfer with 255 others.
     
  Limitations for the different modes when using the W5100 based Ethernet shield with a 4 socket limit:
  
  1A. Two EthernetLink objects can be used for bidirectional communication with two sites.
  1B. Four EthernetLink objects can be used for bidirectional communication with four sites.
  2A. As the sockets are only used temporarily each object can accept connections from 255 others, and 
      deliver packets to 255 others. Up to two objects can be used in one device, if multiple listening ports are needed.
      Each object can have 1 to 255 nodes added by add_node, and can deliver to all.
      There can be up to three objects in each device if needed. One socket must remain free for outgoing connections.
  2B. As 2A, each receiver can receive packets from and send packets to 255 initiators, using a _single listening port_.
      There can be four receiver objects in on device, or three receiver objects and an unlimited number of initiator
      objects that will be able to send to and receive from to up to 255 sites each.  

  Limitations for the different modes when using the ENC28J60 based Ethernet shield with a 1 socket limit:
  
  1A. Not available.
  1B. One EthernetLink object can be used for fast bidirectional communication with one site.
  2A. Not available.
  2B. One receiver can receive from and deliver packets to unlimited initiators, _or_ unlimited initator objects
      can send to and receive from to up to 255 sites each
          
  Note: To use the ENC28J60 shield instead of the WIZ5100 shield, include <UIPEthernet.h> before EthernetLink.h.        
          
  OSPREY, PJON and EthernetLink
  The EthernetLink can be used standalone for simple delivery between two devices. But when using EthernetLink objects
  with OSPREY routing logic combined with PJON communication between devices, the true potential can be reached.
  A site consisting of one or more buses of devices communicating through PJON, wired and/or wirelessly, can be 
  connected seamlessly with multiple other similar sites in other places of the world, communicating through Internet
  with minimal firewall configuration.
  
  Scenarios:
  
  - A site with multiple devices that want to communicate directly through the LAN: 
    Use EthernetLink directly, with approach 1A for best performance or 1B for less configuration (only the 
    initiators will need to know the address of the receiver(s)).
    
  - Multiple sites connected through a LAN, with each site having one or more PJON buses with mutiple devices
    communicating wired or wirelessly. Each site has at least one device running OSPREY+EthernetLink plus an
    Ethernet shield, and these OSPREY routers will make sure that every device can deliver packets to any other
    device on any site as long as the device and bus numbers are known.
    EthernetLink object with configuration 1A should be used for best performance, or 1B for less configuration,
    or if the number of sites exceed the limits, configuration 2A or 2B can be used.
    
  - A main site with a receiver, and multiple remote sites that connect through the Internet. Configuration 2B
    requires one firewall opening to be present on the main site, and the initators are only establishing outgoing 
    connections not requiring firewall openings. Each of the EthernetLink objects are used in an OSPREY device.
    Each Internet-connected site including the main site can consist of one or more PJON buses connected
    with OSPREY PJONLink or through LAN based OSPREY EthernetLink routing.
    Each device on any bus can deliver packets to any device on any other bus. Devices on non-main sites can
    either open firewall openings for direct communication, or deliver packets to each other through the 
    Internet-attached OSPREY router on the main site.  
    
  NOTE: The W5100 full-size Ethernet shields come in multiple variants. If you get a problem where the card is not
   starting after plugging in the power, check if there is small resistor network behind the Ethernet outlet, with
   one resistor having printed "511" on it. If so, try another brand. Even startup delays do not fix this, but it 
   can be solved with a capacitor+resistor, search for it.

  NOTE: If not needing SINGLE_SOCKET functionality, the program size can be reduced by defining NO_SINGLE_SOCKET:
    #define NO_SINGLE_SOCKET
*/


//#pragma GCC optimize ("-O2")

#pragma once

//#include <Link.h>
#ifndef UIPETHERNET_H
  #include <Ethernet.h>
#endif  

// Constants
#define ACK           6
#define NAK           21

// Internal constants
#define FAIL          0x100

#define MAX_REMOTE_NODES 10
#define DEFAULT_PORT     7000

typedef void (*link_receiver)(uint8_t id, const uint8_t *payload, uint16_t length, void *callback_object);
typedef void (*link_error)(uint8_t code, uint8_t data);

class EthernetLink {
private:
  link_receiver _receiver = NULL;
  link_error    _error = NULL;
  void *_callback_object = NULL;

  // Local node
  uint8_t  _local_id = 0;
  uint8_t  _local_ip[4];
  uint16_t _local_port = DEFAULT_PORT;

  // Remote nodes
  uint8_t  _remote_node_count = 0;
  uint8_t  _remote_id[MAX_REMOTE_NODES];
  uint8_t  _remote_ip[MAX_REMOTE_NODES][4];
  uint16_t _remote_port[MAX_REMOTE_NODES];

  EthernetServer *_server = NULL;
  EthernetClient _client_out;    // Created as an outgoing connection
  EthernetClient _client_in;     // Accepted incoming connection
  int16_t _current_device = -1;  // The id of the remove device/node that we have connected to
  bool _keep_connection = false, // Keep sockets permanently open instead of reconnecting for each transfer
       _single_socket = false;   // Do bidirectional transfer on a single socket

  void init();
  int16_t find_remote_node(uint8_t id);
  int16_t read_bytes(EthernetClient &client, uint8_t *contents, uint16_t length);
  uint16_t receive(EthernetClient &client);
  bool connect(uint8_t id);
  void stop(EthernetClient &client) { client.stop(); }
  bool accept();
  void disconnect_out_if_needed(int16_t result);
  bool disconnect_in_if_needed();
  uint16_t send(EthernetClient &client, uint8_t id, const char *packet, uint16_t length);
  uint16_t single_socket_transfer(EthernetClient &client, int16_t id, bool master, const char *contents, uint16_t length);
  bool read_until_header(EthernetClient &client, uint32_t header);
public:
  EthernetLink() { init(); };
  EthernetLink(uint8_t id) { init(); set_id(id); };

  int16_t add_node(uint8_t remote_id, const uint8_t remote_ip[], uint16_t port_number = DEFAULT_PORT);
  void start_listening(uint16_t port_number = DEFAULT_PORT); // Do not call for single_socket initiator

  // Whether to keep outgoing connection live until we need connect to another EthernetLink node
  void keep_connection(bool keep) { _keep_connection = keep; };

  // Whether to do bidirectional data transfer on a single socket or use one socket for each direction.
  // Single socket transfer is slower but more firewall-friendly. Client connects to server, and packets are 
  // exchanged in both directions on the same socket. If using this, _only one_ of the sides should call
  // start_listening, and that side will be receiver with the other initiator (establishing connections)
  void single_socket(bool single_socket) { _single_socket = single_socket; }
  
  // Keep trying to send for a maximum duration
  int16_t send_with_duration(uint8_t id, const char *packet, uint16_t length, uint32_t duration_us);

  // In single-socket mode and acting as initiator, connect and check for incoming packets from a specific device
  uint16_t poll_receive(uint8_t remote_id);
  
  uint8_t get_id() const { return _local_id; }
  
  
  //**************** Overridden functions below *******************

  uint16_t receive();
  uint16_t receive(uint32_t duration_us);

  uint16_t send(uint8_t id, const char *packet, uint16_t length, uint32_t timing_us = 0);

  void set_id(uint8_t id) { _local_id = id; };
  void set_error(link_error e) { _error = e; };
  void set_receiver(link_receiver r, void *callback_object) { _receiver = r; _callback_object = callback_object; };

  uint8_t device_id() { return _local_id; };
  uint8_t acquire_id() { return 0; }; // Not supported yet

  void update() { };
};

#include "EthernetLink.impl"
