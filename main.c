/* Compile with: g++ -Wall –Werror -o shell shell.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>

#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

pid_t   pid;
char    line[1024];
char    *pch[MAX_ARGS];

pid_t   processi[100];
int     pcount = 0;

/// FUNZIONI

void ChildProcess(char *n)
{
     pid_t  pid;
     char   *name = n;

     pid = getpid();
     printf("Child process %s starts (pid = %d)\n", name, pid);
     wait();
}

void esegui(char *words[MAX_ARGS], int arg_counter) {
    if (arg_counter == 1 && strcmp(words[0],"phelp") == 0){
        printf("\nComandi disponibili:\nphelp​ : stampa un elenco dei comandi disponibili\nplist​ : elenca i processi generati dalla shell custom\npnew <nome>​ : crea un nuovo processo con nome <nome>\npinfo <nome>​ : fornisce informazioni sul processo <nome> (almeno ​ pid ​ e ​ ppid ​ )\npclose <nome>​ : chiede al processo <nome> di chiudersi\nquit​ : esce dalla shell custom\n\n");
    }
    else if (arg_counter == 1 && strcmp(words[0],"plist") == 0){
        printf("%d processi figli attivi\n", pcount);
        int i = 0;
        while (i < pcount) {
            printf("Open process with pid = %d\n",processi[i]);
            i++;
        }

    }
    else if (arg_counter == 2 && strcmp(words[0],"pnew") == 0){
        printf("Creazione del processo %s\n", words[1]);
        pid_t p
        processi[pcount] = fork();
        if (processi[pcount] < 0) {
            printf("Failed to fork process\n");
            exit(1);
        }
        else if (processi[pcount] == 0){
            pcount++; // Nb da tenere sempre dopo l'if ma prima del ChildProcess
            ChildProcess(words[1]);
        }
    }
    else if (arg_counter == 2 && strcmp(words[0],"pinfo") == 0){
        printf("informazioni sul processo %s (almeno pid e ppid)\n", words[1]);
    }
    else if (arg_counter == 2 && strcmp(words[0],"pclose") == 0){
        printf("chiede al processo %s di chiudersi\n", words[1]);
        kill(, SIGKILL);
    }
    else if (arg_counter == 1 && strcmp(words[0],"quit") == 0)
        exit(0);
}

int main()
{
    printf("SIMPLE SHELL: Type 'exit' or send EOF to exit.\n");
    int argc = 0;

    // DEBUG
    pid_t main_p = getpid();
    printf("Main process id = %d\n", main_p);
    // end debug

    
    while (1) {
        /* Print the command prompt */
        printf("$> ");

        /* Read a command line */
        if (!fgets(line, MAX_LINE_SIZE, stdin))
            return 0;
        else {
            argc = 0; // reinizializzo numero parole inserite
            pch[argc] = strtok(line," ,.-\n\t");
            while (pch[argc] != NULL) {
                argc++;
                //printf ("%s\n",pch[argc-1]);
                pch[argc] = strtok (NULL, " ,.-\n\t");
            }
        }
        //printf ("END, words: %d\n", argc);
        esegui(pch, argc);
    }
    return 0;
}
