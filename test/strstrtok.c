#include<string.h>
#include<stdio.h>
#include<stdlib.h>

char *strstrtok(char *haystack, const char *needle);

int main(int argc, char *argv[]) {
  char *haystack = argv[1];
  char *needle = argv[2];
  printf("haystack is %s\n", haystack);
  printf("needle is %s\n", needle);
  int i = 0, j = 0, k = 0;
  char *s;
  char *remain;

  for(i = 0; ; i++) {
    remain = strstr(haystack, needle);
    if(remain == NULL) {
      printf("last one is %s\n", haystack);
      break;
    }
    *(haystack + strlen(haystack) - strlen(remain)) = '\0';
    printf("%d is %s\n", i, haystack);
    haystack = remain + strlen(needle);
  }
}

char *strstrtok(char *haystack, const char *needle) {
  char *remain;
  int len;

  if(haystack == NULL) {
    return NULL;
  }

  printf("\nhaystack is %s\n", haystack);
  printf("remain before is %s\n", remain);
  remain = strstr(haystack, needle);
  printf("remain is %s\n", remain);
  len = strlen(haystack);
  if(remain != NULL) {
    len = len - strlen(remain);
    remain = remain + strlen(needle);
  }
  *(haystack + len) = '\0';


  return haystack;
}
