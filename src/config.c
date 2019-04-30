#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mud/config.h"
#include "mud/mudstring.h"

config_t *config_new(void) {
  config_t *config = calloc(1, sizeof *config);
  config->logConfigFile = strdup("config.ini");
  config->ticksPerSecond = 20;

  return config;
}

int config_load(const char *filename, config_t *config) {
  assert(filename);

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

int config_parse_line(char *line, config_t *config) {
  if (!line || !config) {
    return -1;
  }

  char *key = strtok(line, "=");
  char *value = strtok(NULL, "\n");

  if (!key || !value) {
    return -1;
  }

  if (strcmp(key, "log_config_file") == 0) {
    if (config->logConfigFile) {
      free(config->logConfigFile);
    }

    config->logConfigFile = strdup(value);
  }

  if (strcmp(key, "ticks_per_second") == 0) {
    config->ticksPerSecond = atoi(value);
  }

  return 0;
}

void config_free(config_t *config) {
  assert(config);

  if (config->logConfigFile) {
    free(config->logConfigFile);
  }
}
