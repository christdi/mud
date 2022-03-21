#ifndef MUD_NETWORK_GMCP_H
#define MUD_NETWORK_GMCP_H

#include "mud/network/telnet.h"

#define TELOPT_GMCP 201

/**
 Structs
**/
typedef struct gmcp {
  telnet_option_t gmcp;
} gmcp_t;

/**
 * Function prototypes
**/
telnet_extension_t* network_new_gmcp_telnet_extension();
void network_send_gmcp_message(client_t* client, char* topic, size_t topic_len, char* msg, size_t msg_len);

#endif
