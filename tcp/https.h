#ifndef TCP_HTTPS_H
#define TCP_HTTPS_H

#define _GNU_SOURCE
#include<string.h>
#include<stdlib.h>

#define CN_LENGTH 96
#define ONE_MONTH 3600*24*30
#define CN_CACHE_SIZE 2048

struct cn_cache {
  char ip[16];
  int port;
  char cn[CN_LENGTH];
  time_t updated_on;
  int hit_count;
};

static struct cn_cache cn_cache[CN_CACHE_SIZE];

int get_crt_cn(const char *host, const char *port, char *cn);

void get_cache_status();

#endif
