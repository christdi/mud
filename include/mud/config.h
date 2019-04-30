#ifndef HG_CONFIG_H
#define HG_CONFIG_H

typedef struct config config_t;

struct config {
  char *logConfigFile;
  unsigned int ticksPerSecond;
};

config_t *config_new(void);
int config_load(const char *filename, config_t *config);
int config_parse_line(char *line, config_t *config);
void config_free(config_t *config);

#endif
