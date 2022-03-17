#ifndef MUD_UTIL_MUDSTRING_H
#define MUD_UTIL_MUDSTRING_H

#include <stddef.h>

/**
 * Definitions
 **/
#define ANSI_SIZE 17
#define BUFFER_SIZE 1024
#define ARGUMENT_SIZE 256
#define SYMBOL_START '['
#define SYMBOL_END ']'

/**
 * Function prototypes
 **/
char* extract_argument(char* source, char* destination, size_t size);
char* trim(char* source);
char* trim_left(char* source);
char* trim_right(char* source);
char* lowercase(char* input);
int strcmpi(const char* s1, const char* s2);
int int_to_string(int input, char* destination);
void string_to_hex(char* input, char* destination, size_t len);
int convert_symbols_to_ansi_codes(char* input, char* destination, size_t len);
char* replace(const char* src, const char* find, const char* rplc);
char* replace_r(char* src, const char* find, const char* rplc);

#endif
