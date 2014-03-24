#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "shishell.h"
#include "constants.h"

typedef enum imode {
    MODE_NORMAL = 0,
    MODE_INSERT = 1
} imode;

const char * PROMPT_INS_MODE = "i ";
const char * PROMPT_NOR_MODE = "n ";

int main(int argc, const char *argv[])
{
    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "This program should be run in a proper tty session.\n");
        return EXIT_FAILURE;
    }

    static struct termios oldt, newt;

    imode mode = MODE_INSERT;

    tcgetattr(STDIN_FILENO, &oldt);    
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);      

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    printf("Starting %s version %s ...\n", NAME, VERSION);

    for(;;) {
        char *input = "";
        /*
        snprintf(   shell_prompt, sizeof(shell_prompt), "%s:%s $ ", 
                    getenv("USER"), getcwd(NULL, 1024));
        */
        foor:

        printf(PROMPT_INS_MODE);
        char c = getchar();
        while (1) {
            if (mode == MODE_NORMAL) {
                switch (c) {
                    case '\b':
                    case 'h':
                        printf("<BACK>");
                        break;
                    case 'l':
                        printf("<FORWARD>");
                        break;
                    case 'i': // switch to INSERT MODE
                        mode = MODE_INSERT;
                        printf("\033[s\r%s\033[u", PROMPT_INS_MODE);
                        break;
                    case 'e':
                        printf("<END_WORD>");
                        break;
                    case 'b':
                        printf("<BACK_WORD>");
                        break;
                    case 'w':
                        printf("<NEXT_WORD>");
                        break;
                    default:
                        break;
                }
            } else {
                switch (c) {
                    case '\b':
                        printf("<BACKSPACE>");
                        break;
                    case 9:
                        printf("<TAB>");
                        break;
                    case '\n': // RETURN
                        putchar('\n');
                        if (strlen(input) != 0) {
                            shishell(input);
                            strcpy(input, "");
                        }
                        goto foor;
                        break;
                    case 27: // switch to NORMAL MODE
                        mode = MODE_NORMAL;
                        printf("\033[s\r%s\033[u", PROMPT_NOR_MODE);
                        break;
                    default:
                        putchar(c);

                        size_t l = strlen(input);
                        char* tmp = malloc(sizeof(char)*l);
                        strcpy(tmp, input);
                        input = malloc(sizeof(char)*l+2);
                        strcpy(input, tmp);

                        char tmps[ 2 ];
                        tmps[ 0 ] = c;
                        tmps[ 1 ] = '\0';
                        strcat(input, tmps);

                        break;
                }
            }
            c = getchar();
        }
        __fpurge(stdin);
        free(input);
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return EXIT_SUCCESS;
}
