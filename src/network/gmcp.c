#include <arpa/telnet.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "bsd/string.h"

#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/network/gmcp.h"

static void deallocate_gmcp_t(void* value);
static void initialise_gmcp(void* extension, telnet_t* telnet, client_t* client);
static telnet_option_t* get_option(void* extension, int option);
static telnet_config_t* get_config(void* extension, int option);
static void process_se(void* extension, telnet_t* telnet, client_t* client, int option, const char* data, size_t len);

static telnet_config_t gmcp_config = {TELOPT_GMCP, true, true};

/**
 * Creates a new telnet_extension_t for the GMCP extension.
 *
 * Returns the new telnet_extension_t instance.
**/
telnet_extension_t* network_new_gmcp_telnet_extension(void* context, on_gmcp_func_t on_gmcp) {
  assert(context);
  assert(on_gmcp);

  telnet_extension_t* extension = calloc(1, sizeof(telnet_extension_t));
  gmcp_t* gmcp = calloc(1, sizeof(gmcp_t));
  gmcp->context = context;
  gmcp->on_gmcp = on_gmcp;

  extension->extension = gmcp;
  extension->deallocate = deallocate_gmcp_t;
  extension->initialise = initialise_gmcp;
  extension->get_option = get_option;
  extension->get_config = get_config;
  extension->subnegotiation = process_se;
  extension->next = NULL;

  return extension;
}

/**
 * Deallocates a void pointer to gmcp_t
 *
 * value - void pointer to gmcp_t
**/
void deallocate_gmcp_t(void* value) {
  assert(value);

  gmcp_t* gmcp = value;
  
  free(gmcp);
}

/**
 * Initialises GMCP by sending a WILL to the client
 *
 * extension - void pointer to the extension (should be gmcp_t)
 * telnet - the telnet_t instance for the client
 * client - the client_t instance of the client
**/
void initialise_gmcp(void* extension, telnet_t* telnet, client_t* client) {
  assert(extension);
  assert(telnet);
  assert(client);

  network_telnet_send_will(telnet, client, TELOPT_GMCP);
}

/**
 * Retrieves telnet options relevant to the GMCP extension
 * 
 * extension - void pointer to the gmcp_t instance
 * option - the option number of the option we're looking for
 *
 * Returns the option if GMCP or NULL.
**/
telnet_option_t* get_option(void* extension, int option) {
  assert(extension);
  
  gmcp_t* gmcp = extension;

  if (option == TELOPT_GMCP) {
    return &gmcp->gmcp;
  }

  return NULL;
}

/**
 * Retrieves config options relevant to the GMCP extension
 *
 * extension - void pointer to the gmcp_t instance
 * option - the option number of the option we're looking for
 *
 * Returns the option config or NULL.
**/
telnet_config_t* get_config(void* extension, int option) {
  assert(extension);

  if (option == TELOPT_GMCP) {
    return &gmcp_config;
  }  

  return NULL;
}

/**
 * Sends a GMCP message to a client.
 *
 * client - client_t instance of the client to send the GMCP message to
 * topic - the topic of the GMCP message
 * topic_len - the length of the topic message
 * msg - the message to be sent, may be null
 * msg_len - the length of the message to be sent
**/
void network_send_gmcp_message(client_t* client, char* topic, size_t topic_len, char* msg, size_t msg_len) {
  assert(client);
  assert(topic);
  
  char sb[] = { (char) IAC, (char) SB, (char) TELOPT_GMCP };
  char se[] = { (char) IAC, (char) SE };

  send_to_client(client, sb, 3);
  send_to_client(client, topic, topic_len);

  if (msg_len > 0) {
    send_to_client(client, " ", 1);
    send_to_client(client, msg, msg_len);
  }

  send_to_client(client, se, 2);
}

/**
 * Process a Telnet subnegotiation.
 *
 * extension - void pointer to gmcp_t with handlers for GMCP messages
 * telnet - telnet_t instance
 * client - client_t instance representing the remote client
 * option - the option negotiated for
 * data - the data for the subnegotiation
 * len - the length of the data
**/
void process_se(void* extension, telnet_t* telnet, client_t* client, int option, const char* data, size_t len) {
  assert(extension);
  assert(telnet);
  assert(client);
  assert(data);

  gmcp_t* gmcp = extension;  

  if (option == TELOPT_GMCP) {
    if (gmcp && gmcp->on_gmcp) {
      len = len + 1; // null terminator

      char copy[len];
      strlcpy(copy, data, len);

      char* msg = strchr(copy, ' ');
      char* topic = copy;

      if (msg != NULL) {
        *msg = '\0';
        msg = msg + 1;
      }
  
      gmcp->on_gmcp(client, gmcp->context, topic, msg);
    }
  }
}
