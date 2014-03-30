#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <getopt.h>

#include "shishell.h"
#include "constants.h"

typedef enum imode {
    MODE_NORMAL = 0,
    MODE_INSERT = 1
} imode;

const char * PROMPT_EMPTY       = "  ";
const char * PROMPT_INS_MODE    = "i ";
const char * PROMPT_NOR_MODE    = "n ";

const char * getprompt(imode e_mode) {
    switch(e_mode) {
        case MODE_INSERT:
            return PROMPT_INS_MODE;
        case MODE_NORMAL:
            return PROMPT_NOR_MODE;
        default:
            return PROMPT_EMPTY;
    }
}

int main(int argc, const char *argv[])
{
    int c;

    imode mode = MODE_INSERT;

    static struct option lopts[] = {
            {"normal",  no_argument,        0,  'n' },
            {"input",   no_argument,        0,  'i' },
            {"verbose", no_argument,        0,  'v' },
            {"help", no_argument,           0,  'h' },
            {"version", no_argument,        0,  'V' },
            {0,    0,                       0,  0 }
    };

    printf("Starting %s version %s ...\n", NAME, VERSION);

    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "This program should be run in a proper tty session.\n");
        return EXIT_FAILURE;
    }

    while (1) {
        int optind,
            topind = optind ? optind : 1,  
            i = 0;
        
        if ((c = getopt_long(argc, argv, "nivhV", lopts, &optind)) == -1)
            break;

        switch (c) {
            case 'n':
                printf("Starting in NORMAL mode\n");
                mode = MODE_NORMAL;
                break;
            case 'i':
                printf("Starting in INSERT mode\n");
                mode = MODE_INSERT;
                break;
            case 'v':
                break;
            case 'h':
                printf("%s - %s\n", NAME, VERSION);
                printf("%s\n", USAGE);
                exit(EXIT_SUCCESS);
            case 'V':
                printf("%s - %s\n", NAME, VERSION);
                exit(EXIT_SUCCESS);
            default:
                break;
        }

    }

    static struct termios   oldt,  
                            newt;

    tcgetattr(STDIN_FILENO, &oldt);    
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);      

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    for(;;) {
        char *input = "";
        /*
        snprintf(   shell_prompt, sizeof(shell_prompt), "%s:%s $ ", 
                    getenv("USER"), getcwd(NULL, 1024));
        */
        foor:

        printf(getprompt(mode));
        char c = getchar();
        for(;;) {
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
