#include "http.h"

const char *http_method = ";GET;POST;PUT;OPTIONS;HEAD;DELETE;TRACE;CONNECT;";

void print_http(const char *data, char *buf) {
  char url_buf[256];
  char host_buf[96];
  char method_buf[10];
  int i = 1;

  method_buf[0] = ';';
  method_buf[1] = '\0';
  while(*data != ' ' && i < 8) {
    method_buf[i++] = toupper(*data++);
  }
  if(i < 4) {
    return;
  }
  method_buf[i++] = ';';
  method_buf[i] = '\0';

  if(strstr(http_method, method_buf) != NULL) {
    //printf("HTTP method is: %s, letngh is: %d\n", method_buf, strlen(method_buf));
    //printf("%.*s\n", 650, data);
    data += 1;
    i = 0;
    while(*data != '\r') {
      if(*data != ' ' && i < 255) {
        url_buf[i++] = *data++;
      } else {
        if(i < 256) {
          url_buf[i++] = '\0';
        }
        *data++;
      }
    }

    i = 0;
    host_buf[0] = '\0';
    while(data != '\0' && strlen(data) > 7 ) {
      if(toupper(*data++) == 'H' && toupper(*data++) == 'O' && toupper(*data++) == 'S' && toupper(*data++) == 'T' && *data++ == ':' && *data++ == ' ') {
        while(*data != '\r' && i < 95) {
          host_buf[i++] = *data++;
        }
        break;
      }
    }
    host_buf[i] = '\0';

    if(strlen(host_buf) > 0) {
      strsccat(buf, url_buf);
      strsccat(buf, host_buf);
    }
  }
  return;
}
