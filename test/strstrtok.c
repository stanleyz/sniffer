#include<string.h>
#include<stdio.h>
#include<stdlib.h>

char *strstrtok(char *haystack, const char *needle);

int main(int argc, char *argv[]) {
  int i = 0;

  for(i = 1; ; i++) {
    printf("%d\n", i);
    if(i == 20) break;
  }
  exit(1);
  char *haystack = argv[1];
  char *needle = argv[2];
  printf("haystack is %s\n", haystack);
  printf("needle is %s\n", needle);
  //int i = 0;
  char *s;

  printf("haha %s\n", strstrtok(haystack, needle));
  for(i = 0; ; i++) {
    s = strstrtok(NULL, needle);
    if(s == NULL) break;
    printf("%d is %s\n", i, s);
  }
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
