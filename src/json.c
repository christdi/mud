#include <assert.h>
#include <ctype.h>

#include "mud/json.h"
#include "mud/log.h"

json_value_t* parse_value(const char* input, size_t len, size_t pos, json_parse_t p, json_node_t* parent);
void attach_node(json_node_t* parent, json_node_t* child);

json_node_t* json_new_json_node_t() {
  json_node_t* node = calloc(1, sizeof(json_node_t));

  return node;
}

void json_free_json_node_t(json_node_t* node) {
  assert(node);

  free(node);
}

json_node_t* json_parse(const char* input, size_t len) {
  assert(input);

  json_node_t* parent = json_new_json_node_t();
  parent->value = parse_value(input, len, 0, AWAIT_VALUE_START, parent);
  
  return parent;
}

void json_to_string(json_node_t* json, char* output) {
}


json_value_t* parse_value(const char* input, size_t len, size_t pos, json_parse_t p, json_node_t* parent) {
  assert(input);
  assert(len);
  
  json_parse_t p = AWAIT_VALUE_START;
  json_node_t* node = NULL;

  for (int i = pos; i < len; i++) {
    char c = input[i];
    char d = '_';

    switch(p) {
      case AWAIT_VALUE_START:
        c = tolower(c);

        if (c != ' ' && c != '{' && c != '[' && c != 'n' && c != 't' && c != 'f' && c != '"' && !isdigit(c)) {
          LOG(ERROR, "Failed to parse json value, expected space, left brace, left square bracket, null, true, false, quote or number");
          json_free_json_node_t(n);

          return NULL;
        }

        if (c == ' ') {
          continue;
        }

        if (c == '{') {          
          parse_value(input, len, i, AWAIT_FIELD_START, )

          n->type = OBJECT;
          p = AWAIT_FIELD_START;
        }

        if (c == '[') {
          n->type = ARRAY;
          p = AWAIT_FIELD_START;
        }

        if (c == 'n') {
          n->type = NIL;
          p = AWAIT_VALUE_END;
        }

        if (c == 't' || c == 'f') {
          n->type = BOOLEAN;
          p = AWAIT_VALUE_END;
        }

        if (c == '"') {
          n->type = STRING;
          p = AWAIT_VALUE_END;
        }

        if (isdigit(c)) {
          n->type = NUMBER;
          p = AWAIT_VALUE_END;
        }

        break;

      case AWAIT_FIELD_START:
        d = '_';

        if (n->type == ARRAY) {
          d = ']';
        }
      
        if (n->type == OBJECT) {
          d = '}';
        }

        if (c != ' ' && c != '"' && c != d) {
          LOG(ERROR, "Failed to parse json, expected ' ', '\"' or '%c'", d);;
          json_free_json_node_t(n);
  
          return NULL;
        }

        if (c == ' ') {
          continue;
        }

        if (c == '"') {
          p = AWAIT_FIELD_END;
        }

        if (c == d) {
          
        }

        break;

      case AWAIT_FIELD_END:
        if (c == '"') {
          p = AWAIT_COLON;
        }

        break;

      case AWAIT_COLON:
        if (c != ' ' && c != ':') {
          LOG(ERROR, "Failed to parse json, expected ' ' or ':'");
          json_free_json_node_t(n);
        }

        if (c == ' ') {
          continue;
        }

        if (c == ':') {
          p = AWAIT_VALUE_START;
        }

        break;

      case AWAIT_VALUE_END:        

        break;
    }
  }

  return n;
}

void attach_node(json_node_t* parent, json_node_t* child) {
  child->parent = parent;

  if (parent->child == NULL) {
    parent->child = child;
  }

  json_node_t* last_child = parent->child;

  while (parent->child->next != NULL) {
    last_child = parent->child->next;
  }

  last_child->next = child;
}