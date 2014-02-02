#ifndef _ANALYZER_H
#define _ANALYZER_H

#include "tcp/https.h"
#include "processer.h"
#include<signal.h>
#include<stdio.h>
#include<libgen.h>
#include<dirent.h>
#include<attr/xattr.h>

#define LOG_DIR "log/"
#define LOG_BAK_DIR "bak/"
#define LOG_NAME "sniffer.log"
#define XATTR_ANALYZER "user.analyzer"
#define XATTR_ANALYZER_POS "user.analyzer.pos"
#define XATTR_ANALYZER_POS_TS "user.analyzer.pos.ts"
#define PROG_NAME "analyzer"

enum {
  X_A_NONE = 0,
#define X_A_NONE X_A_NONE
  X_A_DONE = 1,
#define X_A_DONE X_A_DONE
  X_A_INPROGRESS = 2,
#define X_A_INPROGRESS X_A_INPROGRESS
};

#endif
