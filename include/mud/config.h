#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef struct config config;

struct config {
	char * logConfigFile;
};

config * config_new();
const int config_load(const char * filename, config * config);
const int config_parse_line(char * line, config * config);
void config_free(config * config);

#endif