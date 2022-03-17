#include <assert.h>
#include <stdlib.h>

#include "lauxlib.h"
#include "lua.h"
#include "lualib.h"

#include "mud/command/command.h"
#include "mud/config.h"
#include "mud/data/hash_table.h"
#include "mud/data/linked_list.h"
#include "mud/ecs/ecs.h"
#include "mud/event/event.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/lua/command_api.h"
#include "mud/lua/common.h"
#include "mud/lua/db_api.h"
#include "mud/lua/game_api.h"
#include "mud/lua/hooks.h"
#include "mud/lua/log_api.h"
#include "mud/lua/player_api.h"
#include "mud/lua/script.h"
#include "mud/lua/script_api.h"
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

  game->config = config_new();

  game->database = NULL;

  game->templates = create_hash_table_t();
  game->templates->deallocator = template_t_deallocate;

  game->players = create_hash_table_t();
  game->players->deallocator = deallocate_player;

  game->entities = create_hash_table_t();
  game->entities->deallocator = ecs_deallocate_entity;

  game->event_broker = event_new_event_broker_t();

  game->components = create_linked_list_t();
  game->components->deallocator = ecs_deallocate_component_t;

  game->archetypes = create_linked_list_t();
  game->archetypes->deallocator = ecs_deallocate_archetype_t;

  game->tasks = create_linked_list_t();
  game->tasks->deallocator = deallocate_task_t;

  game->events = create_linked_list_t();

  game->network = create_network_t();

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

  config_free(game->config);

  free_hash_table_t(game->templates);
  free_hash_table_t(game->players);
  free_hash_table_t(game->entities);

  event_free_event_broker_t(game->event_broker);

  free_linked_list_t(game->components);
  free_linked_list_t(game->archetypes);
  free_linked_list_t(game->tasks);
  free_linked_list_t(game->events);

  free_network_t(game->network);

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
int start_game(int argc, char* argv[]) {
  game_t* game = create_game_t();

  LOG(INFO, "Starting MUD engine");

  if (load_configuration("config.lua", game->config) != 0) {
    printf("Unable to load [config.lua].  Using default configuration\n\r");
  }

  if (parse_configuration(argc, argv, game->config) != 0) {
    exit(-1);
  }

  register_connection_callback(game->network, player_connected, game);
  register_disconnection_callback(game->network, player_disconnected, game);
  register_input_callback(game->network, player_input, game);
  register_flush_callback(game->network, player_output, game);

  if (template_load_from_file(game->templates, "template.properties") != 0) {
    LOG(ERROR, "Failed to load templates");

    return -1;
  }

  if (connect_to_database(game, game->config->database_file) != 0) {
    LOG(ERROR, "Failed to start game server");

    return -1;
  }

  if (initialise_lua(game, game->config) == -1) {
    LOG(ERROR, "Failed to initialise Lua");

    return -1;
  }

  if (ecs_load_entities(game) == -1) {
    LOG(ERROR, "Failed to load entities");

    return -1;
  }

  if (start_game_server(game->network, game->config->game_port) == -1) {
    LOG(ERROR, "Failed to start game server");

    return -1;
  }

  task_schedule(game->tasks, GAME_PLAYER_PULSE_SECONDS, game_pulse_players);

  while (!game->shutdown) {
    poll_network(game->network);
    event_dispatch_events(game->event_broker, game, game->entities, game->players);
    task_execute(game->tasks, game);
    ecs_update_systems(game);
    game_sleep_until_tick(game, game->config->ticks_per_second);
    flush_output(game->network);
  }

  if (stop_game_server(game->network, game->config->game_port) == -1) {
    LOG(ERROR, "Failed to shutdown server");

    return -1;
  }

  disconnect_clients(game->network);

  sqlite3_close(game->database);

  LOG(INFO, "Stopping MUD engine");

  free_game_t(game);

  return 0;
}

/**
 * Attempts to connect to the SQLite3 database with a given filename.  Returns -1 on failure
 * or 0 on success.
 **/
int connect_to_database(game_t* game, const char* filename) {
  LOG(INFO, "Connecting to database [%s]", filename);

  if (sqlite3_open(filename, &game->database) != SQLITE_OK) {
    LOG(ERROR, "Failed to open game database [%s]", sqlite3_errmsg(game->database));

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
    LOG(ERROR, "Failed to initialise Lua state");
    return -1;
  }

  if (lua_common_initialise_state(game->lua_state, game) == -1) {
    LOG(ERROR, "Failed to initialise Lua state");
    return -1;
  }

  luaL_openlibs(game->lua_state);

  if (lua_game_register_api(game->lua_state) == -1) {
    LOG(ERROR, "Failed to register Lua API with state");
    return -1;
  }

  if (lua_db_register_api(game->lua_state) == -1) {
    LOG(ERROR, "Failed to register Lua DB API with state");
    return -1;
  }

  if (lua_player_register_api(game->lua_state) == -1) {
    LOG(ERROR, "Failed to register Lua player API with state");
    return -1;
  }

  if (lua_log_register_api(game->lua_state) == -1) {
    LOG(ERROR, "Failed to register Lua log API with state");
    return -1;
  }

  if (lua_script_register_api(game->lua_state) == -1) {
    LOG(ERROR, "Failed to register Lua script API with state");
    return -1;
  }

  if (lua_command_register_api(game->lua_state) == -1) {
    LOG(ERROR, "Failed to register Lua script API with state");
    return -1;
  }

  if (game->config->lua_common_script != NULL) {
    if (luaL_dofile(game->lua_state, game->config->lua_common_script) != 0) {
      LOG(ERROR, "Error while loading Lua common script [%s].\n\r", lua_tostring(game->lua_state, -1));

      return -1;
    }
  }

  if (luaL_dofile(game->lua_state, config->game_script_file) != 0) {
    LOG(ERROR, "Error while loading Lua main script [%s].\n\r", lua_tostring(game->lua_state, -1));

    return -1;
  }

  if (lua_hook_on_startup(game->lua_state) != 0) {
    return -1;
  }

  LOG(INFO, "LUA state successfully initialised");

  return 0;
}
