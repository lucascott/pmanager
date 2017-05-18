#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
//#include <prctl.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"
#include "utils.h"

#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

#define READ 0
#define WRITE 1

int main(int n_par, char *argv[]){
    int 	argc = 0;
    char    line[MAX_LINE_SIZE];
    char    *pch[MAX_ARGS];

    // apro la pipe
    pipe(fc);
    pipe(fn);

    if (signal(SIGUSR1, handler) == SIG_ERR){
        printf("\ncan't catch SIGUSR1\n");
    }
    // DEBUG
    pid_t main_p = getpid();
    printf("Main process id = %d\n", main_p);
    // end debug

    // inizializzo lista processi del padre
    initlist(processi);

    FILE *ifp;
    printf("Pmanager run with %d params\n", n_par);
    if (n_par == 2){ // SE PASSATO IL FILE
        ifp = fopen(argv[1], "r");
        if (ifp == NULL) {
            printf("Impossibile aprire \"%s\"...\n",argv[1]);
            exit(1);
        }
        while (fscanf(ifp,"%s", line) != EOF){

            if (isCommandWithParam(line)){
                strcat(line, " ");
                char temp[50];
                fscanf(ifp,"%s", temp);
                strcat(line, temp);
            }

            // DEBUG
            //char a[10];
            //scanf("%s", a);
            //END DEBUG

            // tokenizzo il comando passato
            tokenize(line, pch, &argc);
            if(argc == 1)
            printf("$> %s\n", pch[0]);
            else
            printf("$> %s %s\n", pch[0],pch[1]);
            // eseguo operazioni richieste
            esegui(pch, argc, processi);
        }
    }
    else {
        while (1 && getpid() == main_p) {
            // stampo il command prompt
            fflush(stdout); // serve per stampare tutto il buffer prima di dare il controllo alla shell
            printf("$> ");
            // lettura comandi
            if (!fgets(line, MAX_LINE_SIZE, stdin)){
                exit (1);
            }
            // tokenizzo il comando passato
            tokenize(line, pch, &argc);
            // eseguo operazioni richieste
            esegui(pch, argc, processi);
        }
    }
    return 0;
}
