#ifndef _MUD_STRING_H_
#define _MUD_STRING_H_


#include <stddef.h>


/**
 * Definitions
**/
#define ANSI_SIZE 17
#define SYMBOL_START '['
#define SYMBOL_END ']'


/**
 * Function prototypes
**/
int int_to_string(int input, char * destination);
void string_to_hex(char * input, char * destination, size_t len);
int convert_symbols_to_ansi_codes(char * input, char * destination, size_t len);



#endif
