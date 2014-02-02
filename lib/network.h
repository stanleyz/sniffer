#ifndef _LIB_NETWORK_H
#define _LIB_NETWORK_H

#include<stdio.h>

int ip4toint(char *ip);
int ip4insubnet(char *ip, char *network, int netbits);

#endif
