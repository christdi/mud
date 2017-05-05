#ifndef _CONFIG_H_
#define _CONFIG_H_

typedef struct config config_t;

struct config {
    char * logConfigFile;
};

config_t * config_new();
const int config_load(const char * filename, config_t * config);
const int config_parse_line(char * line, config_t * config);
void config_free(config_t * config);

#endif
