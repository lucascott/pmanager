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

void swapandset0(int i, int last) {
    if(last-i >= 0) { // solo per check
        processi[i] = processi[last];
        processi[last] = 0;
    }
}

void ChildProcess(char *n) {
    pid_t  pid;
    char   *name = n;

    pid = getpid();
    printf("Child process %s starts (pid = %d)\n", name, pid);
    printf("STILL RUNNING DC\n");
}



void killProcess(int pid){ // devo gestire se tolgo processi da in mezzo

    kill(pid, SIGKILL);
    int i = 0;
    while (i < pcount){
        if (pid == processi[i]){
            swapandset0(i, pcount - 1);
        }
        i++;
    }
    pcount--;
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
        processi[pcount] = fork();
        if (processi[pcount] < 0) {
            printf("Failed to fork process\n");
            exit(1);
        }
        if (processi[pcount] == 0){ // child process
            ChildProcess(words[1]);
        }
        else { // parent process
            pcount++;
        }
    }
    else if (arg_counter == 2 && strcmp(words[0],"pinfo") == 0){
        printf("informazioni sul processo %s (almeno pid e ppid)\n", words[1]);

        // DEBUG
        pid_t main_p = getpid();
        printf("this process id = %d\n", main_p);
        // end debug
    }
    else if (arg_counter == 2 && strcmp(words[0],"pclose") == 0){
        printf("chiede al processo %s di chiudersi\n", words[1]);
        killProcess(atoi(words[1]));
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
