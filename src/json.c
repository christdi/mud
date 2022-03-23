#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "mud/json.h"
#include "mud/log.h"

json_value_t* parse_value(const char* input, size_t len, size_t* pos);
json_value_t* parse_object(const char* input, size_t len, size_t* pos);
json_value_t* parse_array(const char* input, size_t len, size_t* pos);
json_value_t* parse_string(const char* input, size_t len, size_t* pos);
json_value_t* parse_number(const char* input, size_t len, size_t* pos);
json_value_t* parse_boolean(const char* input, size_t len, size_t* pos);
json_value_t* parse_null(const char* input, size_t len, size_t* pos);

void object_to_string(json_node_t* node, char* buffer);
void array_to_string(json_node_t* node, char* buffer);
void string_to_string(json_node_t* node, char* buffer);
void number_to_string(json_node_t* node, char* buffer);
void boolean_to_string(json_node_t* node, char* buffer);
void null_to_string(json_node_t* node, char* buffer);

void attach_child(json_value_t* parent, json_node_t* child);
void attach_array(json_value_t* array, json_value_t* item);

/**
 * Creates a new instance of json_node_t.
 *
 * Returns the new instance
**/
json_node_t* json_new_json_node_t() {
  json_node_t* node = calloc(1, sizeof(json_node_t));

  node->key = NULL;
  node->value = NULL;

  return node;
}

/**
 * Frees an instance of json_node_t.
 *
 * node - the json_node_t instance to be freed
**/
void json_free_json_node_t(json_node_t* node) {
  assert(node);

  if (node->key != NULL) {
    free(node->key);  
  }

  if (node->value != NULL) {
    json_free_json_value_t(node->value);
  }

  free(node);
}

/**
 * Creates a new instance of json_value_t.  This also allocates memory for the data field as
 * the json_data_t union type should never exist independently of a json_value_t.
 *
 * type - the type of the json value
 *
 * Returns the new instance
**/
json_value_t* json_new_json_value_t(json_type_t type) {
  assert(type != UNDEFINED);

  json_value_t* value = calloc(1, sizeof(json_value_t));
  value->data = calloc(1, sizeof(json_data_t));
  value->type = type;

  return value;
}

/**
 * Frees an allocated instance of json_value_t.  If there is children, it'll recursively
 * free all of them.  If there are siblings, it will recursivel free all of them.
 *
 * value - the json_value_t instance to be freed.
**/
void json_free_json_value_t(json_value_t* value) {
  assert(value);

  if (value->type == OBJECT) {
    json_node_t* child = value->data->children;

    while(child != NULL) {
      json_node_t* next = child->next;

      json_free_json_node_t(child);

      child = next;
    }
  }

  if(value->type == ARRAY) {
    json_value_t* child = value->data->array;

    while (child != NULL) {
      json_value_t* next = child->next;

      json_free_json_value_t(child);

      child = next;
    }
  }

  if (value->type == STRING) {
    free(value->data->str);
  }

  free(value->data);
  free(value);
}

/**
 * Parses a string of JSON into a json_node_t.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * node - the node to be populated with parsed json from the string
 *
 * Returns 0 on success or -1 on failure.
**/
int json_parse(const char* input, size_t len, json_node_t* node) {
  assert(input);
  assert(node);

  size_t pos = 0;

  node->value = parse_value(input, len, &pos);

  if (node->value == NULL) {
    return -1;
  }

  return 0;
}

/**
 * Steps through a json_node_t instance and constructs a JSON string from it.
 *
 * json - the node to step through
 * output - a pointer to a character array sufficient to hold the JSON string
**/
void json_to_string(json_node_t* json, char* output) {
}

/**
 * Module internal method called to parse a JSON value.  This method will
 * delegate to another parse method based on the characters it encounters.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_value_t or NULL if parsing failed.
**/
json_value_t* parse_value(const char* input, size_t len, size_t* pos) {
  size_t i = 0;

  for(; *pos < len; (*pos)++) {
    i = *pos;
    char c = input[i];

    if (c != ' ' && c != '{' && c != '[' && c != '"' && c != 't' && c != 'f' && c != 'n' && !isdigit(c)) {
      LOG(ERROR, "Expected \"{, [, \", true, false, null or number but encountered [%c] at position [%d]", c, i);

      break;
    }

    if (c == ' ') {
      continue;
    }

    json_value_t* value = NULL;

    if (c == '{') {
      value = parse_object(input, len, pos);
    } else if (c == '[') {
      value = parse_array(input, len, pos);
    } else if (c == '"') {
      value = parse_string(input, len, pos);
    } else if (c == 't' || c == 'f') {
      value = parse_boolean(input, len, pos);
    } else if (c == 'n') {
      value = parse_null(input, len, pos);
    } else if (isdigit(c)) {
      value = parse_number(input, len, pos);
    }

    return value;
  }

  LOG(ERROR, "Reached EOF before object was closed");

  return NULL;
}

/**
 * Module internal method called to parse a JSON object.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_value_t or NULL if parsing failed.
**/
json_value_t* parse_object(const char* input, size_t len, size_t* pos) {
  json_parse_t p = AWAIT_OBJECT_OPEN;
  json_value_t* obj = json_new_json_value_t(OBJECT);
  json_node_t* node = NULL;

  size_t key_start = 0;
  size_t key_len = 0;
  size_t i = 0;

  for(; *pos < len; (*pos)++) {
    i = *pos;
    char c = input[i];

    if (p == AWAIT_OBJECT_OPEN) {
      if (c != ' ' && c != '{') {
        LOG(ERROR, "Expected \"{\" but encountered [%c] at position [%d]", c, i);

        break;
      }

      if (c == '{') {
        p = AWAIT_KEY_OPEN;
      }

      continue;
    }

    if (p == AWAIT_KEY_OPEN) {
      if (c != ' ' && c != '"' && c != '}') {
        LOG(ERROR, "Expected '\"' but encountered [%c] at position [%d]", c, i);

        break;
      }

      if (c == '}') {
        return obj;
      }

      if (c == '"') {
        p = AWAIT_KEY_CLOSE;
        key_start = i + 1;
      }

      continue;
    }

    if (p == AWAIT_KEY_CLOSE) {
      if (c == '"') {
        const char* start = input + key_start;

        node = json_new_json_node_t();
        node->key = calloc(1, key_len + 1);
        memcpy(node->key, start, key_len);
        node->key[key_len + 1] = '\0';

        key_start = 0;
        key_len = 0;

        p = AWAIT_KEY_COLON;

        continue;
      }

      key_len++;

      continue;
    }

    if (p == AWAIT_KEY_COLON) {
      if (c != ' ' && c != ':') {
        LOG(ERROR, "Expected \":\" but encountered [%c] at position [%d]", c, i);

        break;
      }

      if (c == ':') {
        p = AWAIT_VALUE_OPEN;
      }

      continue;
    }

    if (p == AWAIT_VALUE_OPEN) {
      json_value_t* value = parse_value(input, len, pos);

      if (value == NULL) {
        break;
      }

      node->value = value;
      attach_child(obj, node);

      p = AWAIT_VALUE_CLOSE;

      continue;
    }

    if (p == AWAIT_VALUE_CLOSE) {
        if (c != ' ' && c != '}' && c!= ',') {
          LOG(ERROR, "Expected \"} or ,\" but encountered [%c] at position [%d]", c, i);

          break;
        }

        if (c == ',') {
          p = AWAIT_KEY_OPEN;

          continue;
        }

        if (c == '}') {
          return obj;
        }

        continue;
    }
  }

  LOG(ERROR, "Reached EOF before object was closed");

  json_free_json_value_t(obj);

  if (node != NULL) {
    json_free_json_node_t(node);
  }

  return NULL;
}

/**
 * Module internal method called to parse a JSON array
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_value_t or NULL if parsing failed.
**/
json_value_t* parse_array(const char* input, size_t len, size_t* pos) {
  assert(input);

  json_value_t* array = json_new_json_value_t(ARRAY);
  json_parse_t p = AWAIT_ARRAY_OPEN;

  size_t i = 0;

  for(; *pos < len; (*pos)++) {
    i = *pos;
    char c = input[i];

    if (p == AWAIT_ARRAY_OPEN) {
      if (c != ' ' && c != '[') {
        LOG(ERROR, "Expected '[' but encountered [%c] at position [%d]", c, i);

        break;
      }

      if (c == '[') {
        p = AWAIT_ARRAY_VALUE;

        continue;
      }
    }

    if (p == AWAIT_ARRAY_VALUE) {
      if (c == ' ') {
        continue;
      }

      if (c == ']') {
        return array;
      }

      json_value_t* value = parse_value(input, len, pos);

      if (value == NULL) {
        break;
      }

      attach_array(array, value);

      p = AWAIT_ARRAY_CLOSE;

      continue;
    }

    if (p == AWAIT_ARRAY_CLOSE) {
      if (c != ' ' && c != ',' && c != ']') {
        LOG(ERROR, "Expected \"',' or ']' but encountered [%c] at position [%d]", c, i);

        break;
      }

      if (c == ' ') {
        continue;
      }

      if (c == ',') {
        p = AWAIT_ARRAY_VALUE;

        continue;
      }

      if (c == ']') {
        return array;
      }
    }
  }

  json_free_json_value_t(array);

  return NULL;
}

/**
 * Module internal method called to parse a JSON string.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_value_t or NULL if parsing failed.
**/
json_value_t* parse_string(const char* input, size_t len, size_t* pos) {
  assert(input);

  json_value_t* value = json_new_json_value_t(STRING);
  json_parse_t p = AWAIT_VALUE_OPEN;
  size_t str_start = 0;
  size_t str_len = 0;
  size_t i = 0;

  for(; *pos < len; (*pos)++) {
    i = *pos;
    char c = input[i];

    if (p == AWAIT_VALUE_OPEN) {
      if (c != ' ' && c != '"') {
        LOG(ERROR, "Expected '\"' but encountered [%c] at position [%d]", c, i);

        break;
      }

      if (c == '"') {
        p = AWAIT_VALUE_CLOSE;
        str_start = i + 1;

        continue;
      }
    }

    if (p == AWAIT_VALUE_CLOSE) {
      if (c == '"') {
        value->data->str = calloc(1, str_len + 1);
        memcpy(value->data->str, input + str_start, str_len);
        value->data->str[str_len + 1] = '\0';
        *pos = i;

        return value;
      }

      str_len++;
    }
  }

  json_free_json_value_t(value);
  LOG(ERROR, "Encountered EOF while parsing JSON string");

  return NULL;
}

/**
 * Module internal method called to parse a JSON number.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_value_t or NULL if parsing failed.
**/
json_value_t* parse_number(const char* input, size_t len, size_t* pos) {
  assert(input);

  size_t i = 0;
  size_t num_start = *pos;
  size_t num_len = 0;

  bool seen_number = false;
  bool read_number = false;

  for(; *pos < len; (*pos)++) {
    i = *pos;
    char c = input[i];

    if (isdigit(c) && !read_number) {
      seen_number = true;

      num_len++;

      continue;
    }

    if (c == ' ' && seen_number) {
      read_number = true;

      continue;
    }

    if (c == ',' || c == '}' || c == ']') {
      char tmp[num_len + 1];
      memcpy(tmp, input + num_start, num_len);

      json_value_t* value = json_new_json_value_t(NUMBER);
      value->data->number = strtoul(tmp, NULL, 10);

      *pos = *pos - 1;

      return value;
    }

    break;
  }

  LOG(ERROR, "Expected \"number, ',', '}' or ']'\" but encountered [%c] at position [%d]", input[i], i);

  return NULL;
}

/**
 * Module internal method called to parse a JSON boolean.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_value_t or NULL if parsing failed.
**/
json_value_t* parse_boolean(const char* input, size_t len, size_t* pos) {
  assert(input);

  json_value_t* value = json_new_json_value_t(BOOLEAN);
  const char* current = input + *pos;

  if (strncmp(current, "true", 4) == 0) {
    value->data->boolean = true;

    *pos = *pos + 3;

    return value;
  }

  if (strncmp(current, "false", 5) == 0) {
    value->data->boolean = false;

    *pos = *pos + 4;

    return value;
  }

  LOG(ERROR, "Expected \"true or false\" but encountered [%c] at position [%i]", input[*pos], *pos);

  json_free_json_value_t(value);

  return NULL;
}

/**
 * Module internal method called to parse a JSON null.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_value_t or NULL if parsing failed.
**/
json_value_t* parse_null(const char* input, size_t len, size_t* pos) {
  assert(input);

  json_value_t* value = json_new_json_value_t(NIL);
  const char* current = input + *pos;

  if ((strncmp(current, "null", 4) != 0)) {
    LOG(ERROR, "Expected \"null\" but encountered [%c] at position [%i]", input[*pos], *pos);

    json_free_json_value_t(value);

    return NULL;
  }

  *pos = *pos + 3;
  return value;
}

/**
 * Module internal method to attach a child node to a parent.
 *
 * parent - a json_value_t representing the parent node
 * child - a json_node_t to be attached to the parent
**/
void attach_child(json_value_t* parent, json_node_t* child) {
  assert(parent);
  assert(child);
  assert(parent->type == OBJECT);

  json_data_t* data = parent->data;

  if (data->children == NULL) {
    data->children = child;

    return;
  }

  json_node_t* children = data->children;

  while (children->next != NULL) {
    children = children->next;
  }

  children->next = child;
}

/**
 * Module internal method to attach a value to an array.
 *
 * array a json_value_t representing the array to be attached to
 * item - a json_value_t to be added to the array.
**/
void attach_array(json_value_t* array, json_value_t* item) {
  assert(array);
  assert(item);
  assert(array->type == ARRAY);

  if (array->data->array == NULL) {
    array->data->array = item;

    return;
  }

  json_value_t* next = array->data->array;

  while(next->next != NULL) {
    next = next->next;
  }

  next->next = item;
}
