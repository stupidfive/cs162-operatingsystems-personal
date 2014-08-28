#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024
#define DELIMS " \t\r\n"

int main(int argc, char *argv[]) {
    char *cmd;
    char line[MAX_LENGTH];

    printf("./shell running as PID %d under %d\n", getpid(), getppid());

    while(1) {
        printf("$ ");
        if (!fgets(line, MAX_LENGTH, stdin)) {
            break;
        }

        //Parse the command
        if ((cmd = strtok(line, DELIMS))) {

            //Execute the command
            if (strcmp(cmd, "exit") == 0) {
                printf("Bye!");
                break;
            }
            system(line);
            }
    }

    return 0;
}
