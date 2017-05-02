/* Compile with: g++ -Wall –Werror -o shell shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ARGS = 256;
#define MAX_LINE_SIZE = 1024;
pid_t pid;
static char line[1024];
int argc = 0;

void esegui(char * words[MAX_ARGS], arg_counter);

int main()
{
    printf("SIMPLE SHELL: Type 'exit' or send EOF to exit.\n");
    while (1) {
        /* Print the command prompt */
        printf("$> ");

        /* Read a command line */
        if (!fgets(line, MAX_LINE_SIZE, stdin))
            return 0;
        else {
            argc = 0; // reinizializzo numero parole inserite
            char * pch[MAX_ARGS];
            pch[argc] = strtok(line," ,.-\n");
            while (pch[argc] != NULL) {
                argc++;
                printf ("%s\n",pch[argc-1]);
                pch[argc] = strtok (NULL, " ,.-\n");
            }
        }
        printf ("END, words: %d\n", argc);
        //esegui(pch, argc);
    }
    return 0;
}

void esegui(char * words[MAX_ARGS], arg_counter){
    if (arg_counter == 1 && strcmp(words[0],"phelp"))
        printf("Comandi disponibili:\nphelp​ : stampa un elenco dei comandi disponibili\nplist​ : elenca i processi generati dalla shell custom\npnew <nome>​ : crea un nuovo processo con nome <nome>\npinfo <nome>​ : fornisce informazioni sul processo <nome> (almeno ​ pid ​ e ​ ppid ​ )\npclose <nome>​ : chiede al processo <nome> di chiudersi\nquit​ : esce dalla shell custom\n");
    /*
    else if (strcmp("phelp",primo) == 0 && arg_counter > 1)
        printf("Sei mona, come delsi\n");
    */
    else if (strcmp("quit",primo) == 0 && arg_counter == 1)
        exit(0);
}
