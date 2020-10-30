#include "mud/command/communication.h"
#include "mud/log/log.h"
#include "mud/util/mudstring.h"
#include "mud/player.h"


/**
**/
void say_command(player_t * player, game_t * game, char * input) {
	zlog_info(gc, "Input: [%s]", input);
	
	if (!input || *input == '\0') {
		send_to_player(player, "Say what?\n\r");
	}
}