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

// lista dei processi
List processi;

// pipe (proc. figlio -> padre)
int         fc[2];
char		readbuffer_c[80];

// pipe (proc. figlio -> padre)
int         fn[2];
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
    printf("PID\t NAME           \t PPID\n========================================\n");
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

int alphanumeric(char * str){
    int i = 0;
    while(isalnum(str[i])) {
        i++;
    }
    if(i == (strlen(str)))
        return 1;
    else
        return 0;
}


void handler (int signo) { //, siginfo_t *siginfo, void *context
    if (signo == SIGUSR1) {
        //printf("%d received clone signal USR1\n", getpid());

        int nbytes_n = read(fn[READ], readbuffer_n, sizeof(readbuffer_n));
        if (nbytes_n == -1) {
            printf("Clonazione processo non riuscita...\n");
            exit(-1);
        }
        pid_t tpid = fork();
        if (tpid < 0) {
            printf("Clonazione processo non riuscita...\n");
            exit(-1);
        }
        else if (tpid == 0) { // processo clone del figlio
            counter = 0;
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
            //printf("STRINGA: %s\tPID dc: %d",str, getpid() );
            write(fc[WRITE], str,(strlen(str)+1));
            //fflush(stdout);
        }
    }
}


void new_process(char *nome){
    if (getPidbyName(&processi, nome) != -1){
        printf(ANSI_COLOR_RED"Processo %s già presente. Comando ignorato...\n" ANSI_COLOR_RESET, nome);
    }
    else {
        printf("Creazione del processo %s\n", nome);
        pid_t pid = fork();
        pid_t ppid = getpid();
        if (pid < 0) {
           printf(ANSI_COLOR_RED "Clonazione processo non riuscita...\n"ANSI_COLOR_RESET);
           exit(-1);
        }
        else if (pid == 0){ // PROCESSO FIGLIO
            destroy(&processi); // dealloca la lista dei figli per salvare spazio
            while(1){
                sleep(-1);
            }
        }
        else{
            insertback(&processi, pid, nome, ppid);
        }
        printf("Processo %s creato con successo (pid: %d)\n", nome, pid);
        fflush(stdout);
    }
}

void info_process(char *nome){
    pid_t pid, ppid;
    getInfos(&processi, nome, &pid, &ppid);
    if (pid == -1 && ppid == -1){
        printf(ANSI_COLOR_RED"Processo %s inesistente. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else{
        printf("Processo %s (pid: %d, ppid: %d)\n", nome, pid, ppid);
    }
}

void kill_process(char* nome){ // devo gestire se tolgo processi da in mezzo
    pid_t temp = change_item_name(&processi, nome, "XXX");
    if (temp == -1){
        printf(ANSI_COLOR_RED"Processo %s inesistente. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else{
        kill(temp, SIGTERM);
        printf("Processo %s chiuso correttamente.\n", nome);
    }
}

void rmall_process(char* nome){ // devo gestire se tolgo processi da in mezzo
    if (getPidbyName(&processi, nome) == -1){
        printf(ANSI_COLOR_RED"Processo %s inesistente. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else{
        printf("Chiusura albero processi di %s...\n", nome);
        rmallrec(&processi, nome);
    }
}

void tree_process(){
    if (processi.head == 0) {
        printf("\nNessun processo attivo.\n\n");
    }
    else {
        printf("\nAlbero processi:\n");
        treerecchild(processi.head, getpid(),0,0);
        printf("\n");
    }
}

void esegui(char *words[MAX_ARGS], int arg_counter) {
    // FATTO
    if (arg_counter == 1 && strcmp(words[0],"phelp") == 0){
        printf("\nComandi disponibili:\n\nphelp​:\t\tstampa un elenco dei comandi disponibili\nplist​:\t\telenca i processi generati dalla shell custom\npnew [arg]:\tcrea un nuovo processo con nome <arg>\npinfo [arg]​:\tfornisce informazioni sul processo <arg>\npclose [arg]:\tchiede al processo <arg> di chiudersi\npspawn [arg]:\tchiede al processo <arg> di clonarsi creando <arg_i> con 'i' progressivo\nprmall [arg]:\tchiede al processo <arg> di chiudersi chiudendo anche eventuali cloni\nptree:\t\tmostra la gerarchia completa dei processi generati attivi\nquit​:\t\tesce dalla shell custom\n\n");
    }
    // FATTO
    else if (arg_counter == 1 && strcmp(words[0],"plist") == 0){
        print_list();
    }
    // FATTO -- maggiori info in console
    else if (arg_counter == 2 && strcmp(words[0],"pnew") == 0){
        if (alphanumeric(words[1]))
            new_process(words[1]);
        else
            printf(ANSI_COLOR_RED"Il nome del processo deve essere alfanumerico\n"ANSI_COLOR_RESET);
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
            printf("Errore processo %s non esistente. Comando ignorato...\n", words[1]);
        }
        else {
            printf("Clonazione processo %s (pid: %d)\n", words[1], p);
            //mando messaggio a processo che deve clonarsi
            kill(p,SIGUSR1);
            write(fn[WRITE], words[1],(strlen(words[1])+1));
            signal(SIGUSR1,handler);
            int nbytes_c = read(fc[READ], readbuffer_c, sizeof(readbuffer_c));
            if (nbytes_c == -1) {
                printf(ANSI_COLOR_RED"Clonazione processo non riuscita...\n"ANSI_COLOR_RESET);
                exit(-1);
            }
            //printf("risposta child: %s\n", readbuffer_c);
            char * ris [MAX_ARGS];
            int arg_ris;
            tokenize(readbuffer_c, ris, &arg_ris);
            insertback(&processi, atoi(ris[0]), ris[1], p);
            //fflush(stdout);
        }

    }
    // FATTO
    else if (arg_counter == 2 && strcmp(words[0],"pclose") == 0){
        printf("Chiedo al processo %s di chiudersi\n", words[1]);
        kill_process(words[1]);
    }
    // FATTO
    else if (arg_counter == 2 && strcmp(words[0],"prmall") == 0){
        rmall_process(words[1]);
    }
    // FATTO
    else if (arg_counter == 1 && strcmp(words[0],"ptree") == 0){
        tree_process();
    }
    // FATTO
    else if (arg_counter == 1 && strcmp(words[0],"quit") == 0){
        killAll(&processi);
        exit(0);
    }
    else {
        printf(ANSI_COLOR_RED "Comando non riconosciuto."ANSI_COLOR_RESET" Digita \"phelp\" per la lista dei comandi disponibili.\n");
    }
}
