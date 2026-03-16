#include "mock_player.h"

DEFINE_FAKE_VOID_FUNC(player_on_event, player_t*, game_t*, event_t*);

void mock_player_reset(void) {
  RESET_FAKE(player_on_event);
}
