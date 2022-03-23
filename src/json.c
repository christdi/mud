#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "mud/json.h"
#include "mud/log.h"

json_value_t* parse_object(const char* input, size_t len, size_t* pos);
json_value_t* parse_array(const char* input, size_t len, size_t* pos);
json_value_t* parse_string(const char* input, size_t len, size_t* pos);
json_value_t* parse_number(const char* input, size_t len, size_t* pos);
json_value_t* parse_boolean(const char* input, size_t len, size_t* pos);
json_value_t* parse_null(const char* input, size_t len, size_t* pos);

void attach_child(json_value_t* parent, json_node_t* child);
void attach_array(json_value_t* array, json_value_t* item);

json_node_t* json_new_json_node_t() {
  json_node_t* node = calloc(1, sizeof(json_node_t));

  node->key = NULL;
  node->value = NULL;

  return node;
}

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

json_value_t* json_new_json_value_t(json_type_t type) {
  assert(type != UNDEFINED);

  json_value_t* value = calloc(1, sizeof(json_value_t));
  value->data = calloc(1, sizeof(json_data_t));
  value->type = type;

  return value;
}

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

int json_parse(const char* input, size_t len, json_node_t* node) {
  assert(input);
  assert(node);

  size_t pos = 0;

  node->value = parse_object(input, len, &pos);

  if (node->value == NULL) {
    return -1;
  }

  return 0;
}

void json_to_string(json_node_t* json, char* output) {
}

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
      if (c != ' ' && c != '"') {
        LOG(ERROR, "Expected '\"' but encountered [%c] at position [%d]", c, i);

        break;
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
      if (c != ' ' && c != '{' && c != '[' && c != '"' && c != 't' && c != 'f' && c != 'n' && !isdigit(c)) {
        LOG(ERROR, "Expected \"{, [, \", true, false, null or number but encountered [%c] at position [%d]", c, i);

        break;
      }

      json_value_t* value = NULL;

      if (c == ' ') {
        continue;
      }

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
      if (c != ' ' && c != '{' && c != '[' && c != ']' && c != '"' && c != 't' && c != 'f' && c != 'n' && !isdigit(c)) {
        LOG(ERROR, "Expected \"{, [, ], \", true, false, null or number but encountered [%c] at position [%d]", c, i);

        break;
      }

      json_value_t* value = NULL;

      if (c == ' ') {
        continue;
      }

      if (c == ']') {
        return array;
      }

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
