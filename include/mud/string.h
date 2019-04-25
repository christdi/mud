#ifndef _MUD_STRING_H_
#define _MUD_STRING_H_

char * string_copy(const char * source);
char * string_remove(char * source, const char token);
char * string_extract(char * source, const int startIndex, const int endIndex);
char * string_append(char * source, char * destination);
char * string_integer_to_ascii(int input);

#endif
