#include "mud/network.h"
#include "mud/string.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <zlog.h>

server * network_server_new() {
    server * server = calloc(1, sizeof * server);

    server->fd = 0;
    server->thread = 0;
    server->port = 0;
    server->backlog = 10;

    return server;
}

const int network_server_listen(server * server) {

    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( server->port == 0 ) {
        zlog_error(networkCategory, "network_server_listen: Precondition failed, server port was 0.");

        return -1;
    }

    struct addrinfo hints;
    struct addrinfo * serverInfo;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    char * portString = string_integer_to_ascii(server->port);

    if ( !portString ) {
        zlog_error(networkCategory, "network_server_listen: Failed to convert port from integer to string.");

        return -1;
    }

    int status = 0;

    if (( status = getaddrinfo(0, portString, &hints, &serverInfo)) != 0 ) {
        zlog_error(networkCategory, "network_server_listen: %s", gai_strerror(status));

        return -1;
    }

    zlog_debug(networkCategory, "network_server_listen: Successfully got address information.");

    server->fd = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);

    if ( !server->fd ) {
        zlog_error(networkCategory, "network_server_listen: %s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "network_server_listen: Successfully got socket.");

    int yes = 1;

    if ( setsockopt(server->fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) != 0 ) {
        zlog_error(networkCategory, "network_server_listen: %s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "network_server_listen: Successfully set socket option.");

    if ( bind(server->fd, serverInfo->ai_addr, serverInfo->ai_addrlen) != 0 ) {
        zlog_error(networkCategory, "network_server_listen: %s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "network_server_listen: Successfully bound port.");

    if ( listen(server->fd, server->backlog) != 0 ) {
        zlog_error(networkCategory, "network_server_listen: %s", strerror(errno));

        return -1;
    }

    zlog_debug(networkCategory, "network_server_listen: Successfully listened on port.");

    freeaddrinfo(serverInfo);

    return 0;
}

const int network_server_create_thread(server * server) {
    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( pthread_create(&server->thread, NULL, network_server_accept_thread, &server->fd) != 0 ) {
        zlog_error(networkCategory, "network_server_create_thread: %s", strerror(errno));

        return -1;
    }

    return 0;
}

void * network_server_accept_thread(void * fd) {
    zlog_category_t * networkCategory = zlog_get_category("network");

    zlog_info(networkCategory, "network_server_accept_thread: In thread");

    return 0;
}

const int network_server_join_thread(server * server) {
    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( !server->thread ) {
        zlog_error(networkCategory, "network_server_join_thread: Precondition failed, no thread ID to join");

        return -1;
    }

    if ( pthread_join(server->thread, 0) != 0 ) {
        zlog_error(networkCategory, "network_server_join_thread: %s", strerror(errno));

        return -1;
    }

    return 0;
}

const int network_server_close(server * server) {
    zlog_category_t * networkCategory = zlog_get_category("network");

    if ( server->fd ) {
        if ( close(server->fd) != 0 ) {
            zlog_error(networkCategory, "network_server_close: %s", strerror(errno));

            return -1;
        }
    }

    return 0;
}

void network_server_free(server * server) {
    if ( server ) {
        free(server);
    }
}
