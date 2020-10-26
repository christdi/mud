#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mud/util/mudstring.h"

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

char *string_remove_range(char *source, unsigned int index, size_t len) {
    assert(source);
    assert(index < strlen(source));

    size_t length = strlen(source);

    while(source[index] != '\0') {
        size_t offset = index + len;

        if (offset < length) {
            source[index] = source[offset];
            index++;
        } else {
            source[index] = '\0';
        }
    }

    return source;
}

char *string_integer_to_ascii(int input) {
  char string[15];

  if (sprintf(string, "%d", input) < 0) {
    return NULL;
  }

  return strdup(string);
}

void string_to_hex(char * input, char * destination, size_t len) {
  assert(input);

  size_t i = 0;
  size_t j = 0;
  
  for (i = 0; i < len; i++) {
    sprintf(destination + j, "%02X ", input[i]);

    j += 3;
  }

  destination[j - 1] = '\0';
}
