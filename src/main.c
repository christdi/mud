#include <stdio.h>
#include <stdlib.h>

#include "mud/config.h"
#include "mud/game.h"
#include "mud/mudstring.h"
#include "mud/log/log.h"

/**
 * Entry point for the application.  Will exit if unable to load to 
 * configuration or initialise logging.  Otherwise, starts the game.
**/
int main(int argc, char *argv[]) {
  config_t * config = config_new();

  if (!config || load_configuration("config.ini", config) != 0) {
    printf("Unable to load configuration.  Shutting down\n\r");
    exit(-1);
  }

  if (log_initialise(config->logConfigFile) != 0) {
    printf("Unable to initialise logging.  Shutting down\n\r");

    exit(-1);
  }

  game_t * game = create_game_t();

  if (start_game(game, config) != 0) {
    exit(-1);
  }

  free_game_t(game);

  log_shutdown();

  return 0;
}
