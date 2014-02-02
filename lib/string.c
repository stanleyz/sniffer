#include "string.h"

char * strdcat(char *dst, const char *src, const char *delimiter) {
  strcat(dst, src);
  strcat(dst, delimiter);
  
  return dst;
}
char * strsccat(char *dst, const char *src) {
  return strdcat(dst, src, DELIMETER_FOUR); 
}

char *strstrtok(char *haystack, const char *needle) {
  static char *remain;
  int len;

  if(haystack == NULL) {
    haystack = remain;
    if(haystack == NULL) {
      return NULL;
    }
  }

  remain = strstr(haystack, needle);
  len = strlen(haystack);
  if(remain != NULL) {
    len = len - strlen(remain);
    remain = remain + strlen(needle);
  }
  *(haystack + len) = '\0';

  return haystack;
}
