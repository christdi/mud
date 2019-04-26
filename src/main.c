#include <stdio.h>
#include <zlog.h>

#include "mud/config.h"
#include "mud/game.h"
#include "mud/string.h"

const int load_configuration();

int main(int argc, char *argv[]) {
    config_t * config = config_new();

    if ( !config ) {
        printf("Unable to allocate configuration.  Shutting down\n\r");
        fflush(stdout);

        return -1;
    }

    if ( load_configuration("config.ini", config) != 0 ) {
        printf("Unable to load configuration.  Shutting down\n\r");
        fflush(stdout);

        return -1;
    }

    if ( zlog_init(config->logConfigFile) != 0 ) {
        printf("Unable to initialise logging.  Shutting down\n\r");
        fflush(stdout);

        return -1;
    }

    if ( game_run(config) != 0 ) {
        return -1;
    }

    config_free(config);

    fflush(stdout);

    zlog_fini();

    return 0;
}

const int load_configuration(const char * filename, config_t * config) {
    if ( config_load(filename, config) != 0 ) {
        config_free(config);

        return -1;
    }

    return 0;
}
