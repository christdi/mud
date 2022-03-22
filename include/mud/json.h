#ifndef MUD_JSON_H
#define MUD_JSON_H

#include <stdint.h>
#include <stdlib.h>

/**
 * Typedef
**/
typedef struct json_value json_value_t;
typedef struct json_node json_node_t;
typedef union json_data json_data_t; 

/**
 * Structs
**/
typedef enum json_parse {
  AWAIT_VALUE_START, AWAIT_FIELD_START, AWAIT_FIELD_END, AWAIT_COLON, AWAIT_VALUE_END
} json_parse_t;

typedef enum json_type {
  OBJECT, ARRAY, STRING, NUMBER, BOOLEAN, NIL
} json_type_t;

typedef struct json_node {
  char* key;
  json_value_t* value;
  json_node_t* children;
  json_node_t* next;
  json_node_t* parent;
} json_node_t;

typedef struct json_value {
  json_type_t type;
  json_data_t* data;
  json_value_t* next;
} json_value_t;

typedef union json_data {
  char* str;
  uint32_t number;
  bool boolean;
  json_value_t* array;
} json_data_t;

/**
 * Function prototypes
**/
json_node_t* json_new_json_node_t();
void json_free_json_node_t(json_node_t* node);
json_node_t* json_parse(const char* input, size_t len);
void json_to_string(json_node_t* json, char* output);

#endif
