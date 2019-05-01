#ifndef HG_CONFIG_H
#define HG_CONFIG_H

/**
 * Structs
**/
typedef struct config {
  char * logConfigFile;
  unsigned int ticksPerSecond;
} config_t;


/**
 * Function prototypes
**/
config_t * config_new(void);
void config_free(config_t *config);

int load_configuration(const char *filename, config_t *config);

#endif
