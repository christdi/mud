#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "mud/string.h"

char * string_copy(const char * source) {
    assert(source);
    
    char * copy = malloc(strlen(source) + 1);

    strcpy(copy, source);

    return copy;
}

char * string_remove(char * source, const char token) {
	assert(source);
	assert(token);

	char * current;
	char * destination;

	for (current = destination = source; *current != '\0'; current++) {
		*destination = *current;

		if (*destination != token) {
			destination++;
		}
	}

	*destination = '\0';

	return source;
}


char * string_integer_to_ascii(int input) {
    char string[15];

    if ( sprintf(string, "%d", input) < 0 ) {
        return 0;
    }

    return string_copy(string);
}
