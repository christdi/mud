#ifndef TEST_MOCK_PLAYER_H
#define TEST_MOCK_PLAYER_H

#include "mud/event.h"
#include "mud/player.h"

#define MOCK_MAX_RECORDED_CALLS 16

void mock_player_reset(void);
int mock_player_on_event_call_count(void);
player_t* mock_player_on_event_player(int index);
event_t* mock_player_on_event_event(int index);

#endif
