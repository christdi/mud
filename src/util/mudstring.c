#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mud/util/mudstring.h"
#include "mud/log/log.h"

char * const ansi_codes[ANSI_SIZE][2] = {
  { "<black>",          "\033[0;30m]" },
  { "<red>",            "\033[0;31m]" },
  { "<green>",          "\033[0;32m]" },
  { "<yellow>",         "\033[0;33m]" },
  { "<blue>",           "\033[0;34m]" },
  { "<magenta>",        "\033[0;35m]" },
  { "<cyan>",           "\033[0;36m]" },
  { "<white>",          "\033[0;37m]" },
  { "<gray>",           "\033[0;90m]" },
  { "<bright red>",     "\033[0;91m]" },
  { "<bright green>",   "\033[0;92m]" },
  { "<bright yellow>",  "\033[0;93m]" },
  { "<bright blue>",    "\033[0;94m]" },
  { "<bright magenta>", "\033[0;95m]" },
  { "<bright cyan>",    "\033[0;96m]" },
  { "<bright white>",   "\033[0;97m]" },
  { "<reset>",          "\033[0m]"    }
};


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


int convert_symbols_to_ansi_codes(char * input, char * destination, size_t len) {
  assert(input);
  assert(destination);

  size_t input_length = strlen(input);

  if (input_length > len) {
    return -1;
  }

  char * current = input;
  char * write = destination;
  char * markup_start = NULL;
  size_t markup_count = 0;
  size_t copied = 0;

  while (*current) {
    if (*current == '<' && !markup_start) {
      markup_start = current;
    }

    if (markup_start) {
      markup_count++;

      if (*current == '>') {
        char * markup = strndup(markup_start, markup_count);

        int i = 0;

        for ( i = 0; i < ANSI_SIZE; i++) {
          if (strcmp(markup, ansi_codes[i][0]) == 0) {
            size_t ansi_length = strlen(ansi_codes[i][1]) - 1;

            if (copied + ansi_length > len) {
              free(markup);

              return -1;
            } else {
              memcpy(write, ansi_codes[i][1], ansi_length);
              write += ansi_length;
              copied += ansi_length;
              break;
            }
          }
        }

        free(markup);

        markup_start = NULL;
        markup_count = 0;
        current++;

        continue;
      }
    }

    if (!markup_start) {
      *write = *current;
      write++;
      copied++;
    }
    
    current++;
  }

  *write = '\0';

  return 0;
}
