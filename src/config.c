#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mud/config.h"
#include "mud/string.h"

config * config_new() {
	config * config = calloc(1, sizeof * config);
	config->logConfigFile = string_copy("config.ini");

	return config;
}

const int config_load(const char * filename, config * config) {
	if ( !filename ) {
		return -1;
	}

	FILE * fp = fopen(filename, "r");

	if ( !fp ) {
		return -1;
	}

	char buffer[1024];

	while ( fgets(buffer, 1024, fp)) {
		if ( config_parse_line(buffer, config) != 0 ) {
			printf("Failed to parse configuration file line: '%s'.\n\r", buffer);

			continue;
		}
	}

	fclose(fp);

	return 0;
}

const int config_parse_line(char * line, config * config) {
	if ( !line || !config ) {
		return - 1;
	}

	char * key = strtok(line, "=");
	char * value = strtok(0, "\n");

	if ( !key || !value ) {
		return -1;
	}

	if ( strcmp(key, "log_config_file") == 0 ) {
		if ( config->logConfigFile ) {
			free(config->logConfigFile);
		}

		config->logConfigFile = string_copy(value);
	}
	
	return 0;
}

void config_free(config * config) {
	if ( config ) {
		if ( config->logConfigFile ) {
			free(config->logConfigFile);
		}

		free(config);
	}
}