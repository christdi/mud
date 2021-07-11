#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "mud/log.h"
#include "mud/util/mudstring.h"

/**
 * Mapping of markup to ANSI control codes.
**/
char* const ansi_codes[ANSI_SIZE][2] = {
  { "[black]", "\033[0;30m]" },
  { "[red]", "\033[0;31m]" },
  { "[green]", "\033[0;32m]" },
  { "[yellow]", "\033[0;33m]" },
  { "[blue]", "\033[0;34m]" },
  { "[magenta]", "\033[0;35m]" },
  { "[cyan]", "\033[0;36m]" },
  { "[white]", "\033[0;37m]" },
  { "[gray]", "\033[0;90m]" },
  { "[bred]", "\033[0;91m]" },
  { "[bgreen]", "\033[0;92m]" },
  { "[byellow]", "\033[0;93m]" },
  { "[bblue]", "\033[0;94m]" },
  { "[bmagenta]", "\033[0;95m]" },
  { "[bcyan]", "\033[0;96m]" },
  { "[bwhite]", "\033[0;97m]" },
  { "[reset]", "\033[0m]" }
};

/**
 * Attempts to extract a single space deliminated argument from a source string.
 * The extracted word is copied into the destination character buffer which must
 * be a valid character buffer.
 *
 * Returns a character pointer to after the extracted word in the source
**/
char* extract_argument(char* source, char* destination) {
  assert(source);
  assert(destination);

  char* current = source;
  char* write = destination;

  while (*current != ' ' && *current != '\0') {
    *write = *current;

    current++;
    write++;
  }

  if (*current == ' ') {
    current++;
  }

  *write = '\0';

  return current;
}

/**
 * Wrapper around trim_left and trim_right.
**/
char* trim(char* source) {
  return trim_left(trim_right(source));
}

/**
 * Creates a pointer which is iterated over a string until the first non
 * whitespace character is found.  Be careful using this with dynamically
 * allocated strings as you won't be able to free the memory if you over
 * write the original pointer.
 *
 * Returns a pointer to the first non-whitespace character in the string.
**/
char* trim_left(char* source) {
  char* current = source;

  while (isspace(*current)) {
    current++;
  }

  return current;
}

/**
 * Finds the end of a string and iterates backwards replacing spaces with null
 * until such time as it finds a non-space character.
 *
 * Returns the same pointer it was passed in so the call be chained.
**/
char* trim_right(char* source) {
  size_t len = strlen(source) - 1;

  char* current = source + len;

  while (isspace(*current)) {
    *current = '\0';
    current--;
  }

  return source;
}

/**
 * Attempts to convert an integer to a string.  A valid destination character
 * buffer must be provided for the string to be written to.
 *
 * Returns 0 on success or -1 on failure
**/
int int_to_string(int input, char* destination) {
  assert(input);
  assert(destination);

  if (sprintf(destination, "%d", input) < 0) {
    return -1;
  }

  return 0;
}

/**
 * Convert up to len characters of the input string into a hex representation.
 * A valid destination character buffer must be provided for the string to be
 * written to.
**/
void string_to_hex(char* input, char* destination, size_t len) {
  assert(input);
  assert(destination);

  size_t i = 0;
  size_t j = 0;

  for (i = 0; i < len; i++) {
    sprintf(destination + j, "%02X ", input[i]);

    j += 3;
  }

  destination[j - 1] = '\0';
}

/**
 * Given an input string, search for our custom markup and replace instances with
 * the equivalent ANSI control codes.  A valid destination character buffer must be
 * provided and it must be large enough to hold the converted string.
 *
 * Returns 0 on success or -1 on failure
**/
int convert_symbols_to_ansi_codes(char* input, char* destination, size_t len) {
  assert(input);
  assert(destination);

  size_t input_length = strlen(input);

  if (input_length > len) {
    return -1;
  }

  char* current = input;
  char* write = destination;
  char* markup_start = NULL;
  size_t markup_count = 0;
  size_t copied = 0;

  while (*current) {
    if (*current == SYMBOL_START && !markup_start) {
      markup_start = current;
    }

    if (markup_start) {
      markup_count++;

      if (*current == SYMBOL_END) {
        char* markup = strndup(markup_start, markup_count);

        int i = 0;

        for (i = 0; i < ANSI_SIZE; i++) {
          if (strcmp(markup, ansi_codes[i][0]) == 0) {
            size_t ansi_length = strlen(ansi_codes[i][1]) - 1;

            if (copied + ansi_length > len) {
              free(markup);

              return -1;
            }

            memcpy(write, ansi_codes[i][1], ansi_length);
            write += ansi_length;
            copied += ansi_length;
            break;
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
