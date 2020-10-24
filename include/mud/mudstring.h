#include <stddef.h>

#ifndef HG_MUD_STRING_H
#define HG_MUD_STRING_H

char * string_remove(char *source, const char token);
char * string_remove_range(char *source, unsigned int index, size_t len);
char * string_integer_to_ascii(int input);
void string_to_hex(char * input, char * destination, size_t len);

#endif