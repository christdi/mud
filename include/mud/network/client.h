#ifndef _CLIENT_H_
#define _CLIENT_H_

#define INPUT_BUFFER_SIZE 1024
#define RECV_SIZE 128
#define DELIM_SIZE 2

#include "mud/util/muduuid.h"

#include <pthread.h>


/**
 * Structs
**/
typedef struct client {
    int fd;  
    unsigned int hungup;

    char uuid[UUID_SIZE];
    char input[INPUT_BUFFER_SIZE];
} client_t;


/**
 * Function prototypes
**/
client_t * create_client_t(void);
void free_client_t(client_t * client);

int send_to_client(client_t * client, char * data);
int receive_from_client(client_t * client);
int close_client(client_t * client);
int extract_from_input(client_t * client, char * dest, size_t dest_len, const char * delim);


#endif
