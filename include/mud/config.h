#ifndef _CONFIG_H_
#define _CONFIG_H_

#define MAX_KEY_LENGTH 128
#define MAX_ARGUMENT_SIZE 128
#define MINIMUM_PORT 1024
#define DEFAULT_PORT 5000
#define DEFAULT_TICKS_PER_SECOND 20
#define MAX_CONFIG_LINE_LENGTH 1024
#define BASE_10 10

/**
 * Structs
**/
typedef struct config {
  char log_config_file[MAX_ARGUMENT_SIZE];
  char database_file[MAX_ARGUMENT_SIZE];
  unsigned int game_port;
  unsigned int ticks_per_second;
} config_t;

/**
 * Function prototypes
**/
config_t* config_new(void);
void config_free(config_t* config);

int parse_configuration(int argc, char* argv[], config_t* config);
int load_configuration(const char* filename, config_t* config);

#endif
