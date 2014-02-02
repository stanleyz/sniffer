#ifndef LIB_MYSQL_H
#define LIB_MYSQL_H

#include<mysql/mysql.h>
#include<stdio.h>
#include<string.h>

#define LINE_COLUMNS 11

extern MYSQL *conn;

int db_save(char *line[]);

#endif
