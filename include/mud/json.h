#ifndef MUD_JSON_H
#define MUD_JSON_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Typedef
**/
typedef struct json_value json_value_t;
typedef struct json_node json_node_t;
typedef union json_data json_data_t; 

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
  char* key;
  json_value_t* value;
  json_node_t* next;
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
  json_node_t* children;
} json_data_t;

/**
 * Module methods
**/
json_node_t* json_new_json_node_t();
void json_free_json_node_t(json_node_t* node);

json_value_t* json_new_json_value_t(json_type_t type);
void json_free_json_value_t(json_value_t* value);

int json_parse(const char* input, size_t len, json_node_t* node);
void json_to_string(json_node_t* json, char* output);

#endif
