#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "mud/json.h"
#include "mud/log.h"

#define TRUE_STR "true"
#define FALSE_STR "false"
#define NULL_STR "null"

json_node_t* parse_value(const char* input, size_t len, size_t* pos);
json_node_t* parse_object(const char* input, size_t len, size_t* pos);
json_node_t* parse_array(const char* input, size_t len, size_t* pos);
json_node_t* parse_string(const char* input, size_t len, size_t* pos);
json_node_t* parse_number(const char* input, size_t len, size_t* pos);
json_node_t* parse_boolean(const char* input, size_t len, size_t* pos);
json_node_t* parse_null(const char* input, size_t len, size_t* pos);

int write_node_value(json_node_t* node, char* buffer, size_t len, size_t* pos);

static const json_type_str_t json_type_strs[] = {
  { OBJECT, "object"},
  { ARRAY, "array" },
  { STRING, "string" },
  { NUMBER, "number" },
  { BOOLEAN, "boolean" },
  { NIL, "null" },
  { UNDEFINED, "undefined"}
};

/**
 * Creates a new instance of json_node_t.
 *
 * Returns the new instance
**/
json_node_t* json_new_json_node_t(json_type_t type) {
  json_node_t* node = calloc(1, sizeof(json_node_t));
  
  node->type = type;
  node->key = NULL;
  node->next = NULL;

  node->value = calloc(1, sizeof(json_value_t));


  return node;
}

/**
 * Frees an instance of json_node_t.
 *
 * node - the json_node_t instance to be freed
**/
void json_free_json_node_t(json_node_t* node) {
  assert(node);

  if (node->type == OBJECT) {
    json_node_t* child = node->value->children;

    while(child != NULL) {
      json_node_t* next = child->next;

      json_free_json_node_t(child);

      child = next;
    }
  }

  if(node->type == ARRAY) {
    json_node_t* child = node->value->array;

    while (child != NULL) {
      json_node_t* next = child->next;

      json_free_json_node_t(child);

      child = next;
    }
  }

  if (node->type == STRING) {
    free(node->value->str);
  }

  if (node->value != NULL) {
    free(node->value);
  }

  if (node->key != NULL) {
    free(node->key);  
  }

  free(node);
}

/**
 * Deserializes a JSON string into a json_node_t.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * node - the node to be populated with parsed json from the string
 *
 * Returns populated json_node_t or NULL
**/
json_node_t* json_deserialize(const char* input, size_t len) {
  assert(input);

  size_t pos = 0;

  return parse_value(input, len, &pos);
}

/**
 * Creates a new JSON object node.
 *
 * Returns the JSON object node.
**/
json_node_t* json_new_object() {
  return json_new_json_node_t(OBJECT);
}

/**
 * Creates a new JSON array node.
 *
 * Returns the JSON array node.
**/
json_node_t* json_new_array() {
  return json_new_json_node_t(ARRAY);
}

/**
 * Creates a new JSON string node.
 *
 * str - the string value associated with the node
 * 
 * Returns the JSON string node.
**/
json_node_t* json_new_string(const char* str) {
  json_node_t* node = json_new_json_node_t(STRING);

  node->value->str = strdup(str);

  return node;
}


/**
 * Creates a new JSON number node.
 *
 * number - the number value associated with the node
 * 
 * Returns the JSON number node.
**/
json_node_t* json_new_number(double number) {
  json_node_t* node = json_new_json_node_t(NUMBER);

  node->value->number = number;

  return node;
}

/**
 * Creates a new JSON boolean node.
 *
 * boolean - the boolean value associated with the node
 * 
 * Returns the JSON boolean node.
**/
json_node_t* json_new_boolean(bool boolean) {
  json_node_t* node = json_new_json_node_t(BOOLEAN);

  node->value->boolean = boolean;

  return node;
}


/**
 * Creates a new JSON null node.
 *
 * Returns the JSON null node.
**/
json_node_t* json_new_null() {
  return json_new_json_node_t(NIL);
}

/**
 * Serializes a json_node_t instance into a JSON string.
 *
 * json - the node to serialize
 * output - a pointer to a character array sufficient to hold the JSON string
 * len - the length of the buffer
**/
int json_serialize(json_node_t* json, char* output, size_t len) {
  size_t pos = 0;

  if (write_node_value(json, output, len, &pos) == -1) {
    return -1;
  }

  output[pos] = '\0';

  return 0;
}

/**
 * Returns a string representation of a JSON type.
 *
 * type - the type to find a representation for
 *
 * Returns the representation
**/
const char* json_get_type_str(json_type_t type) {
  const json_type_str_t* type_str = &json_type_strs[0];

  while (type_str->type != UNDEFINED) {
    if (type_str->type == type) {
      return type_str->str;
    }

    type_str++;
  }

  return type_str->str;
}

/**
 * Returns the enum for a JSON type given the string representation.
 *
 * str - the type to find a representation for
 *
 * Returns the representation
**/
json_type_t json_get_str_type(const char* str) {
  const json_type_str_t* type_str = &json_type_strs[0];

  while (type_str->type != UNDEFINED) {
    if (strncmp(type_str->str, str, strlen(type_str->str)) == 0) {
      return type_str->type;
    }

    type_str++;
  }

  return type_str->type;
}

/**
 * Module internal method called to parse a JSON value.  This method will
 * delegate to another parse method based on the characters it encounters.
 *

 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_node_t or NULL if parsing failed.
**/
json_node_t* parse_value(const char* input, size_t len, size_t* pos) {
  size_t i = 0;

  for(; *pos < len; (*pos)++) {
    i = *pos;
    char c = input[i];

    if (c != ' ' && c != '{' && c != '[' && c != '"' && c != 't' && c != 'f' && c != 'n' && c != '-' && !isdigit(c)) {
      LOG(ERROR, "Expected \"{, [, \", true, false, null or number but encountered [%c] at position [%d]", c, i);

      break;
    }

    if (c == ' ') {
      continue;
    }

    json_node_t* node = NULL;

    if (c == '{') {
      node = parse_object(input, len, pos);
    } else if (c == '[') {
      node = parse_array(input, len, pos);
    } else if (c == '"') {
      node = parse_string(input, len, pos);
    } else if (c == 't' || c == 'f') {
      node = parse_boolean(input, len, pos);
    } else if (c == 'n') {
      node = parse_null(input, len, pos);
    } else if (isdigit(c) || c == '-') {
      node = parse_number(input, len, pos);
    }

    return node;
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
 * Returns an instance of json_node_t or NULL if parsing failed.
**/
json_node_t* parse_object(const char* input, size_t len, size_t* pos) {
  json_parse_t p = AWAIT_OBJECT_OPEN;
  json_node_t* obj = json_new_json_node_t(OBJECT);

  size_t key_start = 0;
  size_t key_len = 0;
  char* key_buffer = NULL;

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

        key_buffer = calloc(1, key_len + 1);
        memcpy(key_buffer, start, key_len);
        key_buffer[key_len + 1] = '\0';

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
      json_node_t* node = parse_value(input, len, pos);

      if (node == NULL) {
        break;
      }

      node->key = key_buffer;
      key_buffer = NULL;

      json_attach_child(obj, node);

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

  if (key_buffer != NULL) {
    free(key_buffer);  
  }

  json_free_json_node_t(obj);

  return NULL;
}

/**
 * Module internal method called to parse a JSON array
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_node_t or NULL if parsing failed.
**/
json_node_t* parse_array(const char* input, size_t len, size_t* pos) {
  assert(input);

  json_node_t* array = json_new_json_node_t(ARRAY);
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

      json_node_t* node = parse_value(input, len, pos);

      if (node == NULL) {
        break;
      }

      json_attach_array(array, node);

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

  json_free_json_node_t(array);

  return NULL;
}

/**
 * Module internal method called to parse a JSON string.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_node_t or NULL if parsing failed.
**/
json_node_t* parse_string(const char* input, size_t len, size_t* pos) {
  assert(input);

  json_node_t* node = json_new_json_node_t(STRING);
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
        node->value->str = calloc(1, str_len + 1);
        memcpy(node->value->str, input + str_start, str_len);
        node->value->str[str_len + 1] = '\0';
        *pos = i;

        return node;
      }

      str_len++;
    }
  }

  json_free_json_node_t(node);

  return NULL;
}

/**
 * Module internal method called to parse a JSON number.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_node_t or NULL if parsing failed.
**/
json_node_t* parse_number(const char* input, size_t len, size_t* pos) {
  assert(input);

  size_t i = 0;
  size_t num_start = *pos;
  bool seen_number = false;

  for(; *pos < len; (*pos)++) {
    i = *pos;
    char c = input[i];

    if (c == ' ' && !seen_number) {
      continue;
    }

    if (c == ' ' && seen_number) {
      break;
    }

    if (c == '-' || c == 'e' || c == 'E' || c == '+' || c == '.' || isdigit(c)) {
      seen_number = true;

      continue;
    }

    if (c == ',' || c == '}' || c == ']') {
      double value = strtod(input + num_start, NULL);

      if (value == 0.0 && strncmp(input + num_start, "0.0", 3) != 0) {
        break;
      }

      json_node_t* node = json_new_json_node_t(NUMBER);
      node->value->number = value;

      *pos = *pos - 1;

      return node;
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
 * Returns an instance of json_node_t or NULL if parsing failed.
**/
json_node_t* parse_boolean(const char* input, size_t len, size_t* pos) {
  assert(input);

  json_node_t* node = json_new_json_node_t(BOOLEAN);
  const char* current = input + *pos;

  if (strncmp(current, "true", 4) == 0) {
    node->value->boolean = true;

    *pos = *pos + 3;

    return node;
  }

  if (strncmp(current, "false", 5) == 0) {
    node->value->boolean = false;

    *pos = *pos + 4;

    return node;
  }

  LOG(ERROR, "Expected \"true or false\" but encountered [%c] at position [%i]", input[*pos], *pos);

  json_free_json_node_t(node);

  return NULL;
}

/**
 * Module internal method called to parse a JSON null.
 *
 * input - null terminated string containing the JSON to be parsed
 * len - length of the data to be parsed
 * pos - current position of the parser in the JSON string
 *
 * Returns an instance of json_node_t or NULL if parsing failed.
**/
json_node_t* parse_null(const char* input, size_t len, size_t* pos) {
  assert(input);

  json_node_t* node = json_new_json_node_t(NIL);
  const char* current = input + *pos;

  if ((strncmp(current, "null", 4) != 0)) {
    LOG(ERROR, "Expected \"null\" but encountered [%c] at position [%i]", input[*pos], *pos);

    json_free_json_node_t(node);

    return NULL;
  }

  *pos = *pos + 3;
  return node;
}

/**
 * Module internal method that writes the JSON data represented by node into a character
 * buffer.
 *
 * node - json_node_t instance with JSON data we want to write
 * buffer - preallocated character array we will write data to
 * len - the length of the buffer, effectively the maximum amount of characters we will write
 * pos - the current position in the buffer for writing
 *
 * Returns 0 on success or -1 on failure
**/
int write_node_value(json_node_t* node, char* buffer, size_t len, size_t* pos) {
  assert(node);
  assert(buffer);

  size_t val_len = 0;

  switch(node->type) {
    case STRING:  
      if ((val_len = snprintf(NULL, 0, "\"%s\"", node->value->str)) > len - *pos) {
        return -1;
      }

      sprintf(buffer + *pos, "\"%s\"", node->value->str);

      *pos += val_len;

      break;

    case NUMBER:
      if ((val_len = snprintf(NULL, 0, "%g", node->value->number)) > len - *pos) {
        return -1;
      }

      sprintf(buffer + *pos, "%g", node->value->number);

      *pos += val_len;
      
      break;

    case BOOLEAN:
      if (node->value->boolean == true) {
        if ((val_len = snprintf(NULL, 0, "%s", TRUE_STR)) > len - *pos) {
          return -1;
        }

        sprintf(buffer + *pos, "%s", TRUE_STR);

        *pos += val_len;

        break;
      }

      if (node->value->boolean == false) {
        if ((val_len = snprintf(NULL, 0, "%s", FALSE_STR)) > len - *pos) {
          return -1;
        }

        sprintf(buffer + *pos, "%s", FALSE_STR);

        *pos += val_len;

        break;
      }

      break;

    case NIL:
      if ((val_len = snprintf(NULL, 0, "%s", NULL_STR)) > len - *pos) {
        return -1;
      }

      sprintf(buffer + *pos, "%s", NULL_STR);

      *pos += val_len;
      
      break;

    case OBJECT:
      if ((val_len = snprintf(NULL, 0, "%s", "{ ")) > len - *pos) {
        return -1;
      }

      sprintf(buffer + *pos, "%s", "{ ");

      *pos += val_len;

      for (json_node_t* child = node->value->children; child != NULL; child = child->next) {
        if ((val_len = snprintf(NULL, 0, "\"%s\": ", child->key)) > len - *pos) {
          return -1;
        }

        sprintf(buffer + *pos, "\"%s\": ", child->key);

        *pos += val_len;

        if (write_node_value(child, buffer, len, pos) == -1) {
          return -1;
        }

        if (child->next != NULL) {
          if ((val_len = snprintf(NULL, 0, "%s", ", ")) > len - *pos) {
            return -1;
          }

          sprintf(buffer + *pos, "%s", ", ");

          *pos += val_len;
        }
      }

      if ((val_len = snprintf(NULL, 0, "%s", " }")) > len - *pos) {
        return -1;
      }

      sprintf(buffer + *pos, "%s", " }");

      *pos += val_len;

      break;

    case ARRAY:
      if ((val_len = snprintf(NULL, 0, "%s", "[ ")) > len - *pos) {
        return -1;
      }

      sprintf(buffer + *pos, "%s", "[ ");

      *pos += val_len;

      for (json_node_t* item = node->value->array; item != NULL; item = item->next) {
        if (write_node_value(item, buffer, len, pos) == -1) {
          return -1;
        }

        if (item->next != NULL) {
          if ((val_len = snprintf(NULL, 0, "%s", ", ")) > len - *pos) {
            return -1;
          }

          sprintf(buffer + *pos, "%s", ", ");

          *pos += val_len;
        }
      }

      if ((val_len = snprintf(NULL, 0, "%s", " ]")) > len - *pos) {
        return -1;
      }

      sprintf(buffer + *pos, "%s", " ]");

      *pos += val_len;

      break;

    default:
      return -1;

      break;
  }

  return 0;
}

/**
 * Attach a child node to a parent.
 *
 * parent - a json_node_t representing the parent node
 * child - a json_node_t to be attached to the parent
**/
void json_attach_child(json_node_t* parent, json_node_t* child) {
  assert(parent);
  assert(child);
  assert(parent->type == OBJECT);

  if (parent->value->children == NULL) {
    parent->value->children = child;

    return;
  }

  json_node_t* children = parent->value->children;

  while (children->next != NULL) {
    children = children->next;
  }

  children->next = child;
}

/**
 * Attach a node to an array.
 *
 * array a json_node_t representing the array to be attached to
 * item - a json_node_t to be added to the array.
**/
void json_attach_array(json_node_t* array, json_node_t* item) {
  assert(array);
  assert(item);
  assert(array->type == ARRAY);

  if (array->value->array == NULL) {
    array->value->array = item;

    return;
  }

  json_node_t* next = array->value->array;

  while(next->next != NULL) {
    next = next->next;
  }

  next->next = item;
}
