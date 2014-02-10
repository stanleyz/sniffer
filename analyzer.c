#include "analyzer.h"

void sig_handler(int signal);
int killed = 0;
pthread_mutex_t mutex;
pthread_key_t my_con;

int main(int argc, char *argv[]) {
  struct dirent **namelist;
  int t_int;
  int i_int = 0;
  int j_int = 0;
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
  signal(SIGUSR1, get_cache_status);

  chdir(LOG_DIR);
  DIR *d = opendir(LOG_BAK_DIR);
  if(d == NULL) {
    mkdir(LOG_BAK_DIR, 0755);
  }
  closedir(d);

  pthread_mutex_init(&mutex, NULL);
  pthread_key_create(&my_con, NULL);
  mysql_library_init(0, NULL, NULL);
  while(killed == 0) {
    j_int = t_int = scandir(".", &namelist, NULL, versionsort);
    if(t_int == -1) {
      perror("scandir");
      exit(EXIT_FAILURE);
    }

    pthread_t threads[t_int];
    while(t_int--) {
      if(namelist[t_int]->d_type != DT_REG) continue;
      if(strlen(strcasestr(namelist[t_int]->d_name, LOG_NAME)) != strlen(namelist[t_int]->d_name)) continue;
      //process(namelist[t_int]->d_name);
      if(pthread_create(&threads[i_int++], NULL, process, (void *)namelist[t_int]->d_name) != 0) {
        fprintf(stderr, "\nError when creating thread\n");  
      }
    }

    for(t_int = 0; t_int < i_int; t_int++) {
      pthread_join(threads[t_int], NULL);
    }

    for(t_int = 0; t_int < j_int; t_int++) {
      free(namelist[t_int]);
    }
    free(namelist);

    if(killed == 0) {
      printf("\nI've finished %d of the whold log directory, will have a 1 min rest, don't try to wake me up!!!\n", ++scan_count);
      sleep(60);
    }
  }

  mysql_library_end();
  pthread_mutex_destroy(&mutex);
  pthread_key_create(&my_con, NULL);
  chdir("../");
  unlink(lock_f);
  pthread_exit(NULL);
  exit(EXIT_SUCCESS);
}

void sig_handler(int signal) {
  printf("Got signal %d, quitting now...\n", signal);
  killed = 1;
  fflush(stdout);
  fflush(stderr);
}
