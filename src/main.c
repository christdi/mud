#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mud/config.h"
#include "mud/game.h"
#include "mud/log.h"
#include "mud/util/mudstring.h"

#include "mud/json.h" // TODO(Chris I) Remove

static void print_object(json_value_t* object);
static void print_array(json_value_t* array);
static void print_value(json_value_t* value);

/**
 * Entry point for the application.  Will exit if unable to load to
 * configuration or initialise logging.  Otherwise, starts the game.
 **/
int main(int argc, char* argv[]) {
  //const char* json = "{ \"field\": \"hello\", \"blah\": \"meh\", \"hmm\": null, \"true\": true, \"false\": false, \"number\": 1234567890, \"array\": [\"string\", 12345, null, true, false, { \"test\": \"meh\", \"test2\": [123, 456, 789]}] }";
  const char* json = "{\"problems\":[{\"Diabetes\":[{\"medications\":[{\"medicationsClasses\":[{\"className\":[{\"associatedDrug\":[{\"name\":\"asprin\",\"dose\":\"\",\"strength\":\"500 mg\"}],\"associatedDrug#2\":[{\"name\":\"somethingElse\",\"dose\":\"\",\"strength\":\"500 mg\"}]}],\"className2\":[{\"associatedDrug\":[{\"name\":\"asprin\",\"dose\":\"\",\"strength\":\"500 mg\"}],\"associatedDrug#2\":[{\"name\":\"somethingElse\",\"dose\":\"\",\"strength\":\"500 mg\"}]}]}]}],\"labs\":[{\"missing_field\":\"missing_value\"}]}],\"Asthma\":[{}]}]}";

  LOG(INFO, "JSON input: [%s]", json);

  json_node_t* root = json_new_json_node_t();
  if (json_parse(json, strlen(json), root) == -1) {
    LOG(ERROR, "Failed to read JSON");

    return -1;
  }

  LOG(INFO, "JSON Output:");

  print_object(root->value);

  json_free_json_node_t(root);

  // if (start_game(argc, argv) != 0) {
  //   return -1;
  // }

  return 0;
}

static void print_object(json_value_t* object) {
  printf("{");

  for (json_node_t* children = object->data->children; children != NULL; children = children->next) {
    printf("\"%s\":", children->key);
    if (children->value->type == OBJECT) {
      print_object(children->value);
    } else if (children->value->type == ARRAY) {
      print_array(children->value);
    } else {
      print_value(children->value);
    }

    if(children->next != NULL) {
      printf(",");
    }
  }

  printf("}");
}

static void print_array(json_value_t* array) {
  size_t i = 0;

  printf("[");

  for (json_value_t* item = array->data->array; item != NULL; item = item->next) {
    if (item->type == OBJECT) {
      print_object(item);
    } else if (item->type == ARRAY) {
      print_array(item);
    } else {
      print_value(item);
    }

    if(item->next != NULL) {
      printf(",");
    }

    i++;
  }

  printf("]");
}

void print_value(json_value_t* value) {
  switch(value->type) {
    case STRING:
      printf("\"%s\"", value->data->str);
      break;
    case BOOLEAN:
      printf("%d", value->data->boolean);
      break;
    case NIL:
      printf("null");
      break;
    case NUMBER:
      printf("%d", value->data->number);
      break;
    default:
      break;
  }
}
