#include <arpa/telnet.h>
#include <assert.h>
#include <stdlib.h>

#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/network/gmcp.h"

static void deallocate_gmcp_t(void* value);
static void initialise_gmcp(void* extension, telnet_t* telnet, client_t* client);
static telnet_option_t* get_option(void* extension, int option);
static telnet_config_t* get_config(void* extension, int option);

static telnet_config_t gmcp_config = {TELOPT_GMCP, true, true};

/**
 * Creates a new telnet_extension_t for the GMCP extension.
 *
 * Returns the new telnet_extension_t instance.
**/
telnet_extension_t* network_new_gmcp_telnet_extension() {
  telnet_extension_t* extension = calloc(1, sizeof(telnet_extension_t));
  gmcp_t* gmcp = calloc(1, sizeof(gmcp_t));

  extension->extension = gmcp;
  extension->deallocate = deallocate_gmcp_t;
  extension->initialise = initialise_gmcp;
  extension->get_option = get_option;
  extension->get_config = get_config;
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

  LOG(INFO, "Sending GMCP WILL");
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
  if (option == TELOPT_GMCP) {
    return &gmcp_config;
  }  

  return NULL;
}

/**
 * Sends a GMCP message to a client.
**/
void network_send_gmcp_message(client_t* client, char* topic, size_t topic_len, char* msg, size_t msg_len) {
  char sb[] = { (char) IAC, (char) SB, (char) TELOPT_GMCP };
  char se[] = { (char) IAC, (char) SE };

  send_to_client(client, sb, 3);
  send_to_client(client, topic, topic_len);

  if (msg_len > 0) {
    send_to_client(client, " ", 1);
    send_to_client(client, msg, msg_len);
  }


  send_to_client(client, se, 2);

  // size_t output_len = topic_len + msg_len + 4;
  // char output[topic_len + msg_len + 4];

  // output[0] = (char) IAC;
  // output[1] = (char) SB;

  // size_t topic_index = 2;
  // size_t msg_index = topic_index + topic_len + 1;
  // size_t se_index = msg_index + msg_len + 1;

  // memcpy(output + topic_index, topic, topic_len);
  // memcpy(output + msg_index, msg, msg_len);

  // putput[se_index] = (char) IAC;
  // output[se_index + 1] = (char) SE;

}
