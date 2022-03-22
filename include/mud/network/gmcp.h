#ifndef MUD_NETWORK_GMCP_H
#define MUD_NETWORK_GMCP_H

#include "mud/network/telnet.h"

#define TELOPT_GMCP 201

/**
 * Typededs
**/
typedef void (*on_gmcp_func_t)(client_t*, void*, const char*, const char*);


/**
 Structs
**/
typedef struct gmcp {
  void* context;
  on_gmcp_func_t on_gmcp;
  telnet_option_t gmcp;
} gmcp_t;

/**
 * Function prototypes
**/
telnet_extension_t* network_new_gmcp_telnet_extension(void* context, on_gmcp_func_t on_gmcp);
void network_send_gmcp_message(client_t* client, char* topic, size_t topic_len, char* msg, size_t msg_len);

#endif
