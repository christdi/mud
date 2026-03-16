#include <stddef.h>

#include "mock_player.h"

static int call_count = 0;
static player_t* recorded_players[MOCK_MAX_RECORDED_CALLS];
static event_t* recorded_events[MOCK_MAX_RECORDED_CALLS];

void mock_player_reset(void) {
  call_count = 0;

  for (int i = 0; i < MOCK_MAX_RECORDED_CALLS; i++) {
    recorded_players[i] = NULL;
    recorded_events[i] = NULL;
  }
}

int mock_player_on_event_call_count(void) {
  return call_count;
}

player_t* mock_player_on_event_player(int index) {
  if (index < 0 || index >= call_count) {
    return NULL;
  }

  return recorded_players[index];
}

event_t* mock_player_on_event_event(int index) {
  if (index < 0 || index >= call_count) {
    return NULL;
  }

  return recorded_events[index];
}

/* Linker-wrap replacement for player_on_event (--wrap=player_on_event).
   Records invocations so tests can assert on call count and arguments. */
void __wrap_player_on_event(player_t* player, game_t* game, event_t* event) {
  (void)game;

  if (call_count < MOCK_MAX_RECORDED_CALLS) {
    recorded_players[call_count] = player;
    recorded_events[call_count] = event;
  }

  call_count++;
}
