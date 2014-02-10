#ifndef LIB_MYSQL_H
#define LIB_MYSQL_H

#include<mysql/mysql.h>
#include<pthread.h>
#include<stdio.h>
#include<string.h>

#include "../tcp/https.h"

#define LINE_COLUMNS 11

extern pthread_mutex_t mutex;
extern pthread_key_t my_con;

int db_save(char *line[]);

#endif
