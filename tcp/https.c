#include "https.h"
#include<openssl/ssl.h>
#include<openssl/err.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>

int get_crt_cn(const char *host, const char *port, char *cn) {
  int sfd, t_int, j_int = -1;
  struct addrinfo hints;
  struct addrinfo *result;
  *cn = '\0';
  SSL_CTX *ctx;
  SSL *ssl;
  X509 *crt;
  X509_NAME *name;
  X509_NAME_ENTRY *e;

  j_int = get_cn_from_cache(host, atoi(port), cn);
  if(strlen(cn) > 0) return;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM;

  t_int = getaddrinfo(host, port, &hints, &result);
  if(t_int != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(t_int));
    return t_int;
  }

  if(result == NULL) {
    fprintf(stderr, "can't connect to null");
    return;
  }
  sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  if(sfd == -1) {
    fprintf(stderr, "socket error: %s\n", strerror(errno)); 
    return sfd;
  }

  t_int = connect(sfd, result->ai_addr, result->ai_addrlen);
  if(t_int == -1) {
    fprintf(stderr, "connect error: %s\n", strerror(t_int));
    return t_int;
  }

  freeaddrinfo(result);

  SSL_load_error_strings();
  SSL_library_init();

  ctx = SSL_CTX_new(SSLv23_client_method());
  ssl = SSL_new(ctx); 
  SSL_set_fd(ssl, sfd);
  SSL_connect(ssl);
  crt = SSL_get_peer_certificate(ssl);

  if(crt != NULL) {
    name = X509_get_subject_name(crt);
    X509_NAME_get_text_by_NID(name, NID_commonName, cn, CN_LENGTH);
  } 

  set_cn_to_cache(host, atoi(port), cn, j_int);
  X509_free(crt);
  SSL_free(ssl);
  SSL_CTX_free(ctx);

  close(sfd);
  return EXIT_SUCCESS;
}

int get_cn_from_cache(const char *host, int port, char *cn) {
  int i_int = -1;

  for(i_int = 0; i_int < CN_CACHE_SIZE; i_int++) {
    if(cn_cache[i_int].ip == NULL) break;

    if(strcasecmp(cn_cache[i_int].ip, host) == 0 && cn_cache[i_int].port == port) {
      if((time(NULL) - cn_cache[i_int].updated_on) < ONE_MONTH) {
        //printf("  Found cn in cache for %s:%d ...\n", host, port);
        strcpy(cn, cn_cache[i_int].cn);
        cn_cache[i_int].hit_count++;
      }
      break;
    }
  } 

  return i_int;
}

int set_cn_to_cache(const char *host, int port, const char *cn, int index) {
  int i_int = -1;
  if(index >= -1 && index < CN_CACHE_SIZE) {
    i_int = index; 
  } else {
    int hit = 2^sizeof(int) - 1;
    int hit_index = 0;

    for(i_int = 0; i_int < CN_CACHE_SIZE; i_int++) {
      if(cn_cache[i_int].ip == NULL || (time(NULL) - cn_cache[i_int].updated_on) > ONE_MONTH) {
        if(cn_cache[i_int].hit_count < hit) {
          hit_index = i_int;
          hit = cn_cache[i_int].hit_count;
        }
        break;
      }
    } 

    if(i_int == CN_CACHE_SIZE) i_int = hit_index;
  }

  //printf("  Setting cn in cache for %s:%d...\n", host, port);
  strcpy(cn_cache[i_int].ip, host);
  cn_cache[i_int].port = port;
  strcpy(cn_cache[i_int].cn, cn);
  cn_cache[i_int].updated_on = time(NULL);
}
