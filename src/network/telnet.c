#include <stdlib.h>
#include <assert.h>
#include <arpa/telnet.h>
#include <string.h>

#include "mud/log.h"
#include "mud/network/client.h"
#include "mud/network/protocol.h"
#include "mud/network/telnet.h"
#include "mud/util/mudstring.h"

static void update_parse_state(telnet_parse_t* ps, char c, int index, int* iac_index);

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

  telnet_t* telnet = protocol;

  network_telnet_send_do(telnet, client, TELOPT_SGA);
}

/**
 * Callback method called when the client has received new input.
 *
 * client - the client who has received input
 * protocol - a void pointer to a telnet_t instance
 * data - the data that has been received
 * len - the length of the input received
 *
 * Returns the length of the input as we may have altered it
**/
int network_telnet_on_input(client_t* client, void* protocol, char* input, size_t len) {
  assert(client);
  assert(protocol);
  assert(input);

  telnet_t* telnet = protocol;
  telnet_parse_t* ps = &telnet->incoming;

  int move_index = 0;
  int cmd_len = 0;

  for (int i = 0; i < len; i++) {
    update_parse_state(ps, input[i], i, &move_index);

    cmd_len = ps->state != READ_IAC ? cmd_len + 1 : cmd_len;

    if (ps->state == DONE) {
      LOG(INFO, "Complete telnet command in: [%d] [%d] [%d]", IAC, ps->op, ps->option);

      switch(ps->op) {
        case DO:
          process_do_incoming(telnet, client, ps->option);
          break;
        
        case DONT:
          process_dont_incoming(telnet, client, ps->option);
          break;

        case WILL:
          process_will_incoming(telnet, client, ps->option);
          break;

        case WONT:
          process_wont_incoming(telnet, client, ps->option);
          break;
      }

      ps->op = 0;
      ps->option = 0;

      int j = move_index + cmd_len + 1;

      if (j > len) {
        memset(input + move_index, 0, cmd_len);
      } else {
        memmove(input + move_index, input + j, len - i);

        i = i - cmd_len;
      }

      len = len - cmd_len;
      cmd_len = 0;
      move_index = 0;

      ps->state = READ_IAC;
    }
  }

  return len;
}

/**
 * Callback method called when the client is about to send output
 *
 * client - the client who is about to send output
 * protocol - a void pointer to a telnet_t instance
 * output - the data that has been buffered for output
 * len - the length of the data buffered
 *
 * Returns the len of the output string as we may have altered it
**/
int network_telnet_on_output(client_t* client, void* protocol, char* output, size_t len) {
  assert(client);
  assert(protocol);
  assert(output);

  telnet_t* telnet = protocol;

  if (telnet->suppress_go_ahead.us != YES) {
    network_telnet_send_ga(telnet, client);
  }

  return len;
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
  telnet_parse_t* ps = &telnet->outgoing;

  int iac_index = 0;

  for (int i = 0; i < len; i++) {
    update_parse_state(ps, output[i], i, &iac_index);

    if (ps->state == DONE) {
      LOG(INFO, "Complete telnet command out: [%d] [%d] [%d]", IAC, ps->op, ps->option);
      switch(ps->op) {
        case DO:
          process_do_outgoing(telnet, ps->option);
          break;

        case DONT:
          process_dont_outgoing(telnet, ps->option);
          break;

        case WILL:
          process_will_outgoing(telnet, ps->option);
          break;
          
        case WONT:
          process_wont_outgoing(telnet, ps->option);
          break;
      }

      ps->op = 0;
      ps->option = 0;
      ps->state = READ_IAC;
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
 * Updates the state of a parse_state_t based on a character.
 *
 * ps - the parse state to updat
 * c - the character to use to update the state
 * index - the position of the character c in the string that it was extracted
 * iac_index - a pointer to an int representing the position of the IAC byte that we update for the caller
 * cmd_len a pointer to an int representing the current length of the telnet command that we update for the
**/
void update_parse_state(telnet_parse_t* ps, char c, int index, int* iac_index) {
  switch(ps->state) {
    case READ_IAC:
      if (c == (char)IAC) {
        *iac_index = index;

        ps->state = READ_OP;
      }

      break;

    case READ_OP:
      ps->op = (unsigned int)(unsigned char)c;

      switch(ps->op) {
        case WILL:
        case WONT:
        case DO:
        case DONT:
          ps->state = READ_OP_VALUE;
          break;
        case SB:
          ps->state = READ_SE;
          break;
        case SE:
          ps->state = DONE;
          break;
        case GA:
        case EL:
        case EC:
        case AYT:
        case AO:
        case IP:
        case BREAK:
        case DM:
        case NOP:
        case EOR:
        case ABORT:
        case SUSP:
        case xEOF:
          ps->state = DONE;
          break;
        default:
          ps->state = READ_IAC;
          break;
      }

    case READ_SE:
      break;

    case READ_OP_VALUE:
      ps->option = (unsigned int)(unsigned char) c;
      ps->state = DONE;

      break;

    case DONE:
      break;
  }
}

/**
 * Processes a DO request from the remote host.
 *
 * telnet - telnet_t object we use to assess and update options
 * client - client we may need to respond to
 * option - the option that has been asked to DO
**/
void process_do_incoming(telnet_t* telnet, client_t* client, int option) {
  if (option == TELOPT_SGA) {
    telnet_option_t* sga = &telnet->suppress_go_ahead;

    if (sga->us == NO) {
      sga->us = YES;

      network_telnet_send_will(telnet, client, option);
    }

    if (sga->us == WANT_NO) {
      if (sga->us_side == EMPTY) {
        sga->us = NO;
      }

      if (sga->us_side == OPPOSITE) {
        sga->us = YES;
        sga->us_side = EMPTY;
      }
    }

    if (sga->us == WANT_YES) {
      if (sga->us_side == EMPTY) {
        sga->us = YES;
      }

      if (sga->us_side == OPPOSITE) {
        sga->us = WANT_NO;
        sga->us_side = EMPTY;

        network_telnet_send_wont(telnet, client, option);
      }
    }

    return;
  }

  network_telnet_send_wont(telnet, client, option);
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
