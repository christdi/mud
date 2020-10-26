#ifndef _CLIENT_H_
#define _CLIENT_H_

#define MAX_INPUT_BUFFER_SIZE 1024
#define MAX_RECV_SIZE 128

#include "mud/util/muduuid.h"

#include <pthread.h>


/**
 * Structs
**/
typedef struct client {
    int fd;  
    unsigned int hungup;

    char uuid[UUID_SIZE];
    char input[MAX_INPUT_BUFFER_SIZE];
} client_t;


/**
 * Function prototypes
**/
client_t * create_client_t(void);
void free_client_t(client_t * client);

int send_to_client(client_t * client, char * data);
int receive_from_client(client_t * client);
int close_client(client_t * client);


#endif
