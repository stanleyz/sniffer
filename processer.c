#include "processer.h"
#include<time.h>
#include<sys/socket.h>
#include<netdb.h>

int read_line(char *line, char *ts);
int analyze_line(char *line[]);
int mark_pos(const char *path, char *pos, const char *ts);
int mark_done(const char *path);

void *process(void *path) {
  char i_char[23];
  char j_char[23];
  char k_char[23];
  char t_char = '0';
  char line[512] = "0";
  int i = 0;
  FILE *f;

  getxattr(path, XATTR_ANALYZER, &t_char, 1);
  printf("\nProcessing file %s \n", path); 
  printf("  attr %s is %d\n", XATTR_ANALYZER, t_char - '0');
  switch(t_char - '0') {
    case X_A_DONE:
      mark_done(path);
      break;
    case X_A_INPROGRESS:
    case X_A_NONE:
      memset(i_char, 0, 23);
      memset(j_char, 0, 23);
      memset(k_char, 0, 23);
      getxattr(path, XATTR_ANALYZER_POS, i_char, 23);
      getxattr(path, XATTR_ANALYZER_POS_TS, j_char, 23);
      printf("  attr %s is %s\n", XATTR_ANALYZER_POS, i_char);
      printf("  attr %s is %s\n", XATTR_ANALYZER_POS_TS, j_char);
      f = fopen(path, "r");
      if(f == NULL) {
        fprintf(stderr, "Error when openning file %s\n", path);
        perror("fopen");
        return;
      }
      if(strlen(i_char) != 0) {
        fseek(f, atol(i_char), SEEK_SET);
        printf("  Moved to pos %d\n", atol(i_char));
        if(fgets(line, 512, f) == NULL) {
          fclose(f);
          return;
        }
        if(strlen(j_char) != 0 && strncmp(line, j_char, strlen(j_char)) !=0) {
          printf("  The position is not matched: %s, %s", j_char, line);
          fclose(f);
          return;
        } 
      }
      if(fgets(line, 512, f) == NULL) {
        printf("  End of the file or error occured, will read this file again after 1 min!\n");
        for(i = 0; i < 60; i++) {
          sleep(1);
          if(killed) {
            fclose(f);
            return;
          }
        }
        if(fgets(line, 512, f) == NULL) {
          printf("  Got EOF in second read, marking this file as finished!\n");
          mark_done(path);
          fclose(f);
          return;
        }
      }
      do {
        if(read_line(line, j_char) == -1) {
          continue; 
        }
        strcpy(k_char, i_char);
        sprintf(i_char, "%d", ftell(f));
        printf("  Finished line start with %s, location was %s, location is %s, killed is %d\n", j_char, k_char, i_char, killed);
        if(i++ == 600) {
          mark_pos(path, k_char, j_char);
          i = 0;
        }
        //sleep(1);
      } while(fgets(line, 512, f) != NULL && (killed == 0));
      mark_pos(path, k_char, j_char);

      MYSQL *conn = pthread_getspecific(my_con);
      if(conn != NULL) mysql_close(conn);
      mysql_thread_end();
      fclose(f);
      return;
  }
}

int mark_pos(const char *path, char *pos, const char *ts) {
  if(strlen(pos) == 0) {
    *pos = '0';
  }
  setxattr(path, XATTR_ANALYZER_POS, pos, strlen(pos), 0); 
  setxattr(path, XATTR_ANALYZER_POS_TS, ts, strlen(ts), 0);
  return;
}

int mark_done(const char *path) {
  char i_char[23];
  printf("  Marking file %s to finished...\n", path);
  sprintf(i_char, "%d", X_A_DONE);
  setxattr(path, XATTR_ANALYZER, i_char, strlen(i_char), 0); 
  strcpy(i_char,LOG_BAK_DIR);
  strcat(i_char, path);
  printf("  Moving file %s to bak dir...\n", path);
  rename(path, i_char);
  return;
}

int read_line(char *line, char *ts) {
  char *i_char;
  int j = 0;
  char *info[LINE_COLUMNS];

  /*
  info[0] = strstrtok(line, DELIMETER_FOUR);
  for(j = 1; ; j++) {
    i_char = strstrtok(NULL, DELIMETER_FOUR);
    if(i_char == NULL) break;
    info[j] = i_char;
  }
  */

  for(j = 0; ; j++) {
    i_char = strstr(line, DELIMETER_FOUR);
    if(i_char == NULL) {
      info[j++] = line;
      info[j] = NULL;
      //printf("hahaha %d is %s\n", j - 1, info[j - 1]);
      break;
    }   
    *(line + strlen(line) - strlen(i_char)) = '\0';
    info[j] = line;
    line = i_char + strlen(DELIMETER_FOUR);
  }

  /*
  for(j = 0; j < LINE_COLUMNS; j++) {
    if(info[j] == NULL) break;
    if(strcmp(info[j], "\n") == 0) break;
    printf("%d is %s\n", j, info[j]);
  }
  return;
  */

  if(strcmp(info[j - 1], "\n") == 0 && (j == LINE_COLUMNS - 5 || j == LINE_COLUMNS - 2 || j == LINE_COLUMNS)) {
    strcpy(ts, info[0]);
    return analyze_line(info);
  } else {
    fprintf(stderr, "  Invalid line!\n");
    return -1;
  }

}

int analyze_line(char *line[]) {
  char sec[LINE_COLUMNS];
  struct tm *tm;
  struct sockaddr_in sa;
  char *l[LINE_COLUMNS];
  char date[9];
  char host[30];
  //char cn_host[CN_LENGTH];
  long i_tmp = 0;
  char *c_tmp;
  struct in_addr addr;

  memset(sec, 0, sizeof(sec));
  memset(date, 0, sizeof(date));
  sscanf(line[0], "%[^-]", sec);
  i_tmp = atol(sec);
  tm = localtime(&i_tmp);
  if(tm == NULL) {
    fprintf(stderr, "  Error to parse time %s\n", sec);
    return EXIT_FAILURE;
  }

  if(strftime(date, sizeof(date), "%Y%m%d", tm) == 0) {
    fprintf(stderr, "  Error to change time to human readable strings\n");
    return EXIT_FAILURE;
  }
  l[0] = date;
  
  if(ip4insubnet(line[2], LOCAL_NET, LOCAL_NETBITS) == 1) {
    c_tmp = line[2];
    l[2] = line[3];
    if(line[6] != NULL && strcasecmp(line[4], "TCP") == 0) {
      l[5] = line[6];
    }
  }
  else{
    c_tmp = line[3];
    l[2] = line[2];
    if(line[5] != NULL && strcasecmp(line[4], "TCP") == 0) {
      l[5] = line[5];
    }
  } 
  
  memset(host, 0, sizeof(host));
  bzero(&sa, sizeof(struct sockaddr_in));
  sa.sin_family = AF_INET;
  sa.sin_addr.s_addr = inet_addr(c_tmp);
  getnameinfo((struct sockaddr *)&sa, sizeof(sa), host, sizeof(host), NULL, 0, NI_NOFQDN);
  if(strlen(host) <= 0) l[1] = c_tmp;
  else l[1] = host;

  l[3] = line[4];
  l[4] = line[1];

  l[7] = "\0";
  if(strcasecmp(line[4], "TCP") == 0) {
    if(strcasecmp(line[7], "FA") == 0) l[6] = "1";
    else l[6] = "0";

    for(i_tmp = 8; i_tmp < LINE_COLUMNS; i_tmp++) {
      if(strcmp(line[i_tmp], "\n") == 0) {
        l[i_tmp] = NULL;
        break;  
      }
      l[i_tmp] = line[i_tmp]; 
    }

    /*
    if(i_tmp == 8 && (atoi(l[5]) == 443 || atoi(l[5]) == 8443)) {
      cn_host[0] = '\0';
      pthread_mutex_lock(&mutex);
      get_crt_cn(l[2], l[5], cn_host); 
      pthread_mutex_unlock(&mutex);
      l[7] = cn_host;
    }
    */

  } else {
    l[5] = NULL;
    l[6] = NULL;
  }

  db_save(l);
}
