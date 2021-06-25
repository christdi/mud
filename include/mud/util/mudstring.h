#ifndef _MUD_STRING_H_
#define _MUD_STRING_H_


#include <stddef.h>


/**
 * Definitions
**/
#define ANSI_SIZE 17
#define BUFFER_SIZE 1024
#define SYMBOL_START '['
#define SYMBOL_END ']'


/**
 * Function prototypes
**/
char * extract_argument(char * source, char * destination);
char * trim(char * source);
char * trim_left(char * source);
char * trim_right(char * source);
int int_to_string(int input, char * destination);
void string_to_hex(char * input, char * destination, size_t len);
int convert_symbols_to_ansi_codes(char * input, char * destination, size_t len);


#endif
