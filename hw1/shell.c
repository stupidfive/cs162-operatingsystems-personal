#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>

#define FALSE 0
#define TRUE 1
#include "io.h"
#include "parse.h"

int cmd_quit(tok_t arg[]) {
  printf("Bye\n");
  exit(0);
  return 1;
}

int cmd_help(tok_t arg[]);

/* Command Lookup table */
typedef int cmd_fun_t (tok_t args[]); /* cmd functions take token arra and rtn int */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_quit, "quit", "quit the command shell"}
};

int cmd_help(tok_t arg[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;
}

int lookup(char cmd[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    if (strcmp(cmd_table[i].cmd, cmd) == 0) return i;
  }
  return -1;
}

int shell (int argc, char *argv[]) {
  char *s;			/* user input string */
  tok_t *t;			/* tokens parsed from input */
  int lineNum = 0;
  int fundex = -1;
  pid_t pid = getpid();		/* get current processes PID */
  pid_t ppid = getppid();	/* get parents PID */

  printf("%s running as PID %d under %d\n",argv[0],pid,ppid);

  lineNum=0;
  fprintf(stdout,"%d: ",lineNum);
  while ((s = freadln(stdin))) {
    t = getToks(s);		/* Break the line into tokens */
    fundex = lookup(t[0]);	/* Is first token a shell literal */
    if (fundex >= 0) cmd_table[fundex].fun(&t[1]);
    else {			/* Treat it as a file to exec */
      fprintf(stdout,"This shell currently supports only built-ins.  Replace this to run programs as commands.\n");
    }
    fprintf(stdout,"%d: ",++lineNum);
  }
  return 0;
}

int main (int argc, char *argv[]) {
  return shell(argc, argv);
}
