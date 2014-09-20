#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

int cmd_cd(tok_t arg[]);

int cmd_wait(tok_t arg[]);

/* Command Lookup table */
typedef int cmd_fun_t (tok_t args[]); /* cmd functions take token array and return int */
typedef struct fun_desc {
  cmd_fun_t *fun;
  char *cmd;
  char *doc;
} fun_desc_t;

fun_desc_t cmd_table[] = {
  {cmd_help, "?", "show this help menu"},
  {cmd_cd, "cd", "change directories"},
  {cmd_quit, "quit", "quit the command shell"},
  {cmd_wait, "wait", "waiting for child processes to finish"}
};

int cmd_help(tok_t arg[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    printf("%s - %s\n",cmd_table[i].cmd, cmd_table[i].doc);
  }
  return 1;
}

int cmd_cd(tok_t arg[]) {
  if (!arg) fprintf(stderr, "cd missing argument\n");
  else chdir(arg[0]);
  return 1;
}

int cmd_wait(tok_t arg[]) {
  pid_t pid;
  while ((pid = waitpid(-1, NULL, 0)) != 0) {
    if (errno == ECHILD) {
      break;
    }
  }
  return 1;
}

int lookup(char cmd[]) {
  int i;
  for (i=0; i < (sizeof(cmd_table)/sizeof(fun_desc_t)); i++) {
    if (cmd && (strcmp(cmd_table[i].cmd, cmd) == 0)) return i;
  }
  return -1;
}

#define MAXPROMPT 1024
void prompt(int lineNum) {
  char pbuf[MAXPROMPT];
  fprintf(stdout,"%d %s: ",++lineNum, getcwd(pbuf, MAXPROMPT));
}

int shell (int argc, char *argv[]) {
  char *s;			/* user input string */
  tok_t *t;			/* tokens parsed from input */
  int lineNum = 0;
  int fundex = -1;
  char *path = getenv("PATH");
  tok_t *tpath = getToks(path);
  pid_t pid = getpid();		/* get current processes PID */
  pid_t ppid = getppid();	/* get parents PID */
  pid_t cpid, tcpid;
  int cstatus, cerr;
  int is_background;
  tok_t *token;
  char *character;

  printf("%s running as PID %d under %d\n",argv[0],pid,ppid);
  lineNum=0;
  prompt(lineNum);
  while ((s = freadln(stdin))) {
    is_background = 0;
    t = getToks(s);		/* Break the line into tokens */
    fundex = lookup(t[0]);	/* Is first token a shell literal */
    if (fundex >= 0) cmd_table[fundex].fun(&t[1]);
    else {			/* Treat it as a file to exec */
      for (token = t; *token != NULL; token++) {
        for (character = token[0]; *character; character++) {
          if (*character == '&') {
            if (character == token[0]) {
              *token = '\0';
            } else {
              *character = '\0';
            }
            is_background = 1;
          }
        }
      }
      char cmd[MAXPROMPT];
      memset(&cmd[0], 0, sizeof(cmd));
      if (!strchr(t[0], '/')) {
        char exec[MAXPROMPT];
        int i;
        for (i=0; tpath[i]; i++) {
          memset(&exec[0], 0, sizeof(exec));
          strcat(exec, tpath[i]);
          strcat(exec, "/");
          strcat(exec, t[0]);
          if ( (access(exec, F_OK)) != -1) {
            strcat(cmd, tpath[i]);
            strcat(cmd, "/");
            break;
          }
        }
      }
      strcat(cmd, t[0]);
      cpid = fork();		/* fork a child */
      if (cpid == 0) {		/* child process */
	      cerr = execv(cmd, t);		/* exec the file within child process */
	      printf("Child: %s exited with error %d\n",t[0],errno);
	      _exit(EXIT_FAILURE);	/* terminate child normally */
      } else if (cpid > 0) {			/* parent process */
        if (!is_background) {
          tcpid = wait(&cstatus);
        }
      } else {			/* fork failed */
	      printf("Fork of child process failed\n");
      }
    }
    prompt(++lineNum);
  }
  return 0;
}

int main (int argc, char *argv[]) {
  return shell(argc, argv);
}