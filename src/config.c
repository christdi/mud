#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"

#include "bsd/string.h"

#include "mud/config.h"
#include "mud/util/mudstring.h"

int config_parse_line(char* line, config_t* config);

int set_log_config_file(const char* value, config_t* config);
int set_database_file(const char* value, config_t* config);
int set_game_port(const char* value, config_t* config);
int set_ticks_per_second(const char* value, config_t* config);

/**
 * Allocates a new config_t structure.
 *
 * Returns the allocated and default populated config_t.
**/
config_t* config_new(void) {
  config_t* config = calloc(1, sizeof *config);
  config->log_config_file = strdup("log.ini");
  config->database_file = strdup("mud.db");
  config->game_port = DEFAULT_PORT;
  config->ticks_per_second = DEFAULT_TICKS_PER_SECOND;

  return config;
}

/**
 * Frees a config_t struct.
**/
void config_free(config_t* config) {
  assert(config);

  if (config->log_config_file != NULL) {
    free(config->log_config_file);
  }

  if (config->database_file != NULL) {
    free(config->database_file);
  }

  free(config);
}

/**
 * Parses command line arguments into the configuration.
**/
int parse_configuration(int argc, char* argv[], config_t* config) {
  int opt = 0;

  while ((opt = getopt(argc, argv, ":d:p:t:h")) != -1) {
    switch (opt) {
    case 'd':
      if (set_database_file(optarg, config) == -1) {
        return -1;
      }

      break;

    case 'p':
      if (set_game_port(optarg, config) == -1) {
        return -1;
      }

      break;

    case 't':
      if (set_ticks_per_second(optarg, config) == -1) {
        return -1;
      }

      break;

    case 'h':
      printf("%s [-d database file] [-p port] [-t ticks per second]\n\r", argv[0]);

      return -1;

    case '?':
      printf("Unknown option [%c]\n\r", optopt);

      return -1;

    case ':':
      printf("Missing argument for option [%c]\n\r", optopt);

      return -1;
    }
  }

  return 0;
}

/**
 * Loads configuration from a Lua script.
 * 
 * Parameters:
 *   filename - The name of the Lua script to be evaluated
 *   config - The config struct to be populated
 *
 * Returns 0 on success or -1 on error.
**/
int load_configuration(const char* filename, config_t* config) {
  assert(filename);
  assert(config);

  lua_State *l = NULL;

  if ((l = luaL_newstate()) == NULL) {
    printf("Failed to create new Lua state to parse configuration");
    
    return -1;
  }

  if (luaL_dofile(l, filename) != 0) {
    printf("Error while loading Lua configuration [%s].\n\r", lua_tostring(l, -1));

    lua_close(l);

    return -1;
  }

  lua_getglobal(l, "game_port");

  if (lua_isstring(l, -1)) {
    set_game_port(lua_tostring(l, -1), config);
  }

  lua_pop(l, 1);

  lua_getglobal(l, "database_file");

  if (lua_isstring(l, -1)) {
    set_database_file(lua_tostring(l, -1), config);
  }

  lua_pop(l, 1);

  lua_getglobal(l, "log_config_file");

  if (lua_isstring(l, -1)) {
    set_log_config_file(lua_tostring(l, -1), config);
  }

  lua_pop(l, 1);

  lua_getglobal(l, "ticks_per_second");

  if (lua_isstring(l, -1)) {
    set_ticks_per_second(lua_tostring(l, -1), config);
  }

  lua_pop(l, 1);

  lua_close(l);

  return 0;
}

/**
 * Sets the filename of the log configuration file in the configuration.
 *
 * Returns 0 on success.
**/
int set_log_config_file(const char* value, config_t* config) {
  if (config->log_config_file != NULL) {
    free(config->log_config_file);
  }

  config->log_config_file = strdup(value);

  return 0;
}

/**
 * Sets the filename of the SQLite database in the configuration.
 *
 * Returns 0 on success.
**/
int set_database_file(const char* value, config_t* config) {
  if (config->database_file != NULL) {
    free(config->database_file);
  }

  config->database_file = strdup(value);

  return 0;
}

/**
 * Sets the game port in the configuration.
 *
 * Returns 0 on success.
 *
 * Returns -1 if the port isn't numeric or is greater than 1024.
**/
int set_game_port(const char* value, config_t* config) {
  if ((config->game_port = strtol(value, NULL, BASE_10)) < MINIMUM_PORT) {
    printf("Invalid value for port [%s], valid values are 1024 or higher.\n\r", value);

    return -1;
  }

  return 0;
}

/**
 * Sets the ticks per second in the configuration.
 *
 * Returns 0 on success.
 *
 * Returns -1 if the value isn't numeric or is equal to or less than 0.
**/
int set_ticks_per_second(const char* value, config_t* config) {
  if ((config->ticks_per_second = strtol(value, NULL, BASE_10)) == 0) {
    printf("Invalid value for ticks per second [%s], valid values are 1 or higher.\n\r", value);

    return -1;
  }

  return 0;
}
