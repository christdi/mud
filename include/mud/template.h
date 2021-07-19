#ifndef MUD_TEMPLATE_H
#define MUD_TEMPLATE_H

/**
 * Definitions
**/
#define TEMPLATE_FILE_BUFFER_SIZE 1025
#define TEMPLATE_FILE_MAX_LINE_LENGTH TEMPLATE_FILE_BUFFER_SIZE - 1

/**
 * Typedefs
**/
typedef struct hash_table hash_table_t;

/**
 * Structs
**/
typedef struct template {
  char* key;
  char* value;
}
template_t;

/**
 * Function prototypes
**/
template_t* template_t_new();
void template_t_free(template_t* template);
void template_t_deallocate(void* value);

int template_load_from_file(hash_table_t* templates, const char* filename);

const char* tpl(hash_table_t* templates, const char* key);

#endif