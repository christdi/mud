#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mud/string.h"

char * string_copy(const char * source) {
	if ( !source ) {
		return 0;
	}
	char * copy = malloc(strlen(source) + 1);

	strcpy(copy, source);

	return copy;
}


char * string_integer_to_ascii(int input) {
    char string[15];

    if ( sprintf(string, "%d", input) < 0 ) {
        return 0;
    }

    return string_copy(string);
}
