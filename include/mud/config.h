#ifndef _CONFIG_H_
#define _CONFIG_H_

/**
 * Structs
**/
typedef struct config {
  char * log_config_file;
  unsigned int ticks_per_second;
} config_t;


/**
 * Function prototypes
**/
config_t * config_new(void);
void config_free(config_t *config);

int load_configuration(const char *filename, config_t *config);

#endif
