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