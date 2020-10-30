#ifndef _MUD_STRING_H_
#define _MUD_STRING_H_


#include <stddef.h>


#define ANSI_SIZE 17


/**
 * Function prototypes
**/
char * string_remove(char *source, const char token);
char * string_remove_range(char *source, unsigned int index, size_t len);
char * string_integer_to_ascii(int input);
void string_to_hex(char * input, char * destination, size_t len);
int convert_symbols_to_ansi_codes(char * input, char * destination, size_t len);


#endif
