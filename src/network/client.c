#include "mud/network/client.h"
#include "mud/string.h"

#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <zlog.h>

client_t * network_client_new() {
    client_t * client = calloc(1, sizeof * client);

    client->fd = 0;
    client->thread = 0;

    return client;
}

const int network_client_send(client_t * client, char * data) {

    zlog_category_t * networkCategory = zlog_get_category("network");

	if ( !client ) {
		zlog_error(networkCategory, "network_client_send: Client was null.");

		return -1;
	}

	if ( !data ) {
		zlog_error(networkCategory, "network_client_send: Data was null.");

		return -1;
	}

	int len = strlen(data);
	int bytes_sent = 0;

	while ( bytes_sent < len ) {
		data = data + bytes_sent;
		
		bytes_sent = send(client->fd, data, len, 0);

		if ( bytes_sent == -1 ) {
			zlog_error(networkCategory, "network_client_send: %s", strerror(errno));

			return -1;
		} else if ( bytes_sent == 0 ) {
			return -1;
		}
	}

	return 0;
}

void network_client_free(client_t * client) {
    if ( client ) {
        free(client);
    }
}
