#include "mud/network/server.h"
#include "mud/network/client.h"
#include "mud/string.h"

#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <zlog.h>

server_t * network_server_new() {
    server_t * server = calloc(1, sizeof * server);

    server->fd = 0;
    server->thread = 0;
    server->port = 0;
    server->backlog = 10;
    server->clients = list_new();

    return server;
}

const int network_server_initialise(server_t * server) {
    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( network_server_create_thread(server) == -1 ) {
        zlog_error(networkCategory, "Failed to create server accept thread.");

        return -1;
    }

    return 0;
}


const int network_server_listen(server_t * server) {
    assert(server);
    assert(server->port != 0);

    zlog_category_t * networkCategory = zlog_get_category("network");

    struct addrinfo hints;
    struct addrinfo * serverInfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char * portString = string_integer_to_ascii(server->port);

    if ( !portString ) {
        zlog_error(networkCategory, "Failed to convert port from integer to string.");

        return -1;
    }

    int status = 0;

    if (( status = getaddrinfo(0, portString, &hints, &serverInfo)) != 0 ) {
        zlog_error(networkCategory, "%s", gai_strerror(status));

        return -1;
    }

    zlog_debug(networkCategory, "Successfully got address information.");

    server->fd = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

    if ( !server->fd ) {
        zlog_error(networkCategory, "%s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "Successfully got socket.");

    int yes = 1;

    if ( setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) != 0 ) {
        zlog_error(networkCategory, "%s", strerror(errno));

        return -1;
    }

    if ( fcntl(server->fd, F_SETFL, O_NONBLOCK) != 0 ) {
        zlog_error(networkCategory, "%s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "Successfully set socket option.");

    if ( bind(server->fd, serverInfo->ai_addr, serverInfo->ai_addrlen) != 0 ) {
        zlog_error(networkCategory, "%s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "Successfully bound port.");

    if ( listen(server->fd, server->backlog) != 0 ) {
        zlog_error(networkCategory, "%s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "Successfully listened on port.");

    freeaddrinfo(serverInfo);

    zlog_info(networkCategory, "Successfully bound to port [%d].", server->port);

    return 0;
}

const int network_server_create_thread(server_t * server) {
    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( pthread_create(&server->thread, NULL, network_server_accept_thread, server) != 0 ) {
        zlog_error(networkCategory, "%s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "Successfully created server accept thread.");

    return 0;
}

void * network_server_accept_thread(void * serverThreadData) {
    zlog_category_t * networkCategory = zlog_get_category("network");

    server_t * server = (server_t *) serverThreadData;

    struct timeval timeout;

    fd_set readSet;

    while (server->fd > 0) {
        FD_ZERO(&readSet);        
        FD_SET(server->fd, &readSet);

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int results = select(server->fd + 1, &readSet, NULL, NULL, &timeout);
        
        if ( results == -1 ) {
            if ( errno == EWOULDBLOCK ) {
                zlog_debug(networkCategory, "Server would block, polling again.");

                continue;
            }

            zlog_error(networkCategory, "%s", strerror(errno));

            break;
        } else if ( results == 0 ) {
            zlog_debug(networkCategory, "Timeout, polling again.");

            continue;
        } else if ( results > 0 ) {
            if ( FD_ISSET(server->fd, &readSet) ) {
                struct sockaddr_storage remoteAddress;
                socklen_t remoteAddressSize = sizeof remoteAddress;

                client_t * client = network_client_new();
                client->fd = accept(server->fd, (struct sockaddr *)&remoteAddress, &remoteAddressSize);

                if ( !client->fd ) {
                    zlog_error(networkCategory, "%s", strerror(errno));
                    network_client_free(client);
                    continue;
                }

                if ( network_client_create_thread(client) != 0 ) {
                    zlog_error(networkCategory, "Failed to create receive thread");
                    network_client_free(client);
                    continue;
                }

                node_t * node = node_new();
                node->data = client;

                list_insert(server->clients, node);

                zlog_info(networkCategory, "New client with descriptor [%d]", client->fd);

                network_client_send(client, "Hello\n\r");
            }
        }
    }

    zlog_debug(networkCategory, "Server shutdown, returning from thread.");

    return 0;
}

const int network_server_poll_clients(server_t * server) {
    assert(server);
    assert(server->clients);

    zlog_category_t * networkCategory = zlog_get_category("network");

    node_t * node = NULL;
    list_first(server->clients, &node);

    while (node != NULL) {
        client_t * client = (client_t *) node->data;

        if ( client->hungup == 1) {
            zlog_info(networkCategory, "Client descriptor [%d] disconnected", client->fd);

            if ( network_client_close(client) == -1 ) {
                zlog_error(networkCategory, "Failed to close hungup client");            
            }

            list_remove(server->clients, node);
            network_client_free(client);
            node_free(node);
        }

        list_next(server->clients, &node);
    }

    return 0;
}

const int network_server_join_thread(server_t * server) {
    assert(server);
    assert(server->thread);

    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( pthread_join(server->thread, 0) != 0 ) {
        zlog_error(networkCategory, "%s", strerror(errno));

        return -1;
    }

    return 0;
}

const int network_server_shutdown(server_t * server) {
    assert(server);

    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( network_server_close(server) == -1 ) {
        zlog_error(networkCategory, "Network server failed to close.");

        return -1;
    }

    if ( network_server_join_thread(server) == -1 ) {
        zlog_error(networkCategory, "Network server join thread failed.");

        return -1;
    }

    return 0;
}

const int network_server_close(server_t * server) {
    assert(server);

    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( server->fd ) {
        zlog_info(networkCategory, "Closing server on port [%d] with descriptor [%d]", server->port, server->fd);

        if ( close(server->fd) != 0 ) {
            zlog_error(networkCategory, "%s", strerror(errno));

            return -1;
        }

        server->fd = 0;
    }

    return 0;
}

void network_server_free(server_t * server) {
    assert(server);
    assert(server->clients);

    node_t * node = NULL;

    list_first(server->clients, &node);

    while (node != NULL) {
        client_t * client = (client_t *)node->data;

        network_client_free(client);

        list_next(server->clients, &node);
    }

    list_free(server->clients);

    free(server);
}
