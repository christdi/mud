#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "mud/json.h"
#include "mud/log.h"

json_value_t* parse_object(const char** input, json_node_t* parent);
json_value_t* parse_array(const char** input);
json_value_t* parse_string(const char** input);
json_value_t* parse_number(const char** input);
json_value_t* parse_boolean(const char** input);
json_value_t* parse_null(const char** input);

json_node_t* json_new_json_node_t() {
  json_node_t* node = calloc(1, sizeof(json_node_t));

  return node;
}

void json_free_json_node_t(json_node_t* node) {
  assert(node);

  if (node->key != NULL) {
    free(node->key);  
  }

  free(node);
}

json_node_t* json_parse(const char* input, size_t len) {
  assert(input);

  json_node_t* parent = json_new_json_node_t();
  parent->value = parse_object(&input, parent);

  return parent;
}

void json_to_string(json_node_t* json, char* output) {
}

json_value_t* parse_object(const char** input, json_node_t* parent) {
  const char* current = *input;
  json_parse_t p = AWAIT_OBJECT_OPEN;
  json_node_t* node = NULL;

  while (*current++) {
    if (p == AWAIT_OBJECT_OPEN) {
      if ((current = strchr(current, '{')) == NULL) {
        LOG(ERROR, "Expected '{' but encountered EOF");

        return NULL;
      }

      p = AWAIT_KEY_OPEN;

      continue;
    }

    if (p == AWAIT_KEY_OPEN) {
      if ((current = strchr(current, '"')) == NULL) {
        LOG(ERROR, "Expected '\"' but encountered EOF");

        return NULL;
      }

      p = AWAIT_KEY_CLOSE;

      continue;
    }

    if (p == AWAIT_KEY_CLOSE) {
      node = json_new_json_node_t();

      const char* start = current;
      char* end;

      if ((end = strchr(current, '"')) == NULL) {
        LOG(ERROR, "Expected '\"' but encountered EOF");

        json_free_json_node_t(node);

        return NULL;
      }

      size_t len = 0;

      while (current++ != end) {
        len = len + 1;
      }

      node->key = calloc(1, len + 1);
      memcpy(node->key, start, len);
      node->key[len + 1] = '\0';

      current = end;

      p = AWAIT_KEY_COLON;

      continue;
    }

    if (p == AWAIT_KEY_COLON) {
      if ((current = strchr(current, ':')) == NULL) {
        LOG(ERROR, "Expected ':' but encountered EOF");

        return NULL;
      }

      p = AWAIT_VALUE_OPEN;

      continue;
    }

    if (p == AWAIT_VALUE_OPEN) {
      char c = tolower(*current);

      if (c != ' ' && c != '{' && c != '[' && c != '"' && c != 't' && c != 'f' && c != 'n' && !isdigit(c)) {
        LOG(ERROR, "Expected \"{, [, \", true, false, null or number but encountered [%c]", c);

        json_free_json_node_t(node);

        return NULL;
      }

      json_value_t* value;

      if (c == '{') {
        value = parse_object(&current, node);      
      }

      if (c == '[') {
        value = parse_array(&current);
      }

      if (c == '"') {
        value = parse_string(&current);
      }

      if (c == 't' || c == 'f') {
        value = parse_boolean(&current);
      }

      if (c == 'n') {
        value = parse_null(&current);
      }

      if (isdigit(c)) {
        value = parse_number(&current);
      }

      if (value == NULL) {
        json_free_json_node_t(node);

        return NULL;
      }
  
      node->value = value;
    }
  }

  return NULL;
}

json_value_t* parse_array(const char** input) {
  return NULL;
}

json_value_t* parse_string(const char** input) {
  return NULL;
}

json_value_t* parse_number(const char** input) {
  return NULL;
}

json_value_t* parse_boolean(const char** input) {
  return NULL;
}

json_value_t* parse_null(const char** input) {
  return NULL;
}
