/* Compile with: g++ -Wall –Werror -o shell shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

pid_t pid;
static char line[1024];
int argc = 0;

int main()
{
    printf("SIMPLE SHELL: Type 'exit' or send EOF to exit.\n");
    while (1) {
        /* Print the command prompt */
        printf("$> ");

        /* Read a command line */
        if (!fgets(line, 1024, stdin))
            return 0;
        else {
            argc = 0; // reinizializzo numero parole inserite
            char * pch[256];
            pch[argc] = strtok(line," ,.-\n");
            while (pch[argc] != NULL) {
                argc++;
                printf ("%s\n",pch[argc-1]);
                pch[argc] = strtok (NULL, " ,.-\n");
            }
        }
        printf ("END, words: %d\n", argc);
        // do stuff
    }
    return 0;
}
