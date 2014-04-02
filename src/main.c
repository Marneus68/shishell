#include <stdio.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <getopt.h>

#include "shishell.h"
#include "constants.h"

const char * PROMPT_EMPTY       = "  ";
const char * PROMPT_INS_MODE    = "i ";
const char * PROMPT_NOR_MODE    = "n ";

const char * getprompt(imode e_mode) {
    switch(e_mode) {
        case INMODE_INSERT:
            return PROMPT_INS_MODE;
        case INMODE_NORMAL:
            return PROMPT_NOR_MODE;
        default:
            return PROMPT_EMPTY;
    }
}

int main(int argc, const char *argv[])
{
    int c,
        verbose = 0;

    imode mode = INMODE_INSERT;

    static struct option lopts[] = {
            {"normal",  no_argument,        0,  'n' },
            {"input",   no_argument,        0,  'i' },
            {"verbose", no_argument,        0,  'v' },
            {"help", no_argument,           0,  'h' },
            {"version", no_argument,        0,  'V' },
            {0,    0,                       0,  0 }
    };

    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "This program should be run in a proper tty session.\n");
        return EXIT_FAILURE;
    }

    while (1) {
        int optind,
            topind = optind ? optind : 1,  
            i = 0;
        
        if ((c = getopt_long(argc, (char * const*)argv, "nivhV", lopts, &optind)) == -1)
            break;

        switch (c) {
            case 'n':
                mode = INMODE_NORMAL;
                break;
            case 'i':
                mode = INMODE_INSERT;
                break;
            case 'v':
                verbose = 1;
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

    if (verbose) {
        printf("Starting %s version %s", NAME, VERSION);
        if (mode == INMODE_NORMAL)
            printf(" in NORMAL mode ");
        else if (mode = INMODE_INSERT)
            printf(" in INSERT mode ");
        printf("...\n");
    }

    for(;;) {
        char *input = "";

        foor:

        printf(getprompt(mode));
        char c = getchar();
        for(;;) {
            if (mode == INMODE_NORMAL) {
                switch (c) {
                    case '\b':
                    case 'h':
                        printf("<BACK>");
                        break;
                    case 'l':
                        printf("<FORWARD>");
                        break;
                    case 'i': // switch to INSERT MODE
                        mode = INMODE_INSERT;
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
            } else if (mode == INMODE_INSERT) {
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
                        mode = INMODE_NORMAL;
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
