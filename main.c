#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"

#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

#define READ 0
#define WRITE 1

List processi;

// pipe (proc. figlio -> padre)
int     	fc[2], nbytes_c;
char		readbuffer_c[80];

// pipe (proc. figlio -> padre)
int         fn[2], nbytes_n;
char        readbuffer_n[80];
// contatore processi cloni generati (solo per figli)
int 		counter = 0;


/// FUNZIONI
int isCommandWithParam(char * str) {
    if (strcmp(str, "pnew") == 0 ||
    strcmp(str, "pinfo") == 0 ||
    strcmp(str, "pclose") == 0 ||
    strcmp(str, "pspawn") == 0 ||
    strcmp(str, "prmall") == 0 ){
        return 1;
    }
    return 0;
}
void print_list(){
    printf("%d processi figli attivi\n", length(processi));
    printf("PID\t NAME\t PPID\n=====================\n");
    printlist(processi);
}
void tokenize(char * line, char ** tokens, int *argc){
    *argc = 0; // reinizializzo numero parole inserite
    tokens[*argc] = strtok(line," ,.-\n\t");
    while (tokens[*argc] != NULL) {
        (*argc)++;
        tokens[*argc] = strtok (NULL, " ,.-\n\t");
    }
}
void getMyPid(char * mystrpid) {
    sprintf(mystrpid,"%d",getpid());
}



static void handler (int signo) { //, siginfo_t *siginfo, void *context
    if (signo == SIGUSR1) {
        printf("%d received clone signal USR1\n", getpid());
        nbytes_n = -1;
        do {
            nbytes_n = read(fn[READ], readbuffer_n, sizeof(readbuffer_n));
            printf("letto...\n" );
            fflush(stdout);
            if (nbytes_n != -1){
                printf("nome dal main: %s\n", readbuffer_n);
            }
            fflush(stdout);
        }while(nbytes_n == -1);

        pid_t tpid = fork();
        if (tpid < 0) {
                printf("Failed to fork clone process\n");
                exit(1);
            }
        else if (tpid == 0) { // processo clone del figlio cicla all'infinito
            // crea l'handler per segnali di clonazione
            //signal(SIGUSR1,handler);
            printf("HANDLED, entrato nel clone...\n");

            fflush(stdout);
        }
        else {// processo padre "figlio del padre"
            counter++; // aumento contatore processi clonati

            //messaggio di ritorno al padre
            char str[MAX_LINE_SIZE];
            char strcounter[MAX_LINE_SIZE];
            char strtpid[MAX_LINE_SIZE];
            sprintf(strcounter,"%d",counter); // converto counter in stringa per strcat
            sprintf(strtpid,"%d",tpid);

            //Messaggio di risposta: "pid nome_counter"
            strcpy(str,strtpid);
            strcat(str," ");
            strcat(str,readbuffer_n);
            strcat(str,"_");
            strcat(str,strcounter);


            // invio messaggio al padre
            printf("STRINGA: %s\tPID dc: %d",str, getpid() );
            write(fc[WRITE], str,(strlen(str)+1));
            // DEBUG
            pid_t main_p = getpid();
            printf("this process id = %d\n", main_p);
            // end debug
            fflush(stdout);
        }
    }
}


void new_process(char *nome){
    printf("Creazione del processo %s\n", nome);
    pid_t pid = fork();
    pid_t ppid = getpid();
    if (pid < 0) {
        printf("Failed to fork process\n");
        exit(1);
    }
    else if (pid == 0){ // PROCESSO FIGLIO
        while(1){
            sleep(-1);
        }
    }
    else{
        insertback(&processi, pid, nome, ppid);
    }
}

void info_process(char *nome){
    pid_t pid = getPidbyName(&processi, nome);
    pid_t ppid = getPPidbyName(&processi, nome);
    printf("Processo %s (pid: %d, ppid: %d)\n", nome, pid, ppid);

}

void kill_process(char* nome){ // devo gestire se tolgo processi da in mezzo
    pid_t temp = change_item_name(&processi, nome, "XXX");
    kill(temp, SIGTERM);
}

void rmall_process(char* nome){ // devo gestire se tolgo processi da in mezzo
    printf("Chiusura albero processi di %s...\n", nome);
    rmallrec(&processi, nome);
}

void esegui(char *words[MAX_ARGS], int arg_counter) {
    // FATTO
    if (arg_counter == 1 && strcmp(words[0],"phelp") == 0){
        printf("\nComandi disponibili:\nphelp​ : stampa un elenco dei comandi disponibili\nplist​ : elenca i processi generati dalla shell custom\npnew <nome>​ : crea un nuovo processo con nome <nome>\npinfo <nome>​ : fornisce informazioni sul processo <nome>\npclose <nome>​ : chiede al processo <nome> di chiudersi\npspawn <nome>: chiede al processo <nome> di clonarsi creando <nome_i> con i progressivo\nprmall <nome>: chiede al processo <nome> di chiudersi chiudendo anche eventuali cloni\nptree: mostra la gerarchia completa dei processi generati attivi\nquit​ : esce dalla shell custom\n\n");
    }
    // FATTO -- formattazione fatta bene
    else if (arg_counter == 1 && strcmp(words[0],"plist") == 0){
        print_list();
    }
    // FATTO -- maggiori info in console
    else if (arg_counter == 2 && strcmp(words[0],"pnew") == 0){
        new_process(words[1]);
    }
    // FATTO
    else if (arg_counter == 2 && strcmp(words[0],"pinfo") == 0){
        info_process(words[1]);
    }
    // FATTO
    else if (arg_counter == 2 && strcmp(words[0],"pspawn") == 0){

        // ottengo pid processo dal nome passato dall'utente
        pid_t p = getPidbyName(&processi,words[1]);

        if (p == -1){
            printf("Errore processo %s non esistente\n", words[1]);
        }
        else {
            printf("Clonazione processo %s (pid: %d)\n", words[1], p);
            //mando messaggio a processo che deve clonarsi
            kill(p,SIGUSR1);
            write(fn[WRITE], words[1],(strlen(words[1])+1));
            signal(SIGUSR1,handler);
            nbytes_c = -1;
            do {
                nbytes_c = read(fc[READ], readbuffer_c, sizeof(readbuffer_c));
                printf("letto...\n" );
                fflush(stdout);
                if (nbytes_c != -1){
                    printf("risposta child: %s\n", readbuffer_c);
                    char * ris [MAX_ARGS];
                    int arg_ris;
                    tokenize(readbuffer_c, ris, &arg_ris);
                    insertback(&processi, atoi(ris[0]), ris[1], p);
                }
                fflush(stdout);
            } while (nbytes_c == -1);

        }

    }
    // FATTO
    else if (arg_counter == 2 && strcmp(words[0],"pclose") == 0){
        printf("Chiedo al processo %s di chiudersi\n", words[1]);
        kill_process(words[1]);
    }
    //
    else if (arg_counter == 2 && strcmp(words[0],"prmall") == 0){
        rmall_process(words[1]);
    }
    else if (arg_counter == 1 && strcmp(words[0],"ptree") == 0){
        printf("NON DISPONIBILE - albero\n");
    }
    else if (arg_counter == 1 && strcmp(words[0],"quit") == 0){
        killAll(&processi);
        exit(0);
    }
}

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
    initlist(&processi);

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
            esegui(pch, argc);
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
            esegui(pch, argc);
        }
    }
    return 0;
}
