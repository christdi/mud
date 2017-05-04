#ifndef _NETWORK_H_
#define _NETWORK_H_

struct server {
	int port;
};

typedef struct server server;

server * network_server_new();
const int network_server_listen(server * server);
const int network_server_close(server * server);
void network_server_free(server * server);


#endif