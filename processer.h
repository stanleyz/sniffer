#ifndef _PROCESSER_H
#define _PROCESSER_H

#include<stdio.h>
#include "analyzer.h"
#include "lib/string.h"
#include "lib/network.h"
#include "lib/mysql.h"

#define LOCAL_NET "10.1.1.0"
#define LOCAL_NETBITS 24

extern int killed;

void *process(void *path);

#endif
