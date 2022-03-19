#ifndef MUD_NETWORK_CLIENT_H
#define MUD_NETWORK_CLIENT_H

#include "mud/util/muduuid.h"

#include <pthread.h>
#include <time.h>

/**
 * Definitions
 **/
#define CLIENT_BUFFER_SIZE (1024 * 5) + 1 // 5 KB + null terminator
#define CLIENT_BUFFER_LENGTH CLIENT_BUFFER_SIZE - 1 // 5 KB
#define DELIM_SIZE 2

/**
 * Typedefs
**/
typedef struct protocol protocol_t;

/**
 * Structs
 **/
typedef struct client {
  int fd;
  unsigned int hungup;
  time_t last_active;
  void* userdata;
  protocol_t* protocol;

  char input[CLIENT_BUFFER_SIZE];
  char output[CLIENT_BUFFER_SIZE];
  size_t output_length;
} client_t;

/**
 * Function prototypes
 **/
client_t* create_client_t(void);
void free_client_t(client_t* client);

int send_to_client(client_t* client, char* data, size_t len);
int flush_client_output(client_t* client);
int receive_from_client(client_t* client);
int close_client(client_t* client);
int client_get_idle_seconds(const client_t* const client);
int extract_from_input(client_t* client, char* dest, size_t dest_len, const char* delim);
int network_add_protocol_to_client(client_t* client, protocol_t* protocol);

#endif