#include <stdlib.h>
#include <assert.h>
#include <arpa/telnet.h>
#include <string.h>

#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/network/protocol.h"
#include "mud/network/telnet.h"

static void process_do_incoming(telnet_t* telnet, client_t* client, int option);
static void process_dont_incoming(telnet_t* telnet, client_t* client, int option);
static void process_will_incoming(telnet_t* telnet, client_t* client, int option);
static void process_wont_incoming(telnet_t* telnet, client_t* client, int option);

static void process_do_outgoing(telnet_t* telnet, int option);
static void process_dont_outgoing(telnet_t* telnet, int option);
static void process_will_outgoing(telnet_t* telnet, int option);
static void process_wont_outgoing(telnet_t* telnet, int option);

/**
 * Allocates a new instance of telnet_t
 *
 * Returns the allocated instance
**/
telnet_t* network_new_telnet_t() {
  telnet_t* telnet = calloc(1, sizeof(telnet_t));

  telnet->incoming.state = READ_IAC;
  telnet->incoming.op = 0;
  telnet->incoming.option = 0;

  telnet->outgoing.state = READ_IAC;
  telnet->outgoing.op = 0;
  telnet->outgoing.option = 0;

  telnet->echo.us = NO;
  telnet->echo.them = NO;
  telnet->echo.us_side = EMPTY;
  telnet->echo.them_side = EMPTY;

  telnet->suppress_go_ahead.us = NO;
  telnet->suppress_go_ahead.them = NO;
  telnet->suppress_go_ahead.us_side = EMPTY;
  telnet->suppress_go_ahead.them_side = EMPTY;

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

  protocol->type = TELNET;
  protocol->data = network_new_telnet_t();
  protocol->deallocator = network_deallocate_telnet_t;
  protocol->initialiser = network_telnet_initialised;
  protocol->on_input = network_telnet_on_input;
  protocol->on_output = network_telnet_on_output;
  protocol->on_flush = network_telnet_on_flush;

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

  network_telnet_send_will(protocol, client, (char) TELOPT_SGA);
  network_telnet_send_do(protocol, client, (char) TELOPT_SGA);
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
    switch(telnet->incoming.state) {
      case READ_IAC:
        if (input[i] == (char)IAC) {
          move_index = i;
          telnet->incoming.state = READ_OP;
        }

        break;

      case READ_OP:
        telnet->incoming.op = (unsigned int)(unsigned char)input[i];

        switch(telnet->incoming.op) {
          case WILL:
          case WONT:
          case DO:
          case DONT:
            telnet->incoming.state = READ_OP_VALUE;
            break;
          default:
            LOG(ERROR, "Unsupported telnet op [%u]", (unsigned int)(unsigned char) input[i]);

            telnet->incoming.state = IAC;
            break;
        }

      case READ_OP_VALUE:
        telnet->incoming.option = (unsigned int)(unsigned char)input[i];
        telnet->incoming.state = DONE;

        break;

      case DONE:
        break;
    }

    cmd_len = telnet->incoming.state != READ_IAC ? cmd_len + 1 : cmd_len;

    if (telnet->incoming.state == DONE) {
      switch(telnet->incoming.op) {
        case DO:
          process_do_incoming(telnet, client, telnet->incoming.option);
          break;
        
        case DONT:
          process_dont_incoming(telnet, client, telnet->incoming.option);
          break;

        case WILL:
          process_will_incoming(telnet, client, telnet->incoming.option);
          break;

        case WONT:
          process_wont_incoming(telnet, client, telnet->incoming.option);
          break;
      }

      telnet->incoming.op = 0;
      telnet->incoming.option = 0;

      int j = i + 1;

      if (j > len) {
        telnet->incoming.state = READ_IAC;

        continue;
      }

      memmove(input + move_index, input + j, len - i);

      len = len - cmd_len;
      i = i - cmd_len;

      cmd_len = 0;
      move_index = 0;

      telnet->incoming.state = READ_IAC;
    }
  }
}

/**
 * Callback method called when the client is about to send output
 *
 * client - the client who is about to send output
 * protocol - a void pointer to a telnet_t instance
 * output - the data that has been buffered for output
 * len - the length of the data buffered
**/
void network_telnet_on_output(client_t* client, void* protocol, char* output, size_t len) {
  assert(client);
  assert(protocol);
  assert(output);

  telnet_t* telnet = protocol;

  if (telnet->suppress_go_ahead.us != YES) {
    network_telnet_send_ga(telnet, client);
  }
}

/**
 * Callback method called when the client has flushed output.
 *
 * client - the client who has flushed output
 * protocol - a void pointer to a telnet_t instance
 * output - the output that has been flushed
 * len - the length of the data that was flushed
**/
void network_telnet_on_flush(client_t* client, void* protocol, char* output, size_t len) {
  telnet_t* telnet = protocol;

  for (int i = 0; i < len; i++) {
    switch(telnet->outgoing.state) {
      case READ_IAC:
        if (output[i] == (char)IAC) {
          telnet->outgoing.state = READ_OP;
        }

        break;

      case READ_OP:
        telnet->outgoing.op = (unsigned int) (unsigned char)output[i];

        switch(telnet->outgoing.op) {
          case WILL:
          case WONT:
          case DO:
          case DONT:
            telnet->outgoing.state = READ_OP_VALUE;
            break;
          default:
            LOG(ERROR, "Unsupported telnet op [%u]", (unsigned int)(unsigned char) output[i]);
            telnet->outgoing.state = IAC;

            break;
        }

      case READ_OP_VALUE:
        telnet->outgoing.option = (unsigned int)(unsigned char)output[i];
        telnet->outgoing.state = DONE;

        break;

      case DONE:
        break;
    }

    if (telnet->outgoing.state == DONE) {
      switch(telnet->outgoing.op) {
        case DO:
          process_do_outgoing(telnet, telnet->outgoing.option);
          break;

        case DONT:
          process_dont_outgoing(telnet, telnet->outgoing.option);
          break;

        case WILL:
          process_will_outgoing(telnet, telnet->outgoing.option);
          break;
          
        case WONT:
          process_wont_outgoing(telnet, telnet->outgoing.option);
          break;
      }

      telnet->outgoing.op = 0;
      telnet->outgoing.option = 0;
    }
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

/**
 * Processes a DO request from the remote host.
 *
 * telnet - telnet_t object we use to assess and update options
 * client - client we may need to respond to
 * option - the option that has been asked to DO
**/
void process_do_incoming(telnet_t* telnet, client_t* client, int option) {
}

/**
 * Processes a DONT request from the remote host.
 *
 * telnet - telnet_t object we use to assess and update options
 * client - client we may need to respond to
 * option - the option that has been asked to DO
**/
void process_dont_incoming(telnet_t* telnet, client_t* client, int option) {
}

/**
 * Processes a WILL request from the remote host.
 *
 * telnet - telnet_t object we use to assess and update options
 * client - client we may need to respond to
 * option - the option that has been asked to DO
**/
void process_will_incoming(telnet_t* telnet, client_t* client, int option) {
  if (option == TELOPT_SGA) {
    telnet_option_t* sga = &telnet->suppress_go_ahead;

    if (sga->them == NO) {
      sga->them = YES;
      network_telnet_send_do(telnet, client, option);
    }

    if (sga->them == WANT_NO) {
      if (sga->them_side == EMPTY) {
        
      }

      if (sga->them_side == OPPOSITE) {
        sga->them = YES;
        sga->them_side = EMPTY;
      }
    }

    if (sga->them == WANT_YES) {
      if (sga->them_side == EMPTY) {
        sga->them = YES;        
      }

      if (sga->them_side == OPPOSITE) {
        sga->them = WANT_NO;
        sga->them_side = EMPTY;
        
        network_telnet_send_dont(telnet, client, option);
      }
    }

    return; 
  }

  network_telnet_send_dont(telnet, client, option);
}

/**
 * Processes a WONT request from the remote host.
 *
 * telnet - telnet_t object we use to assess and update options
 * client - client we may need to respond to
 * option - the option that has been asked to DO
**/
void process_wont_incoming(telnet_t* telnet, client_t* client, int option) {
}

/**
 * Processes a DO request we are sending to the remote host.
 *
 * telnet - telnet_t object we need to update
 * option - the option that we're asking to DO
**/
void process_do_outgoing(telnet_t* telnet, int option) {
}

/**
 * Processes a DONT request we are sending to the remote host.
 *
 * telnet - telnet_t object we need to update
 * option - the option that we're asking to DONT
**/
void process_dont_outgoing(telnet_t* telnet, int option) {
}

/**
 * Processes a WILL request we are sending to the remote host.
 *
 * telnet - telnet_t object we need to update
 * option - the option that we're asking to WILL
**/
void process_will_outgoing(telnet_t* telnet, int option) {
}

/**
 * Processes a WONT request we are sending to the remote host.
 *
 * telnet - telnet_t object we need to update
 * option - the option that we're asking to WONT
**/
void process_wont_outgoing(telnet_t* telnet, int option) {
}
