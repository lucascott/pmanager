#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>

#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

char    	line[1024];
char    	*pch[MAX_ARGS];
int     	pcount = 0;


void tokenize(char * line, char ** tokens, int *argc){
    *argc = 0; // reinizializzo numero parole inserite
    tokens[*argc] = strtok(line," ,.-\n\t");
    while (tokens[*argc] != NULL) {
        (*argc)++;
        tokens[*argc] = strtok (NULL, " ,.-\n\t");
    }
}

int main(){

    int argc = 0;

    while (1) {
        /* Print the command prompt */
        printf("$> ");

        /* Read a command line */
        if (!fgets(line, MAX_LINE_SIZE, stdin))
            return 0;
        else {

            tokenize(line, pch, &argc);
            {
                int i;
                for (i = 0; i < argc; i++) {
                    printf("%d: %s\n", i, pch[i] );
                }
            }
        }
        //printf ("END, words: %d\n", argc);
    }
    return 0;
}
