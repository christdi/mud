#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mud/config.h"
#include "mud/util/mudstring.h"

int config_parse_line(char *line, config_t *config);

/**
 * Allocates a new config_t structure.
 *
 * Returns the allocated and default populated config_t.
**/
config_t * config_new(void) {
  config_t * config = calloc(1, sizeof *config);
  config->log_config_file = strdup("log.ini");
  config->ticks_per_second = 20;

  return config;
}

/**
 * Frees a config_t struct.
**/
void config_free(config_t * config) {
  assert(config);

  if (config->log_config_file) {
    free(config->log_config_file);
  }
}


/**
 * Attempts to read the file specified by the filename parameter and parse the 
 * contents as key value pairs to populate the config_t struct represented by
 * the config parameter.
 *
 * Returns 0 on success or -1 on error.
**/
int load_configuration(const char *filename, config_t *config) {
  assert(filename);
  assert(config);

  FILE *fp = fopen(filename, "r");

  if (!fp) {
    return -1;
  }

  char buffer[1024];

  while (fgets(buffer, 1024, fp)) {
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
int config_parse_line(char * line, config_t * config) {
  assert(line);
  assert(config);

  char * key = strtok(line, "=");
  char * value = strtok(NULL, "\n");

  if (!key || !value) {
    return -1;
  }

  if (strcmp(key, "log_config_file") == 0) {
    if (config->log_config_file) {
      free(config->log_config_file);
    }

    config->log_config_file = strdup(value);
  }

  if (strcmp(key, "ticks_per_second") == 0) {
    config->ticks_per_second = atoi(value);
  }

  return 0;
}


