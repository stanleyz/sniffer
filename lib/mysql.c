#include "mysql.h"

MYSQL *conn = NULL;

MYSQL *get_connection() {
  const char *user = "root";
  const char *pass = "root";
  const char *host = "10.1.1.82";
  const char *db = "sniffer";
  if(conn == NULL) {
    conn = mysql_init(conn);
    mysql_options(conn, MYSQL_OPT_RECONNECT, "1");
    mysql_real_connect(conn, host, user, pass, db, 0, NULL, 0);
  }

  return conn;
}

int db_save(char *line[]) {
  MYSQL_RES *r;
  MYSQL_ROW row;
  int i_tmp = 0;
  unsigned long long j_tmp = 0;
  char i_char[64];
  char j_char[64];
  char sql[630];

  /*
  for(i_tmp = 0; i_tmp < LINE_COLUMNS; i_tmp++) {
    if(line[i_tmp] == NULL) {
      printf("\n");
      break;
    }
    printf("%d is %s\n", i_tmp, line[i_tmp]);
  }
  */

  get_connection();

  strcpy(sql, "SELECT * FROM traffic WHERE date = '");
  strcat(sql, line[0]);
  strcat(sql, "' AND src = '");
  strcat(sql, line[1]);
  strcat(sql, "' AND d_ip = '");
  strcat(sql, line[2]);
  strcat(sql, "' AND protocol = '");
  strcat(sql, line[3]);
  if(strcmp(line[3], "TCP") == 0) {
    strcat(sql, "' AND d_port = '");
    strcat(sql, line[5]);
  }
  strcat(sql, "';");

  //printf("%s\n", sql);
  if(mysql_query(conn, sql) != 0) {
    fprintf(stderr, "  Error when executing sql: %s\n", sql);
    return;
  }
  mysql_query(conn, sql);
  r = mysql_store_result(conn);
  if(r != NULL) {
    row = mysql_fetch_row(r);

    if(row != NULL) {
      strcpy(sql, "UPDATE traffic SET size = ");
      sprintf(i_char , "%ld", atol(row[6]) + atol(line[4]));
      strcat(sql, i_char);
      if(line[6] != NULL && strcmp(line[6], "0") != 0) {
        strcat(sql, ", 2hit = ");
        sprintf(j_char, "%d", atol(row[7]) + atol(line[6]));
        strcat(sql, j_char);
      }

      strcat(sql, " WHERE id = ");
      strcat(sql, row[0]);

      //printf("%s\n", sql);
      if(mysql_query(conn, sql) != 0) {
        fprintf(stderr, "  Error when executing sql: %s\n", sql);
        return;
      }
      save_http_log(line, row[0]);
    } else {
      strcpy(sql, "INSERT INTO traffic(date, src, d_ip, protocol, size, d_port, 2hit, hostname) VALUES('");
      for(i_tmp = 0; i_tmp < 8; i_tmp++) {
        if(line[i_tmp] == NULL) {
          strcat(sql, "', '");
        } else {
          strcat(sql, line[i_tmp]);
          strcat(sql, "', '");
        }
      }
      sql[strlen(sql) - 3] = ')';
      sql[strlen(sql) - 2] = '\0';

      //printf("%s\n", sql);
      if(mysql_query(conn, sql) != 0) {
        fprintf(stderr, "  Error when executing sql: %s\n", sql);
        return;
      }

      j_tmp = mysql_insert_id(conn);
      sprintf(i_char, "%lld", j_tmp);

      save_http_log(line, i_char); 
    }

    mysql_free_result(r);
  }
}

int save_http_log(const char *line[], const char *traffic_id) {
  char sql[512];

  if(strcmp(line[3], "TCP") == 0 && line[8] != NULL) {
    strcpy(sql, "INSERT INTO http_log(traffic_id, domain, url) VALUES('");
    strcat(sql, traffic_id);
    strcat(sql, "', '");
    strcat(sql, line[9]);
    strcat(sql, "', '");
    strcat(sql, line[8]);
    strcat(sql, "')");

    //printf("%s\n", sql);
    if(mysql_query(conn, sql) != 0) {
      fprintf(stderr, "  Error when executing sql: %s\n", sql);
      return;
    }
  }
}
