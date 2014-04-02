#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "shishell.h"

int shishell(char* e_input) {
    int n = 1,
        cur_i = 0,
        cur_w = 0;
    char tmp;
    char** input = malloc(sizeof(char*));
    input[n-1] = malloc(sizeof(char));

    while ((tmp = e_input[cur_i]) == ' ') {
        cur_i++;
    }
    while ((tmp = e_input[cur_i]) != '\0') {
        switch (tmp) {
            case ' ':
                cur_w = 0; cur_i++;
                if (e_input[cur_i+1] != ' ') {
                    n++;
                    //input[n-1] = malloc(sizeof(char));
                }
                break;
            case '&':
                shiexec(input, EXMODE_BACKGR);
                int j;
                for (j = 0; j < n; j++) {
                    free(input[j]);
                }
                cur_w = 0; cur_i++; n = 0;
                input = realloc(input, sizeof(char*));
                printf("background process created\n");
                break;
            default:
                input[n-1] = realloc(input[n-1], sizeof(char*)*cur_w+2);
                input[n-1][cur_w] = e_input[cur_i];
                input[n-1][cur_w+1] = '\0';
                cur_w++; cur_i++;
                break;
        }
    }
    shiexec(input, EXMODE_NORMAL);
}

int shiexec(char** e_input_arr, emode e_mode) {
    pid_t cpid;
    if((cpid = fork()) == 0) {
        int ret = 0;
        switch (execvpe(e_input_arr[0], e_input_arr, (char*[]){"/bin","/usr/bin"})) {
            case -1:
                printf("%s\n", strerror(errno));
                ret = -1;
                break;
            case 0:
                ret =  0;
                break;
        }
    } else {
        int stat;
        waitpid(cpid, &stat, 0);
    }
}

