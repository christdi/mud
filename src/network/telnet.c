#include <stdlib.h>
#include <assert.h>
#include <arpa/telnet.h>
#include <string.h>

#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/network/protocol.h"
#include "mud/network/telnet.h"

/**
 * Allocates a new instance of telnet_t
 *
 * Returns the allocated instance
**/
telnet_t* network_new_telnet_t() {
  telnet_t* telnet = calloc(1, sizeof(telnet_t));

  telnet->state = READ_IAC;

  return telnet;
}

/**
 * Frees an allocated instance of telnet_t
 *
 * telnet - the telnet_t instance to be freed
**/
void network_free_telnet_t(telnet_t* telnet) {
  assert(telnet);

  free(telnet);
}

/**
 * Deallocates a void pointer to telnet_t
 *
 * value - void pointer to an instance of telnet_t
**/
void network_deallocate_telnet_t(void* value) {
  assert(value);
  
  network_free_telnet_t(value);
}

/**
 * Creates and configures a new instance of protocol_t for use with the telnet protocol.
 *
 * Returns the configured instance of protocol_t
**/
protocol_t* network_new_telnet_protocol_t() {
  protocol_t* protocol = network_new_protocol_t();

  protocol->data = network_new_telnet_t();
  protocol->deallocator = network_deallocate_telnet_t;
  protocol->initialiser = network_telnet_initialised;
  protocol->on_input = network_telnet_on_input;
  protocol->on_output = network_telnet_on_output;

  return protocol;
}

/**
 * Callback method called when the protocol is initialised
 *
 * client - the client who has established a connection
 * protocol - a void pointer to a telnet_t instance
**/
void network_telnet_initialised(client_t* client, void* protocol) {
  assert(client);
  assert(protocol);

  network_telnet_send_will(protocol, client, (char) TELOPT_ECHO);
  network_telnet_send_will(protocol, client, (char) TELOPT_SGA);
}

/**
 * Callback method called when the client has received new input.
 *
 * client - the client who has received input
 * protocol - a void pointer to a telnet_t instance
 * data - the data that has been received
**/
void network_telnet_on_input(client_t* client, void* protocol, char* input, size_t len) {
  assert(client);
  assert(protocol);
  assert(input);

  telnet_t* telnet = protocol;
  int move_index = 0;
  int cmd_len = 0;

  for (int i = 0; i < len; i++) {
    switch(telnet->state) {
      case READ_IAC:
        if (input[i] == (char)IAC) {
          LOG(INFO, "Received IAC byte");

          move_index = i;
          telnet->state = READ_OP;
        }

        break;

      case READ_OP:
        if (input[i] == (char) WILL || input[i] == (char) WONT || input[i] == (char) DO || input[i] == (char) DONT) {
          telnet->buffer[0] = input[i];

          telnet->state = READ_OP_VALUE;
        } else {
          LOG(ERROR, "Unsupported telnet op [%u]", (unsigned int)(unsigned char) input[i]);

          telnet->state = IAC;
        }

        break;

      case READ_OP_VALUE:
        telnet->buffer[1] = input[i];
        telnet->state = DONE;

        break;

      case DONE:
        break;
    }

    cmd_len = telnet->state != READ_IAC ? cmd_len + 1 : cmd_len;

    if (telnet->state == DONE) {
      LOG(INFO, "Full telnet request: [%u] [%u] [%u]", IAC, (unsigned int)(unsigned char)telnet->buffer[0], (unsigned int)(unsigned char)telnet->buffer[1]);

      int j = i + 1;

      if (j > len) {
        telnet->state = READ_IAC;

        continue;
      }

      memset(telnet->buffer, 0, TELNET_BUFFER_SIZE);
      memmove(input + move_index, input + j, len - i);

      len = len - cmd_len;
      i = i - cmd_len;

      cmd_len = 0;
      move_index = 0;

      telnet->state = READ_IAC;
    }
  }
}

/**
 * Callback method called when the client has received new output.
 *
 * client - the client who has received output
 * protocol - a void pointer to a telnet_t instance
 * data - the data that has been buffered for output
**/
void network_telnet_on_output(client_t* client, void* protocol, char* output, size_t len) {
  assert(client);
  assert(protocol);
  assert(output);

  telnet_t* telnet = protocol;

  if (telnet->suppress_go_ahead != YES) {
    network_telnet_send_ga(telnet, client);
  }
}

/**
 * Sends an IAC GA to the client.
 *
 * telnet - the telnet instance option request will be recorded against
 * client - the client we'll send the request to
**/
int network_telnet_send_ga(telnet_t* telnet, client_t* client) {
  assert(telnet);
  assert(client);

  char msg[] = { (char) IAC, (char) GA };
  send_to_client(client, msg, 2);

  return 0;
}

/**
 * Sends an IAC WILL OPTION to the client.
 *
 * telnet - the telnet instance option request will be recorded against
 * client - the client to sent the WILL to
 * option - the option to send WILL for
 *
 * Returns 0 on success or -1 on failure
**/
int network_telnet_send_will(telnet_t* telnet, client_t* client, int option) {
  assert(client);
  assert(option);

  char msg[] = { (char) IAC, (char) WILL, (char) option };
  send_to_client(client, msg, 3);

  return 0;
}

/**
 * Sends an IAC WONT OPTION to the client.
 *
 * telnet - the telnet instance option request will be recorded against
 * client - the client to sent the WONT to
 * option - the option to send WONT for
 *
 * Returns 0 on success or -1 on failure
**/
int network_telnet_send_wont(telnet_t* telnet, client_t* client, int option) {
  assert(client);
  assert(option);

  char msg[] = { (char) IAC, (char) WONT, (char) option };
  send_to_client(client, msg, 3);

  return 0;
}

/**
 * Sends an IAC DO OPTION to the client.
 *
 * telnet - the telnet instance option request will be recorded against
 * client - the client to sent the DO to
 * option - the option to send DO for
 *
 * Returns 0 on success or -1 on failure
**/
int network_telnet_send_do(telnet_t* telnet, client_t* client, int option) {
  assert(client);
  assert(option);

  char msg[] = { (char) IAC, (char) DO, (char) option };
  send_to_client(client, msg, 3);

  return 0;
}

/**
 * Sends an IAC DONT OPTION to the client.
 *
 * telnet - the telnet instance option request will be recorded against
 * client - the client to sent the DONT to
 * option - the option to send DONT for
 *
 * Returns 0 on success or -1 on failure
**/
int network_telnet_send_dont(telnet_t* telnet, client_t* client, int option) {
  assert(client);
  assert(option);

  char msg[] = { (char) IAC, (char) DONT, (char) option };
  send_to_client(client, msg, 3);

  return 0;
}
