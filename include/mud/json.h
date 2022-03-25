#ifndef MUD_JSON_H
#define MUD_JSON_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Typedef
**/
typedef union json_value json_value_t;
typedef struct json_node json_node_t;

/**
 * Enums
**/
typedef enum json_parse {
  AWAIT_OBJECT_OPEN, AWAIT_KEY_OPEN, AWAIT_KEY_CLOSE, AWAIT_KEY_COLON, AWAIT_VALUE_OPEN, AWAIT_VALUE_CLOSE, AWAIT_ARRAY_OPEN, AWAIT_ARRAY_VALUE, AWAIT_ARRAY_CLOSE
} json_parse_t;

typedef enum json_type {
  UNDEFINED, OBJECT, ARRAY, STRING, NUMBER, BOOLEAN, NIL
} json_type_t;

/**
 * Structs
**/
typedef struct json_node {
  json_type_t type;
  json_value_t* value;
  json_node_t* next;

  char* key;
} json_node_t;

typedef struct json_type_str {
  json_type_t type;
  char* str;
} json_type_str_t;

typedef union json_value {
  char* str;
  double number;
  bool boolean;
  json_node_t* array;
  json_node_t* children;
} json_value_t;

/**
 * Module methods
**/
json_node_t* json_new_json_node_t(json_type_t type);
void json_free_json_node_t(json_node_t* node);

json_node_t* json_deserialize(const char* input, size_t len);
int json_serialize(json_node_t* json, char* output, size_t len);

json_node_t* json_new_object();
json_node_t* json_new_array();
json_node_t* json_new_string(const char* str);
json_node_t* json_new_number(double number);
json_node_t* json_new_boolean(bool boolean);
json_node_t* json_new_null();

const char* json_get_type_str(json_type_t type);
json_type_t json_get_str_type(const char* str);

#endif
