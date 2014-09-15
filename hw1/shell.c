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

int cmd_cd(tok_t arg[]) {
    if (arg[0] == NULL) {
        chdir(getenv("HOME"));
    } else {
        chdir(arg[0]);
    }
}
int cmd_wait(tok_t arg[]) {
    while (wait() != -1);
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
    {cmd_quit, "quit", "quit the command shell"},
    {cmd_cd, "cd", "change directories"},
    {cmd_wait, "wait", "wait for all background processes to finish"}
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
    char *path = malloc(1024);
    char *cur_path = malloc(4096);
    pid_t pid = getpid();		/* get current processes PID */
    pid_t ppid = getppid();	/* get parents PID */

    printf("%s running as PID %d under %d\n",argv[0],pid,ppid);

    lineNum=0;
    char *cwd = malloc(4096);
    getcwd(cwd, 4096);
    fprintf(stdout,"%d %s: ",lineNum, cwd);
    while ((s = freadln(stdin))) {
        char *and = strstr(s, "&");
        if (and) *and = 0;
        t = getToks(s, " \n");		/* Break the line into tokens */
        fundex = lookup(t[0]);	/* Is first token a shell literal */
        if (fundex >= 0) cmd_table[fundex].fun(&t[1]);
        else {			/* Treat it as a file to exec */
            int pid;
            if (strstr(t[0], "/") == t[0] || strstr(t[0], "./") == t[0] || strstr(t[0], "../") == t[0]) {
                pid = fork();
                if (pid == 0) {
                    int execexit = execv(t[0], t);
                } else if (!and) {
                    wait(pid);
                }
            } else {
                path = getenv("PATH");
                tok_t *path_strings;
                path_strings = getToks(path, ":");
                int i = 0;
                while (path_strings[i] != NULL) {
                    strcat(cur_path, path_strings[i]);
                    strcat(cur_path, "/");
                    strcat(cur_path, t[0]);
                    if (access(cur_path, X_OK) != -1) {
                        pid = fork();
                        if (pid == 0) {
                            int execexit = execv(cur_path, t);
                        } else if (!and) {
                            wait(pid);
                        }
                        path_strings[i + 1] = NULL;
                    }
                    i++;
                    cur_path[0] = 0;
                }
            }
        }
        getcwd(cwd, 4096);
        fprintf(stdout,"%d %s: ",++lineNum, cwd);
    }
    free(cwd);
    return 0;
}
int main (int argc, char *argv[]) {
    return shell(argc, argv);
}

