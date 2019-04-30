#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mud/mudstring.h"

char *string_remove(char *source, const char token) {
  assert(source);
  assert(token);

  char *current;
  char *destination;

  for (current = destination = source; *current != '\0'; current++) {
    *destination = *current;

    if (*destination != token) {
      destination++;
    }
  }

  *destination = '\0';

  return source;
}

char *string_integer_to_ascii(int input) {
  char string[15];

  if (sprintf(string, "%d", input) < 0) {
    return NULL;
  }

  return strdup(string);
}
