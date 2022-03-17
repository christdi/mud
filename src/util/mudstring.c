#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsd/string.h"

#include "mud/log.h"
#include "mud/util/mudstring.h"

/**
 * Mapping of markup to ANSI control codes.
 **/
static const char* ansi_codes[][2] = {
  { "[black]", "\033[0;30m" },
  { "[red]", "\033[0;31m" },
  { "[green]", "\033[0;32m" },
  { "[yellow]", "\033[0;33m" },
  { "[blue]", "\033[0;34m" },
  { "[magenta]", "\033[0;35m" },
  { "[cyan]", "\033[0;36m" },
  { "[white]", "\033[0;37m" },
  { "[gray]", "\033[0;90m" },
  { "[bred]", "\033[0;91m" },
  { "[bgreen]", "\033[0;92m" },
  { "[byellow]", "\033[0;93m" },
  { "[bblue]", "\033[0;94m" },
  { "[bmagenta]", "\033[0;95m" },
  { "[bcyan]", "\033[0;96m" },
  { "[bwhite]", "\033[0;97m" },
  { "[reset]", "\033[0m" },
  { NULL }
};

/**
 * Extracts an argument from a string, an argument defined as either a single
 * word or a sequence of words contained within quotes.  Preceeding whitespace
 * will be ignored.  If the argument does not fit within the destination buffer
 * then it will be truncated.
 *
 * Parameters
 *  source - the string from which arguments should be extracted
 *  destination - an allocated string with sufficient space for the argument
 *  size - the size of the argument buffer
 *
 * Returns a character pointer to after the extracted word in the source
 **/
char* extract_argument(char* source, char* destination, size_t size) {
  assert(source);
  assert(destination);

  char* current = source;
  char* write = destination;

  while (*current != '\0' && isblank(*current)) {
    current++;
  }

  if (*current == '\0') {
    *write = '\0';

    return source;
  }

  char terminator = ' ';

  if (*current == '"') {
    terminator = '"';
    current++;
  }

  size_t count = 0;

  while (*current != terminator && *current != '\0') {
    if (count++ == size) {
      *write = '\0';

      while (*current != terminator && *current != '\0') {
        current++;
      }

      return current;
    }

    *write++ = *current++;
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
 * Modifies a string inline to all lowercase.  The string must be null terminated
 * or a buffer overflow will likely occur.
 *
 * Parameters
 *  input - the string to be lowercased
 *
 * Returns input
 **/
char* lowercase(char* input) {
  char* w = input;

  while (*w != '\0') {
    *w = tolower(*w);
    w++;
  }

  return input;
}

/**
 * Case insensitive comparison of two strings.
 *
 * Parameters
 *  s1 - first string to compare, must be null terminated
 *  s2 - second string to compare, must be null terminated
 *
 * Returns the results of strncmp of s1 and s2
 **/
int strcmpi(const char* s1, const char* s2) {
  char* s1_lower = lowercase(strndup(s1, strlen(s1) + 1));
  char* s2_lower = lowercase(strndup(s2, strlen(s2) + 1));

  int result = 0;

  const char* c1 = s1_lower;
  const char* c2 = s2_lower;

  while (*c1 != '\0' || *c2 != '\0') {
    if (*c1++ != *c2++) {
      result = -1;
      break;
    }
  }

  free(s1_lower);
  free(s2_lower);

  return result;
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
 * Replace occurrences of "find" with "rplc" in a given string src. The src string must
 * be null terminated or behaviour is undefined.  This method calculates how big the new
 * string must be to accomodate replacements and allocates a buffer big enough to hold it.
 * It's the caller's responsibility to free the replacement string when done.
 *
 * Parameters
 *  src - the source string to be replaced
 *  find - what to search for
 *  rplc - what to replace it with
 *
 * Returns a newly allocated string that must be freed by the caller when done.
 **/
char* replace(const char* src, const char* find, const char* rplc) {
  size_t find_len = strlen(find);
  size_t rplc_len = strlen(rplc);
  size_t new_len = strlen(src);

  const char* c = src;
  int found = 0;

  while (*c != '\0') {
    if (strncmp(c, find, find_len) == 0) {
      if (find_len > rplc_len) {
        new_len = new_len - (find_len - rplc_len);
      } else {
        new_len = new_len + (rplc_len - find_len);
      }

      found = 1;
    }

    c++;
  }

  if (found == 0) {
    return strdup(src);
  }

  size_t new_size = (new_len + 1) * sizeof(char);
  char* replacement = malloc(new_size);

  c = src;
  char* w = replacement;

  while (*c != '\0') {
    if (strncmp(c, find, find_len) == 0) {
      strncpy(w, rplc, rplc_len);
      c += find_len;
      w += rplc_len;
    } else {
      *w++ = *c++;
    }
  }

  *w = '\0';

  return replacement;
}

/**
 * Variant of replace that will automatically free the src after it has been used
 * by replace.  This is to allow chainging of replace calls.
 *
 * Parameters
 *  src - the source string to be replaced
 *  find - what to search for
 *  rplc - what to replace it with
 *
 * Returns a newly allocated string that must be freed by the caller when done.
 **/
char* replace_r(char* src, const char* find, const char* rplc) {
  char* new = replace(src, find, rplc);

  free(src);

  return new;
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

  int i = 0;
  char* tmp = strdup(input);
  char* swp = NULL;

  while (ansi_codes[i][0] != NULL) {
    swp = tmp;
    tmp = replace(swp, ansi_codes[i][0], ansi_codes[i][1]);
    free(swp);

    i++;
  }

  strlcpy(destination, tmp, len);
  free(tmp);

  return 0;
}
