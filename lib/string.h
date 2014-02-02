#ifndef LIB_COMMON_H
#define LIB_COMMON_H

#include<string.h>

#define DELIMETER_FOUR "|`~|"

char *strdcat(char *dst, const char *src, const char *delimiter);
char *strsccat(char *dst, const char *src);
char *strstrtok(char *haystack, const char *needle);

#endif
