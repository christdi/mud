#ifndef MUD_NETWORK_CLIENT_H
#define MUD_NETWORK_CLIENT_H

#include "mud/util/muduuid.h"

#include <pthread.h>
#include <time.h>

/**
 * Definitions
**/
#define INPUT_BUFFER_SIZE 1025 // 1 KB + null terminator
#define INPUT_BUFFER_LENGTH INPUT_BUFFER_SIZE - 1 // 1 KB
#define DELIM_SIZE 2

/**
 * Structs
**/
typedef struct client {
  int fd;
  unsigned int hungup;
  time_t last_active;
  void* userdata;

  char input[INPUT_BUFFER_SIZE];
} client_t;

/**
 * Function prototypes
**/
client_t* create_client_t(void);
void free_client_t(client_t* client);

int send_to_client(client_t* client, char* data);
int receive_from_client(client_t* client);
int close_client(client_t* client);
int client_get_idle_seconds(const client_t* const client);
int extract_from_input(client_t* client, char* dest, size_t dest_len, const char* delim);

#endif