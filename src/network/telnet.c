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

static int send_raw_do(client_t* client, int option);
static int send_raw_dont(client_t* client, int option);
static int send_raw_will(client_t* client, int option);
static int send_raw_wont(client_t* client, int option);

static telnet_option_t* get_option(telnet_t* telnet, int option);
static option_state_t* get_local_option(telnet_t* telnet, int option);
static option_state_t* get_client_option(telnet_t* telnet, int option);
static telnet_config_t* get_option_config(telnet_t* telnet, int option);

static void log_telnet_parse(telnet_parse_t* ps, size_t len, bool in);
static char* get_op_string(int op);
static char* get_option_string(int option);

static char *telops[] = {
  "IAC", "DONT", "DO", "WONT", "WILL", "SB", "GA", "EL", "EC", "AYT", "AO", "IP", "BREAK", "DM", "NOP", "SE"
};

static char* telopts[] = {
  "BINARY", "ECHO", "RCP", "SGA", "NAMS", "STATUS", "TM", "RCTE", "NAOL", "NAOP", "NAOCRD",
  "NAOHTS", "NAOHTD", "NAOFFD", "NAOVTS", "NAOVTD", "NAOLFD", "XASCII", "LOGOUT", "BM", "DET",
  "SUPDUP", "SUPDUPOUTPUT", "SNDLOC", "TTYPE", "EOR", "TUID", "OUTMRK", "TTYLOC", "3270REGIME",
  "X3PAD", "NAWS", "TSPEED", "LFLOW", "LINEMODE", "XDISPLOC", "OLD_ENVIRON", "AUTHENTICATION",
  "ENCRYPT", "NEW_ENVIRON"
};

static telnet_config_t opt_config[] = {
  { TELOPT_ECHO, true, false },
  { TELOPT_SGA, false, false },
  { 0, false, false }
};


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

  telnet->suppress_go_ahead.us = NO;
  telnet->suppress_go_ahead.them = NO;

  return telnet;
}

/**
 * Frees an allocated instance of telnet_t
 *
 * telnet - the telnet_t instance to be freed
**/
void network_free_telnet_t(telnet_t* telnet) {
  assert(telnet);

  telnet_extension_t* ext = telnet->extensions;

  while (ext != NULL) {
    telnet_extension_t* next = ext->next;

    ext->deallocate(ext->extension);

    ext = next;
  }

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
 * Allows extensions to be registered with Telnet.
 *
 * telnet - a reference to the telnet_t instance to add the extension to
 * extension - the extension to be added
**/
void network_register_telnet_extension(telnet_t* telnet, telnet_extension_t* extension) {
  if (telnet->extensions == NULL) {
    telnet->extensions = extension;

    return;
  }

  telnet_extension_t* ext = telnet->extensions;

  while (ext->next != NULL) {
    ext = ext->next;
  }

  ext->next = extension;
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

  network_telnet_send_do(telnet, client, TELOPT_ECHO);

  telnet_extension_t* ext = telnet->extensions;

  while (ext != NULL) {
    if (ext->initialise != NULL) {
      ext->initialise(ext->extension, telnet, client);
    }

    ext = ext->next;
  }
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
      log_telnet_parse(ps, cmd_len, true);

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

      int j = move_index + cmd_len;

      if (j + 1 > len) {
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
  int cmd_len = 0;

  for (int i = 0; i < len; i++) {
    update_parse_state(ps, output[i], i, &iac_index);

    cmd_len = ps->state != READ_IAC ? cmd_len + 1 : cmd_len;

    if (ps->state == DONE) {
      log_telnet_parse(ps, cmd_len, false);

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

  option_state_t* state = get_local_option(telnet, option);

  if (state == NULL) {
    return -1;
  }

  switch(*state) {
    case NO: // No current negotiation for option, say we will do this
      if (send_raw_will(client, option) == -1) {
        LOG(ERROR, "Telnet request WILL [%d] failed, couldn't send to client", option);

        return -1;
      }

      *state = WANT_YES;
      break;

    case YES: // We've already enabled this option, don't do anything else
      LOG(WARN, "Telnet request WILL [%d] failed, option already enabled", option);
      break;

    case WANT_YES_OPPOSITE: // Sent a WILL, followed by a WONT, then another WILL.
      *state = WANT_YES;     
      break;

    case WANT_NO: // We've already sent a DONT for this option but are waiting for a response
      *state = WANT_NO_OPPOSITE;
      break;

    case WANT_NO_OPPOSITE: // We've already sent a DONT, followed by a DO.  Just no.
    case WANT_YES: // We've already sent a DO
      LOG(WARN, "Telnet request WILL [%d] failed, option already under negotiation", option);
      break;
  }

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

  option_state_t* state = get_local_option(telnet, option);

  if (state == NULL) {
    return -1;
  }

  switch(*state) {
    case NO: // Client already has disabled this option, don't do anything
      LOG(WARN, "Telnet request WONT [%d] failed, option already disabled", option);
      break;

    case YES: // No current negotiation for this object, send dont
      if (send_raw_wont(client, option) == -1) {
        LOG(ERROR, "Telnet request WONT [%d] failed, couldn't send to client", option);

        return -1;
      }

      *state = WANT_NO;
      break;

    case WANT_NO_OPPOSITE: // DONT, followed by DO, followed by DONT while waiting for a response.
      *state = WANT_NO;
      break;

    case WANT_YES: // DO, followed by DONT while waiting for a response.
      *state = WANT_YES_OPPOSITE;
      break;

    case WANT_YES_OPPOSITE:
    case WANT_NO: // We've already sent a DONT for this option but are waiting for a response
      LOG(WARN, "Telnet request WONT [%d] failed, option already under negotiation", option);     
      break;
  }

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

  option_state_t* state = get_client_option(telnet, option);

  if (state == NULL) {
    LOG(ERROR, "Unsupported DO request for option [%d]", option);

    return -1;
  }

  switch(*state) {
    case NO: // No current negotiation for option, say we want client to do this
      if (send_raw_do(client, option) == -1) {
        LOG(ERROR, "Telnet request DO [%d] failed, couldn't send to client", option);

        return -1;
      }

      *state = WANT_YES;
      break;

    case YES: // Client already has enabled this option, don't do anything else
      LOG(WARN, "Telnet request DO [%d] failed, option already enabled", option);
      break;

    case WANT_YES_OPPOSITE: // Sent a DO, followed by a DONT, then another DO.
      *state = WANT_YES;     
      break;

    case WANT_NO: // We've already sent a DONT for this option but are waiting for a response
      *state = WANT_NO_OPPOSITE;
      break;

    case WANT_NO_OPPOSITE: // We've already sent a DONT, followed by a DO.  Just no.
    case WANT_YES: // We've already sent a DO
      LOG(WARN, "Telnet request DO [%d] failed, option already under negotiation", option);
      break;
  }

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

  option_state_t* state = get_client_option(telnet, option);

  if (state == NULL) {
    LOG(ERROR, "Unsupported DONT request for option [%d]", option);

    return -1;
  }

  switch(*state) {
    case NO: // Client already has disabled this option, don't do anything
      LOG(WARN, "Telnet request DONT [%d] failed, option already disabled", option);
      break;

    case YES: // No current negotiation for this object, send dont
      if (send_raw_dont(client, option) == -1) {
        LOG(ERROR, "Telnet request DONT [%d] failed, couldn't send to client", option);

        return -1;
      }

      *state = WANT_NO;
      break;

    case WANT_NO_OPPOSITE: // DONT, followed by DO, followed by DONT while waiting for a response.
      *state = WANT_NO;
      break;

    case WANT_YES: // DO, followed by DONT while waiting for a response.
      *state = WANT_YES_OPPOSITE;
      break;

    case WANT_YES_OPPOSITE:
    case WANT_NO: // We've already sent a DONT for this option but are waiting for a response
      LOG(WARN, "Telnet request DONT [%d] failed, option already under negotiation", option);     
      break;
  }

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
  option_state_t* state = get_local_option(telnet, option);

  if (state == NULL) {    
    if (send_raw_wont(client, option) == -1) {
      LOG(ERROR, "Failed to send WONT for unsupported DO [%d]", option);
    }

    return;
  }

  telnet_config_t* config = get_option_config(telnet, option);

  if (config == NULL) {
    LOG(ERROR, "Failed to retrieve configuration for option [%d]", option);
    return;
  }
  
  switch(*state) {
    case NO: // Option disabled and we're not currently negotiating, respond will and enable 
      if (config->accept_do) {        
        if (send_raw_will(client, option) == -1) {
          LOG(ERROR, "Failed to send WILL response for supported DO [%d]", option);
          return;
        }

        LOG(INFO, "Client [%d] enabled local [%s] telnet option", client->fd, get_option_string(option));

        *state = YES;
      } else {
        if (send_raw_wont(client, option) == -1) {
          LOG(ERROR, "Failed to send WONT response for supported DO [%d]", option);
          return;
        }

        *state = NO;
      }
      
      break;

    case YES: // Option already enabled, don't do anything
      break;

    case WANT_NO: // We've sent a WONT for this already, disable option
      *state = NO;

      break;

    case WANT_NO_OPPOSITE: // We've sent a WONT followed by a WILL for this, enable option
      *state = YES;      
      break;

    case WANT_YES: // We've sent a WILL, enable option
      LOG(INFO, "Got DO for option [%d]", option);
      *state = YES;

      break;

    case WANT_YES_OPPOSITE: // We've sent a WILL, followed by a WONT
      if (send_raw_wont(client, option) == -1) {
        LOG(ERROR, "Failed to send queued WONT for supported DO [%d]", option);
        return;
      }

      *state = WANT_NO;
      
      break;
  }
}

/**
 * Processes a DONT request from the remote host.
 *
 * telnet - telnet_t object we use to assess and update options
 * client - client we may need to respond to
 * option - the option that has been asked to DO
**/
void process_dont_incoming(telnet_t* telnet, client_t* client, int option) {
  option_state_t* state = get_local_option(telnet, option);

  if (state == NULL) {
    return;
  }

  switch(*state) {
    case NO: // Option already disabled, don't do anything
      break;

    case YES: // Option is enabled and there is no negotiation.  Send WONT, disable option    
      *state = NO;

      LOG(INFO, "Client [%d] disabled local [%s] telnet option", client->fd, get_option_string(option));

      if (send_raw_wont(client, option) == -1) {
        LOG(ERROR, "Failed to send WONT response for supported DONT [%d]", option);
      }

      break;

    case WANT_NO: // We've sent a WONT for this, client has agreed, disable option.
      *state= NO;

      break;

    case WANT_NO_OPPOSITE: // We've sent a WONT, followed by a WILL, send queued WILL
      if (send_raw_will(client, option) == -1) {
        LOG(ERROR, "Failed to send queued WILL for supported DONT [%d]", option);
      }

      *state = WANT_YES;
      
      break;

    case WANT_YES: // We've sent a WILL, but client doesn't want to, disable option
      *state = NO;

      break;

    case WANT_YES_OPPOSITE: // We've sent a WILL, followed by a WONT, client says they wont
      *state = NO;

      break;
  }
}

/**
 * Processes a WILL request from the remote host.
 *
 * telnet - telnet_t object we use to assess and update options
 * client - client we may need to respond to
 * option - the option that has been asked to DO
**/
void process_will_incoming(telnet_t* telnet, client_t* client, int option) {
  option_state_t* state = get_client_option(telnet, option);

  if (state == NULL) {
    if (send_raw_dont(client, option) == -1) {
      LOG(ERROR, "Failed to send DONT for unsupported WILL [%d]", option);
    }

    return;
  }

    telnet_config_t* config = get_option_config(telnet, option);

    if (config == NULL) {
      LOG(ERROR, "Failed to retrieve configuration for option [%d]", option);
      return;
    }

  switch(*state) {
    case NO: // Option disabled and we're not currently negotiating, respond do and enable
      if (config->accept_will) {
        if (send_raw_do(client, option) == -1) {
          LOG(ERROR, "Failed to send DO response for supported WILL [%d]", option);
          return;
        }

        LOG(INFO, "Client [%d] enabled client [%s] telnet option", client->fd, get_option_string(option));

        *state = YES;
      } else {
        if (send_raw_dont(client, option) == -1) {
          LOG(ERROR, "Failed to send DONT response for supported WILL [%d]", option);
          return;
        }

        *state = NO;
      }


      break;

    case YES: // Option already enabled, don't do anything
      break;

    case WANT_NO: // We've sent a DONT for this already, disable option
      *state = NO;

      break;

    case WANT_NO_OPPOSITE: // We've sent a DONT followed by a DO for this, enable option
      *state = YES;
      break;

    case WANT_YES: // We've sent a DO, client agrees, enable option
      *state = YES;

      break;

    case WANT_YES_OPPOSITE: // We've sent a DO, followed by a DONT
      if (send_raw_dont(client, option) == -1) {
        LOG(ERROR, "Failed to send queued DONT for supported WILL [%d]", option);
        return;
      }

      *state = WANT_NO;
      
      break;
  }
}

/**
 * Processes a WONT request from the remote host.
 *
 * telnet - telnet_t object we use to assess and update options
 * client - client we may need to respond to
 * option - the option that has been asked to DO
**/
void process_wont_incoming(telnet_t* telnet, client_t* client, int option) {
  option_state_t* state = get_client_option(telnet, option);

  if (state == NULL) {
    return;
  }

  switch(*state) {
    case NO: // Option already disabled, don't do anything
      break;

    case YES: // Option is enabled and there is no negotiation.  Send DONT, disable option
      *state = NO;

      LOG(INFO, "Client [%d] disabled client [%s] telnet option", client->fd, get_option_string(option));

      if (send_raw_dont(client, option) == -1) {
        LOG(ERROR, "Failed to send DONT response for supported WONT [%d]", option);
      }

      break;

    case WANT_NO: // We've sent a DONT for this, client has agreed, disable option.
      *state= NO;

      break;

    case WANT_NO_OPPOSITE: // We've sent a DONT, followed by a DO, send queued DO
      if (send_raw_do(client, option) == -1) {
        LOG(ERROR, "Failed to send queued DO for supported WONT [%d]", option);
      }

      *state = WANT_YES;
      
      break;

    case WANT_YES: // We've sent a DO, but client doesn't want to, disable option
      *state = NO;

      break;

    case WANT_YES_OPPOSITE: // We've sent a DO, followed by a DONT, client says they wont
      *state = NO;

      break;
  }
}

/**
 * Sends an IAC DO OPTION to the client.
 
 * client - the client to sent the DO to
 * option - the option to send DO for
 *
 * Returns 0 on success or -1 on failure
**/
static int send_raw_do(client_t* client, int option) {
  char msg[] = { (char) IAC, (char) DO, (char) option };
  
  if (send_to_client(client, msg, 3) == -1) {
    LOG(ERROR, "Failed to send Telnet DO [%d] to client", option);
    
    return -1;
  }

  return 0;
}

/**
 * Sends an IAC DONT OPTION to the client.
 
 * client - the client to sent the DONT to
 * option - the option to send DONT for
 *
 * Returns 0 on success or -1 on failure
**/
static int send_raw_dont(client_t* client, int option) {
  char msg[] = { (char) IAC, (char) DONT, (char) option };
  
  if (send_to_client(client, msg, 3) == -1) {
    LOG(ERROR, "Failed to send Telnet DONT [%d] to client", option);

    return -1;
  }

  return 0;
}

/**
 * Sends an IAC WILL OPTION to the client.
 
 * client - the client to sent the WILL to
 * option - the option to send WILL for
 *
 * Returns 0 on success or -1 on failure
**/
static int send_raw_will(client_t* client, int option) {
  char msg[] = { (char) IAC, (char) WILL, (char) option };
  
  if (send_to_client(client, msg, 3) == -1) {
    LOG(ERROR, "Failed to send Telnet WILL [%d] to client", option);

    return -1;
  }

  return 0;
}

/**
 * Sends an IAC WONT OPTION to the client.
 
 * client - the client to sent the WONT to
 * option - the option to send WONT for
 *
 * Returns 0 on success or -1 on failure
**/
static int send_raw_wont(client_t* client, int option) {
  char msg[] = { (char) IAC, (char) WONT, (char) option };
  
  if (send_to_client(client, msg, 3) == -1) {
    LOG(ERROR, "Failed to send Telnet WONT [%d] to client", option);

    return -1;
  }

  return 0;
}

/**
 * Retreives an option from the telnet_t structure if supported.
 *
 * telnet - the telnet_t structure containing options for this client
 * option - the option code being referenced
 *
 * Retrieves option if found or NULL
**/
telnet_option_t* get_option(telnet_t* telnet, int option) {
  switch(option) {
    case TELOPT_SGA:
      return &telnet->suppress_go_ahead;
    case TELOPT_ECHO:
      return &telnet->echo;
  }

  telnet_extension_t* ext = telnet->extensions;
  telnet_option_t* opt = NULL;

  while (ext != NULL && opt == NULL) {
    if (ext->get_option != NULL) {
      opt = ext->get_option(ext->extension, option);
    }
    
    ext = ext->next;
  }

  return opt;
}

/**
 * Retrieves the local state of an option if supported.
 *
 * telnet - the telnet_t structure containing options for this client
 * option - the option code being referenced
 *
 * Returns local option state it found or NULL
**/
option_state_t* get_local_option(telnet_t* telnet, int option) {
  telnet_option_t* opt = get_option(telnet, option);

  if (opt == NULL) {
    return NULL;
  }

  return &opt->us;
}

/**
 * Retrieves the client state of an option if supported.
 *
 * telnet - the telnet_t structure containing options for this client
 * option - the option code being referenced
 *
 * Returns client option state it found or NULL
**/
option_state_t* get_client_option(telnet_t* telnet, int option) {
  telnet_option_t* opt = get_option(telnet, option);

  if (opt == NULL) {
    return NULL;
  }

  return &opt->them;
}

/**
 * Retrieves the configuration for a given telnet option.
 *
 * telnet - the telnet_t instance for extensions
 * option - the option to find the configuration for
 *
 * Returns the config or NULL
**/
telnet_config_t* get_option_config(telnet_t* telnet, int option) {
  telnet_config_t* config = &opt_config[0];

  while (config->option != 0) {
    if (config->option == option) {
      return config;
    }

    config++;
  }

  config = NULL;
  telnet_extension_t* ext = telnet->extensions;

  while (ext != NULL && config == NULL) {
    if (ext->get_config != NULL) {
      config = ext->get_config(telnet, option);
    }

    ext = ext->next;
  }

  return config;
}

/**
 * Utility method that logs Telnet requests
**/
void log_telnet_parse(telnet_parse_t* ps, size_t len, bool in) {
  if (len == 2) {
    LOG(DEBUG, "[%s] IAC %s", in == true ? "IN" : "OUT", get_op_string(ps->op));
  } else {
    LOG(DEBUG, "[%s] IAC %s %s", in == true ? "IN" : "OUT", get_op_string(ps->op), get_option_string(ps->option));    
  }
}


/**
 * Retrieve a string representing an operation.
 *
 * op - operation to retrieve string for
 *
 * Returns string representing op or NULL
**/
char* get_op_string(int op) {
  char* str = NULL;
  int index = 255 - op;

  if (index <= 15) {
    str = telops[index];
  }

  return str;
}

/**
 * Retrieves a string representing an option.
 *
 * option - option to search for
 *
 * Returns string representing option or NULL
**/
char* get_option_string(int option) {
  char* op = NULL;

  if (option <= 39) {
    op = telopts[option];
  }

  return op;
}
