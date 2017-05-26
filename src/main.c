#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"
#include "utils.h"

#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

#define READ 0
#define WRITE 1

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern List processi;

// pipe (proc. figlio -> padre)
extern int     	fc[2], nbytes_c;
extern char		readbuffer_c[80];

// pipe (proc. figlio -> padre)
extern int      fn[2], nbytes_n;
extern char     readbuffer_n[80];

// contatore processi cloni generati (solo per figli)
extern int 		counter;

int main(int n_par, char *argv[]){
    int 	argc = 0;
    char    line[MAX_LINE_SIZE];
    char    *pch[MAX_ARGS];

    // apro la pipe
    pipe(fc);
    pipe(fn);

    if (signal(SIGUSR1, handler) == SIG_ERR){
        printf(ANSI_COLOR_RED"Impossibile catturare il SIGUSR1\n"ANSI_COLOR_RESET);
    }

    // inizializzo lista processi del padre
    initlist(&processi);

    FILE *ifp;
    if (n_par == 2 && access( argv[1], F_OK ) == -1){ // SE IL FILE NON ESISTE (poi avvia in modalità inserimento utente)
        printf(ANSI_COLOR_RED"File \"%s\" non trovato...\n\n"ANSI_COLOR_RESET,argv[1]);
    }
    if (n_par == 2 && access( argv[1], F_OK ) != -1){ // SE PASSATO IL FILE
        ifp = fopen(argv[1], "r");
        if (ifp == NULL) {
            printf(ANSI_COLOR_RED"Impossibile aprire \"%s\"...\n"ANSI_COLOR_RESET,argv[1]);
            exit(1);
        }
        while (fscanf(ifp,"%s", line) != EOF){

            if (isCommandWithParam(line)){
                strcat(line, " ");
                char temp[50];
                fscanf(ifp,"%s", temp);
                strcat(line, temp);
            }

            // tokenizzo il comando passato
            tokenize(line, pch, &argc);
            if(argc == 1)
            printf("$> %s\n", pch[0]);
            else
            printf("$> %s %s\n", pch[0],pch[1]);
            // eseguo operazioni richieste
            esegui(pch, argc);
        }
    }
    else {
        printf("PMANAGER: \n");
        while (1) {
            fflush(stdout); // serve per stampare tutto il buffer prima di dare il controllo alla shell
            printf("\r$> ");
            // lettura comandi
            if (!fgets(line, MAX_LINE_SIZE, stdin)){
                exit (1);
            }
            // tokenizzo il comando passato
            tokenize(line, pch, &argc);
            // eseguo operazioni richieste
            esegui(pch, argc);
        }
    }
    return 0;
}
