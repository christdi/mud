#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

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
  strlcpy(config->log_config_file, "log.ini", MAX_ARGUMENT_SIZE);
  strlcpy(config->database_file, "mud.db", MAX_ARGUMENT_SIZE);
  config->game_port = DEFAULT_PORT;
  config->ticks_per_second = DEFAULT_TICKS_PER_SECOND;


  return config;
}

/**
 * Frees a config_t struct.
**/
void config_free(config_t* config) {
  assert(config);

  free(config);
}


/**
 * Parses command line arguments into the configuration.
**/
int parse_configuration(int argc, char *argv[], config_t* config) {
  int opt = 0;

  while ((opt = getopt(argc, argv, ":d:p:t:h")) != -1) {
    switch(opt) {
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
 * Attempts to read the file specified by the filename parameter and parse the 
 * contents as key value pairs to populate the config_t struct represented by
 * the config parameter.
 *
 * Returns 0 on success or -1 on error.
**/
int load_configuration(const char* filename, config_t* config) {
  assert(filename);
  assert(config);

  FILE* fp = fopen(filename, "re");

  if (!fp) {
    return -1;
  }

  char buffer[MAX_CONFIG_LINE_LENGTH];

  while (fgets(buffer, MAX_CONFIG_LINE_LENGTH, fp)) {
    if (config_parse_line(buffer, config) != 0) {
      printf("Failed to parse configuration file line: '%s'.\n\r", buffer);

      continue;
    }
  }

  fclose(fp);

  return 0;
}

/**
 * Parses a single line of configuration with the expected format of key=value.
 * If a key is recognised, the associated field is populated in the config_t
 * represented by the config parameter.
 *
 * Returns 0 on success or -1 on failure.
**/
int config_parse_line(char* line, config_t* config) {
  assert(line);
  assert(config);

  char* key = strtok(line, "=");
  char* value = strtok(NULL, "\n");

  if (!key || !value) {
    return -1;
  }

  if (strncmp(key, "log_config_file", MAX_KEY_LENGTH) == 0) {
    set_log_config_file(value, config);
  }

  if (strncmp(key, "database_file", MAX_KEY_LENGTH) == 0) {
    set_database_file(value, config);
  }

  if (strncmp(key, "game_port", MAX_KEY_LENGTH) == 0) {
    set_game_port(value, config);
  }

  if (strncmp(key, "ticks_per_second", MAX_KEY_LENGTH) == 0) {
    set_ticks_per_second(value, config);
  }

  return 0;
}

/**
 * Sets the filename of the log configuration file in the configuration.
 *
 * Returns 0 on success.
 *
 * Returns -1 if the length of the file path exceeds MAX_ARGUMENT_SIZE.
**/
int set_log_config_file(const char* value, config_t* config) {
  if (strlcpy(config->log_config_file, value, MAX_ARGUMENT_SIZE) > MAX_ARGUMENT_SIZE) {
    printf("Invalid value for log config file [%s], longer than max argument size [%d]", value, MAX_ARGUMENT_SIZE);

    return -1;
  }

  return 0;
}

/**
 * Sets the filename of the SQLite database in the configuration.
 *
 * Returns 0 on success.
 *
 * Returns -1 if the length of the file path exceeds MAX_ARGUMENT_SIZE.
**/
int set_database_file(const char* value, config_t* config) {
  if (strlcpy(config->database_file, value, MAX_ARGUMENT_SIZE) > MAX_ARGUMENT_SIZE) {
    printf("Invalid value for database file [%s], longer than max argument size [%d]", value, MAX_ARGUMENT_SIZE);

    return -1;
  }

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
