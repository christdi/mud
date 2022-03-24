#include "mud/game.h"

/**
 * Entry point for the application.  Will exit if unable to load to
 * configuration or initialise logging.  Otherwise, starts the game.
 **/
int main(int argc, char* argv[]) {
  if (start_game(argc, argv) != 0) {
    return -1;
  }

  return 0;
}
