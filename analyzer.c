#include "analyzer.h"

void sig_handler(int signal);
int killed = 0;

int main(int argc, char *argv[]) {
  struct dirent **namelist;
  int t_int;
  int scan_count = 0;

  chdir(dirname(argv[0]));
  char lock_f[strlen(basename(PROG_NAME)) + 5];
  strcpy(lock_f, PROG_NAME);
  strcat(lock_f, ".lck");

  FILE *f = NULL;
  if((f = fopen(lock_f, "r")) != NULL) {
    fprintf(stderr, "There is a same process running there, quit now!\n");
    fclose(f);
    exit(EXIT_FAILURE);
  }

  f = fopen(lock_f, "w+");
  fclose(f);
  signal(SIGTERM, sig_handler);
  signal(SIGINT, sig_handler);

  chdir(LOG_DIR);
  if(opendir(LOG_BAK_DIR) == NULL) {
    mkdir(LOG_BAK_DIR, 0755);
  }

  while(killed == 0) {
    t_int = scandir(".", &namelist, NULL, versionsort);
    if(t_int == -1) {
      perror("scandir");
      exit(EXIT_FAILURE);
    }
    while(t_int-- && (killed == 0)) {
      if(namelist[t_int]->d_type != DT_REG) continue;
      if(strlen(strcasestr(namelist[t_int]->d_name, LOG_NAME)) != strlen(namelist[t_int]->d_name)) continue;
      process(namelist[t_int]->d_name);
    }
    free(namelist);

    if(killed == 0) {
      printf("\nI've finished %d of the whold log directory, will have a 1 min rest, don't try to wake me up!!!\n", ++scan_count);
      sleep(60);
    }
  }

  mysql_close(conn);
  conn = NULL;
  chdir("../");
  unlink(lock_f);
  exit(EXIT_SUCCESS);
}

void sig_handler(int signal) {
  printf("Got signal %d, quitting now...\n", signal);
  killed = 1;
  fflush(stdout);
  fflush(stderr);
}
