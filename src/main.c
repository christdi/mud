#include <stdio.h>
#include <stdlib.h>

#include "mud/config.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/util/mudstring.h"

/**
 * Entry point for the application.  Will exit if unable to load to 
 * configuration or initialise logging.  Otherwise, starts the game.
**/
int main(int argc, char* argv[]) {
  config_t* config = config_new();

  if (!config || load_configuration("config.ini", config) != 0) {
    printf("Unable to load [config.ini].  Using default configuration\n\r");
  }

  if (!config || parse_configuration(argc, argv, config) != 0) {
    exit(-1);
  }

  if (log_initialise(config->log_config_file) != 0) {
    printf("Unable to initialise logging.  Shutting down\n\r");
    exit(-1);
  }

  if (start_game(config) != 0) {
    exit(-1);
  }

  log_shutdown();

  config_free(config);

  return 0;
}
