#include "network.h"

int ip4toint(char *ip) {
  int a = 0, b = 0, c = 0, d = 0;
  if(sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d) != 4) {
    fprintf(stderr, "error when parsing ip %s\n", ip);
    perror("scanf");
  }

  return a << 24 | b << 16 | c << 8 | d;
}

int ip4insubnet(char *ip, char *network, int netbits) {
  int i = 0;
  if(netbits > 32) return i;
  unsigned int mask = 0xFFFFFFFF << (32 - netbits);

  int i_ip = ip4toint(ip);
  int i_network = ip4toint(network);

  if((i_ip & mask) == (i_network & mask)) i = 1;

  return i;
}
