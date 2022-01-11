#include <assert.h>
#include <stdlib.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "mud/game.h"
#include "mud/command/command.h"
#include "mud/config.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/ecs.h"
#include "mud/lua/game_api.h"
#include "mud/lua/db_api.h"
#include "mud/lua/hooks.h"
#include "mud/log.h"
#include "mud/narrator/narrator.h"
#include "mud/network/network.h"
#include "mud/player.h"
#include "mud/task/task.h"
#include "mud/template.h"

int connect_to_database(game_t* game, const char* filename);
void game_execute_tasks(game_t* game);
int game_pulse_players(game_t* game);
void game_sleep_until_tick(game_t* game, unsigned int ticks_per_second);
int initialise_lua(game_t* game, config_t* config);

/**
 * Allocate a new instance of a game_t struct.
 *
 * Returns an allocated game_t struct with default values.
**/
game_t* create_game_t(void) {
  game_t* game = calloc(1, sizeof *game);

  game->shutdown = 0;
  gettimeofday(&game->last_tick, NULL);

  game->database = NULL;

  game->templates = create_hash_table_t();
  game->templates->deallocator = template_t_deallocate;

  game->players = create_hash_table_t();
  game->players->deallocator = deallocate_player;

  game->entities = create_hash_table_t();
  game->entities->deallocator = deallocate_entity;

  game->components = create_linked_list_t();
  game->components->deallocator = deallocate_component_t;

  game->tasks = create_linked_list_t();
  game->tasks->deallocator = deallocate_task_t;

  game->events = create_linked_list_t();

  game->network = create_network_t();
  game->narrator = create_narrator_t();

  game->lua_state = NULL;

  return game;
}

/**
 * Frees an allocated game_t struct.
**/
void free_game_t(game_t* game) {
  assert(game);
  assert(game->players);
  assert(game->network);
  assert(game->components);
  assert(game->narrator);

  free_hash_table_t(game->templates);
  free_hash_table_t(game->players);
  free_hash_table_t(game->entities);

  free_linked_list_t(game->components);
  free_linked_list_t(game->tasks);
  free_linked_list_t(game->events);

  free_network_t(game->network);
  free_narrator_t(game->narrator);

  if (game->lua_state != NULL) {
    lua_close(game->lua_state);
  }

  free(game);
}

/**
 * Starts running the game, binds a server and enters the game loop.
 *
 * Returns a 0 on success or -1 on failure.
**/
int start_game(config_t* config) {
  assert(config);

  game_t* game = create_game_t();

  mlog(INFO, "start_game", "Starting MUD engine");

  register_connection_callback(game->network, player_connected, game);
  register_disconnection_callback(game->network, player_disconnected, game);
  register_input_callback(game->network, player_input, game);

  if (template_load_from_file(game->templates, "template.properties") != 0) {
    mlog(ERROR, "start_game", "Failed to load templates");

    return -1;
  }

  if (connect_to_database(game, config->database_file) != 0) {
    mlog(ERROR, "start_game", "Failed to start game server");

    return -1;
  }

  if (initialise_lua(game, config) == -1) {
    mlog(ERROR, "start_game", "Failed to initialise Lua");

    return -1;
  }

  if (load_entities(game) == -1) {
    mlog(ERROR, "start_game", "Failed to load entities");

    return -1;
  }

  if (start_game_server(game->network, config->game_port) == -1) {
    mlog(ERROR, "start_game", "Failed to start game server");

    return -1;
  }

  task_schedule(game->tasks, GAME_PLAYER_PULSE_SECONDS, game_pulse_players);

  while (!game->shutdown) {
    poll_network(game->network);
    update_systems(game);
    task_execute(game->tasks, game);
    narrate_events(game);
    game_sleep_until_tick(game, config->ticks_per_second);
  }

  if (stop_game_server(game->network, config->game_port) == -1) {
    mlog(ERROR, "start_game", "Failed to shutdown server");

    return -1;
  }

  disconnect_clients(game->network);

  sqlite3_close(game->database);

  mlog(INFO, "start_game", "Stopping MUD engine");

  free_game_t(game);

  return 0;
}

/**
 * Attempts to connect to the SQLite3 database with a given filename.  Returns -1 on failure
 * or 0 on success.
**/
int connect_to_database(game_t* game, const char* filename) {
  mlog(INFO, "connect_to_database", "Connecting to database [%s]", filename);

  if (sqlite3_open(filename, &game->database) != SQLITE_OK) {
    mlog(ERROR, "connect_to_database", "Failed to open game database [%s]", sqlite3_errmsg(game->database));

    sqlite3_close(game->database);

    return -1;
  }

  return 0;
}

/**
 * Pulses all currently connected players.
 * 
 * Parameters
 *  game - contains all necessary game data
 * 
 * Returns 0 on success or -1 on failure
**/
int game_pulse_players(game_t* game) {
  assert(game);
  assert(game->players);

  h_it_t it = hash_table_iterator(game->players);

  player_t* player = NULL;

  while ((player = (player_t*)h_it_get(it)) != NULL) {
    player_on_tick(player, game);

    it = h_it_next(it);
  }

  task_schedule(game->tasks, GAME_PLAYER_PULSE_SECONDS, game_pulse_players);

  return 0;
}

/**
 * Forces the game loop to adhere to a spcified ticks per second.  Calculates the elapsed time
 * time since the last time the method was called and makes the thread sleep if it's less than
 * the amount of time calculated per tick.
**/
void game_sleep_until_tick(game_t* game, const unsigned int ticks_per_second) {
  struct timeval current_time;
  gettimeofday(&current_time, NULL);

  time_t seconds_elapsed = current_time.tv_sec - game->last_tick.tv_sec;
  suseconds_t microseconds_elapsed = current_time.tv_usec - game->last_tick.tv_usec;
  long nanoseconds_elapsed = (seconds_elapsed * ONE_SECOND_IN_NANOSECONDS) + (microseconds_elapsed * ONE_SECOND_IN_MICROSECONDS);
  long nanoseconds_per_tick = ONE_SECOND_IN_NANOSECONDS / ticks_per_second;

  if (nanoseconds_elapsed < nanoseconds_per_tick) {
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = nanoseconds_per_tick - nanoseconds_elapsed;

    nanosleep(&sleep_time, NULL);
  }

  game->last_tick = current_time;
}


int initialise_lua(game_t* game, config_t* config) {
  if ((game->lua_state = luaL_newstate()) == NULL) {
    mlog(ERROR, "initialise_lua", "Failed to initialise Lua state");
    return -1;
  }

  luaL_openlibs(game->lua_state);

  if (lua_game_register_api(game->lua_state, game) == -1) {
    mlog(ERROR, "initialise_lua", "Failed to register Lua API with state");
    return -1;
  }

  if (lua_db_register_api(game->lua_state, game->database) == -1) {
    mlog(ERROR, "initialise_lua", "Failed to register Lua DB API with state");
    return -1;
  }

  if (luaL_dofile(game->lua_state, config->game_script_file) != 0) {
    printf("Error while loading Lua game script [%s].\n\r", lua_tostring(game->lua_state, -1));

    return -1;
  }

  lua_hook_on_startup(game->lua_state);

  mlog(INFO, "initialise_lua", "LUA state successfully initialised");

  return 0;
}
