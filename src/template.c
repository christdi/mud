#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "bsd/string.h"
#include "mud/data/hash_table.h"
#include "mud/log.h"
#include "mud/template.h"
#include "mud/util/mudstring.h"

int template_parse_line(char* line, hash_table_t* templates);

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

  mlog(INFO, "template_load_from_file", "Loading templates from [%s]", filename);

  FILE* fp = fopen(filename, "re");

  if (!fp) {
    return -1;
  }

  char buffer[TEMPLATE_FILE_MAX_LINE_LENGTH];

  while (fgets(buffer, TEMPLATE_FILE_MAX_LINE_LENGTH, fp)) {
    if (template_parse_line(buffer, templates) != 0) {
      mlog(ERROR, "template_load_from_file", "Unable to parse template file line: [%s]", buffer);

      continue;
    }
  }

  fclose(fp);

  return 0;
}

/**
 * Parses a line of a template configuration file which has a simple key=value structure.
 *
 * Parameters
 *  line - the line currently being parsed
 *  templates - the hash table templates should be stored in
**/
int template_parse_line(char* line, hash_table_t* templates) {
  assert(line);
  assert(templates);

  char* key = strtok(line, "=");
  char* value = strtok(NULL, "\n");

  if (!key || !value) {
    return -1;
  }

  template_t* template = template_t_new();
  template->key = strdup(key);

  char * tmp = replace(value, "\\r", "\r");
  template->value = replace(tmp, "\\n", "\n");
  free(tmp);

  hash_table_insert(templates, template->key, template);

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
