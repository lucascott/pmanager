#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"
#include "utils.h"

//DEFINIZIONE COSTANTI
#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

#define READ 0
#define WRITE 1

//COLORE ERRORI SHELL
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

extern List processi; //dichiarazione esterna lista processi

extern int counter; //dichiarazione esterna del contatore processi cloni generati (solo per figli)

//PIPE (proc. figlio -> padre)
extern int     	fc[2], nbytes_c;
extern char		readbuffer_c[80];

//PIPE (proc. padre -> figlio)
extern int      fn[2], nbytes_n;
extern char     readbuffer_n[80];


int main(int n_par, char *argv[]) {
    int 	argc = 0;
    char    line[MAX_LINE_SIZE];
    char    *pch[MAX_ARGS];

    //apro la pipe
    pipe(fc);
    pipe(fn);

    if (signal(SIGUSR1, handler) == SIG_ERR) { //Apertura handler SIGUSR1
        printf(ANSI_COLOR_RED"Impossibile catturare il SIGUSR1\n"ANSI_COLOR_RESET);
    }

    initlist(&processi); //inizializzo lista processi del padre

    FILE *ifp; //gestione file di input
    if (n_par == 2 && access( argv[1], F_OK ) == -1) { //SE IL FILE NON ESISTE (poi avvia in modalità inserimento utente)
        printf(ANSI_COLOR_RED"File \"%s\" non trovato...\n\n"ANSI_COLOR_RESET,argv[1]);
    }
    if (n_par == 2 && access( argv[1], F_OK ) != -1) { //SE IL FILE ESISTE
        ifp = fopen(argv[1], "r");
        if (ifp == NULL) {
            printf(ANSI_COLOR_RED"Impossibile aprire \"%s\"...\n"ANSI_COLOR_RESET,argv[1]);
            exit(1);
        }
        while (fscanf(ifp,"%s", line) != EOF) { //lettura file sequenziale

            if (isCommandWithParam(line)) { //controllo se la riga è un comando che richiede parametri
                strcat(line, " ");
                char temp[50];
                fscanf(ifp,"%s", temp);
                strcat(line, temp);
            }

            tokenize(line, pch, &argc); //separe comando dagli argomenti
            if(argc == 1)
            printf("$> %s\n", pch[0]);
            else
            printf("$> %s %s\n", pch[0],pch[1]);
            esegui(pch, argc); // eseguo operazioni richieste
        }
    }
    else {
    printf("    ____                                                 \n   / __ \\____ ___  ____ _____  ____ _____ ____  _____    \n  / /_/ / __ `__ \\/ __ `/ __ \\/ __ `/ __ `/ _ \\/ ___/    \n / ____/ / / / / / /_/ / / / / /_/ / /_/ /  __/ /        \n/_/   /_/ /_/ /_/\\__,_/_/ /_/\\__,_/\\__, /\\___/_/  v1.3\n                                  /____/             \n\n");
        while (1) {
            fflush(stdout); //libero il buffer stdout
            printf("\r$> ");
            // lettura comandi
            if (!fgets(line, MAX_LINE_SIZE, stdin)) {
                exit (1);
            }
            tokenize(line, pch, &argc); //separo comando dagli argomenti
            esegui(pch, argc); //eseguo operazione richiesta
        }
    }
    return 0;
}
