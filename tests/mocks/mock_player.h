#ifndef TEST_MOCK_PLAYER_H
#define TEST_MOCK_PLAYER_H

#include "fff.h"

#include "mud/event.h"
#include "mud/player.h"

DECLARE_FAKE_VOID_FUNC(player_on_event, player_t*, game_t*, event_t*);

void mock_player_reset(void);

#endif
