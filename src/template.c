#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsd/string.h"
#include "mud/data/hash_table.h"
#include "mud/log.h"
#include "mud/template.h"
#include "mud/util/mudstring.h"

int template_parse_buffer(char* buffer, hash_table_t* templates, size_t* position);

/**
 * Allocates a new instance of template_t.
 *
 * Returns the newly allocated instance of template_t.
 **/
template_t* template_t_new() {
  template_t* template = calloc(1, sizeof *template);

  return template;
}

/**
 * Frees an allocated instance of template_t.
 *
 * Parameters
 *  template - the template to be freed
 **/
void template_t_free(template_t* template) {
  assert(template);

  if (template->key != NULL) {
    free(template->key);
  }

  if (template->value != NULL) {
    free(template->value);
  }

  free(template);
}

/**
 * Deallocator for use with template_t in hash_table.
 *
 * Parameters
 *  value - should be a template_t*, behaviour is undefined if not
 **/
void template_t_deallocate(void* value) {
  assert(value);

  template_t_free((template_t*)value);
}

/**
 * Load key/value templates from supplied filename into templates.
 *
 * Parameters
 *  templates - the hash table templates should be inserted into*
 *  filename - the filename to load the templates from
 *
 * Returns 0 on success or -1 on failure.
 **/
int template_load_from_file(hash_table_t* templates, const char* filename) {
  assert(templates);
  assert(filename);

  LOG(INFO, "Loading templates from [%s]", filename);

  FILE* fp = fopen(filename, "re");

  if (!fp) {
    return -1;
  }

  char buffer[TEMPLATE_FILE_BUFFER_SIZE];
  size_t position = 0;

  while (fgets(buffer + position, TEMPLATE_FILE_BUFFER_SIZE - position, fp) != NULL) {
    if (strnlen(buffer, TEMPLATE_FILE_MAX_LINE_LENGTH) == TEMPLATE_FILE_MAX_LINE_LENGTH) {
      if (buffer[TEMPLATE_FILE_MAX_LINE_LENGTH - 1] != '\n') {
        LOG(ERROR, "Template line was too long [%s]", buffer);

        return -1;
      }
    }

    if (template_parse_buffer(buffer, templates, &position) != 0) {
      LOG(ERROR, "Unable to parse template file line: [%s]", buffer);

      continue;
    }
  }

  fclose(fp);

  return 0;
}

/**
 * Parses a buffer of a template configuration file which has a simple key=value structure.
 *
 * Parameters
 *  buffer - the line currently being parsed
 *  templates - the hash table templates should be stored in
 **/
int template_parse_buffer(char* buffer, hash_table_t* templates, size_t* position) {
  assert(buffer);
  assert(templates);
  assert(position);

  size_t len = strnlen(buffer, TEMPLATE_FILE_MAX_LINE_LENGTH);
  char* c = strrchr(buffer, '"');

  if (c != NULL && c != buffer) {
    c--;

    if (*c != '=' && *c != '\\') {
      char* key = strtok(buffer, "=");
      char* value = strtok(NULL, "\"");

      if (!key || !value) {
        return -1;
      }

      template_t* template = template_t_new();
      template->key = strdup(key);
      template->value = replace_r(replace(value, "\\r", "\r"), "\\n", "\n");

      hash_table_insert(templates, template->key, template);

      *position = 0;

      return 0;
    }
  }

  *position = len;

  return 0;
}

/**
 * Retrieve the template for a given key.
 *
 * Parameters
 *  templates - hash table containing templates
 *  key - the key to search for in the hash table
 *
 * Returns the value of the template or "undefined" literal if not found.
 **/
const char* tpl(hash_table_t* templates, const char* key) {
  assert(templates);
  assert(key);

  if (hash_table_has(templates, key) != 1) {
    template_t* template = template_t_new();
    template->key = strdup(key);

    size_t len = strlen(key) + 3;
    template->value = malloc(len * sizeof(char));
    snprintf(template->value, len, "%s\n\r", key);

    hash_table_insert(templates, key, template);
  }

  template_t* existing_tpl = (template_t*)hash_table_get(templates, key);

  return existing_tpl->value;
}
