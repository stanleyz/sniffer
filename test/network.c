#include<stdio.h>
#include<string.h>
#include "../tcp/https.h"

int main(int argc, char *argv[]) {


}

int get_cn_from_cache(const char *host, int port, char *cn) {
  int i_int = 0;

  for(i_int = 0; i_int < sizeof(cn_cache); i_int++) {
    if(cn_cache[i_int].ip == NULL) break;

    if(strcasecmp(cn_cache[i_int].ip, host) == 0 && cn_cache[i_int].port == port) {
      if((time() - cn_cache[i_int].updated_on) < ONE_MONTH)
        strcpy(cn, cn_cache[i_int].cn);
        cn_cache[i_int].hit_count++;
      break;
    }
  } 

  return i_int;
}

int set_cn_to_cache(const char *host, int port, char *cn) {
  int i_int = 0;
  int hit = 2^sizeof(int) - 1;
  int hit_index = 0;

  for(i_int = 0; i_int < sizeof(cn_cache); i_int++) {
    if(cn_cache[i_int].ip == NULL || (time() - cn_cache[i_int].updated_on) > ONE_MONTH) {
      if(cn_cache[i_int].hit_count < hit) {
        hit_index = i_int;
        hit = cn_cache[i_int].hit_count;
      }
      break;
    }
  } 

  if(i_int == sizeof(cn_cache)) i_int = hit_index;
  
  strcpy(cn_cache[i_int].ip, host);
  cn_cache[i_int].port = port;
  strcpy(cn_cache[i_int].cn, cn);
  cn_cache[i_int].updated_on = time();
}
