#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "datagen.h"

char        commands[NCOMMANDS][STRLEN]; // lista comandi
float       prob[NCOMMANDS]; // probabilità lancio comando

char        **processi; // array processi
short       *proc_aperto; // array processi aperti / chiudersi
short       p_open = 0; // counter processi aperti


int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc != 2) // controllo correttezza d'uso del tool datagen
    {
        printf(ANSI_COLOR_RED"Errore sintassi. Uso: datagen nome_file\n"ANSI_COLOR_RESET);
        exit(-1);
    }
    else {
        // apro file in scrittura
        FILE *of;
        of = fopen(argv[1], "w");
        printf("Apertura di \"%s\" avvenuta con successo.\n", argv[1]);
        if (of == NULL) {
            printf(ANSI_COLOR_RED"Errore in fase di apertura di \"%s\"\n"ANSI_COLOR_RESET, argv[1]);
            exit(-1);
        }

        // chiedo all'utente il numero di processi desiderati
        char n[STRLEN];
        int valid = 0;
        int numero_processi = -1;
        printf("Inserisci il numero di processi da creare:\n");
        do {
            printf("\r>> ");
            scanf("%s",n);
            if (!myIsDigit(n)){
                printf(ANSI_COLOR_RED"Errore: caratteri non numerici non sono ammessi. Riprovare...\n"ANSI_COLOR_RESET);
            }
            else {
                numero_processi = atoi(n);
                if (numero_processi < 0){
                    printf(ANSI_COLOR_RED"Errore: valori negativi non ammessi. Riprovare...\n"ANSI_COLOR_RESET);
                }
                else{
                    valid = 1;
                }
            }
        } while (!valid);

        // allocazione spazio per array processi e processi aperti
        int i;
        processi = (char **)malloc(numero_processi * sizeof(char*));
        for(i = 0; i < numero_processi; i++) processi[i] = (char *)malloc(50 * sizeof(char));
        proc_aperto = (short *)malloc(numero_processi * sizeof(short));

        // dichiarazione array comandi
        {
            strcpy(commands[0], "phelp");
            strcpy(commands[1], "plist");
            strcpy(commands[2], "pnew");
            strcpy(commands[3], "pinfo");
            strcpy(commands[4], "pclose");
            strcpy(commands[5], "pspawn");
            strcpy(commands[6], "prmall");
            strcpy(commands[7], "ptree");
            strcpy(commands[8], "quit");
        }

        // popola array dei processi
        {
            int i;
            for (i = 0; i < numero_processi; i++) {
                char num[STRLEN];
                sprintf(num,"%d",i);
                strcpy(processi[i], "proc");
                strcat(processi[i],num);
                proc_aperto[i] = 0;
                //printf("%s - %d\n", processi[i], proc_aperto[i]);
            }
            //printf("==========\n");
        }

        // TEST FUNCTION
        //test(numero_processi, 1000);
        int end = GENERA_MIN_C + (rand() % (GENERA_MAX_C - GENERA_MIN_C+1));
        { // genera n comandi tra GENERA_MAX_C e GENERA_MIN_C
            int i;
            for (i = 1; i < end; i++){
                chiamacomando(of,numero_processi);
                //printProb(numero_processi, p_open);
            }
            fprintf(of,"quit\n");
            end++;
        }
        fclose(of);
        printf("Creazione di %d comandi avvenuta con successo\n", end);
    }
    return 0;
}

void setProb(int numprocessi) { // funzione setup delle probabilità dinamiche assegnate a ciascun comando
    prob[0] = numprocessi/2 * 0.1;       // phelp
    prob[1] = numprocessi/2 * 0.1;       // plist
    prob[2] = numprocessi - p_open;    // pnew
    prob[3] = p_open * 0.1;              // pinfo
    prob[4] = p_open * 0.8;            // pclose
    prob[5] = p_open * 1;                  // pspawn
    prob[6] = p_open * 0.8;            // prmall
    prob[7] = numprocessi/2 * 0.1;             // ptree
    prob[8] = 0;                       // quit
}

void chiamacomando(FILE *f, int numprocessi) { // funzione che genera un comando casuale e lo stampa nel file di output passato come argomento
    char    comando[STRLEN*2];
    int     r;
    int     r2;

    normalizeProb(numprocessi);
    r = pickOne();
    strcpy(comando, commands[r]);
    strcat(comando, " ");
    if (r == 2) { // PNEW
        do {
            r2 = rand() % numprocessi;
            //printf("%d\n", r2);
        } while (proc_aperto[r2]);
        proc_aperto[r2] = 1;
        p_open++;

        strcat(comando, processi[r2]);
        fprintf(f,"\t\t%s\n", comando);
    }
    else if (r == 3 || r == 4 || r == 5 || r == 6) {
        // concateno nome processo
        do {
            r2 = rand() % numprocessi;
            //printf("%d\n", r2);
        } while (!proc_aperto[r2]);
        if (r == 4 || r == 6){
            proc_aperto[r2] = 0;
            p_open--;
        }
        strcat(comando, processi[r2]);
        fprintf(f,"\t%s\n", comando);
    }
    else if (r != -1){
        fprintf(f,"%s\n", comando);
    }
}

void normalizeProb(int numprocessi) { // funzione di normalizzazione delle probabilità (sommma totale 100%)
    setProb(numprocessi);
    float sum = 0;
    int i;
    for (i = 0; i < NCOMMANDS; i++){
        sum += prob[i];
    }
    for (i = 0; i < NCOMMANDS; i++){
        prob[i] = prob[i]/sum*100;
    }
}

int pickOne() { // funzione che seleziona un indice casuale basato sulle probabilità assegnate a ogni comando
    int index = 0;
    float r = rand() % 100;
    while(r > 0){
        r = r - prob[index];
        index++;
    }
    index--;
    return index;
}

int myIsDigit(char *str) { // funzione che controlla se la stringa è un numero
    int i;
    int len = strlen(str);
    if (atoi(str)){
        return 1;
    }
    for (i = 0; i < len; i++){
        char c = str[i];
        if(!isdigit(c)){
            return 0;
        }
    }
    return 1;
}

void normalizeExtr(float *extr) { // normalizza probabilità per modalità test
    float sum = 0;
    int i;
    for (i = 0; i < NCOMMANDS; i++){
        sum += extr[i];
    }
    for (i = 0; i < NCOMMANDS; i++){
        extr[i] = extr[i]/sum*100;
    }
}

void test(int numprocessi, int n_prove) { // funzione di test aleatorio. Genera un test casuale di scelta e stampa le probabilità attese con quelle otttenute
    float extr[NCOMMANDS];  // per testing aleatorio
    {
        int i;
        for(i = 0; i < NCOMMANDS; i++){
            extr[i] = 0;
        }
    }
    normalizeProb(numprocessi);
    // eseguo esperimento aleatorio
    int index;
    int i;
    for (i = 0; i < n_prove; i++) {
        index = pickOne();
        extr[index]++;
    }
    // visualizzo probabilità ottenute
    normalizeExtr(extr);
    printf("COMANDO\t\tPROBABILITA'\tTEST ALEATORIO (prove totali: %d)\n", n_prove);
    for (i = 0; i < NCOMMANDS; i++) {
        if (strcmp(commands[i],"pnew") == 0 || strcmp(commands[i],"quit") == 0)
            printf("c: %s\t\t%f\t%f\n", commands[i], prob[i], extr[i]);
        else
            printf("c: %s\t%f\t%f\n", commands[i], prob[i], extr[i]);
    }
}

void printProb(int numprocessi, int aperti) { // stampa probabilità processi nel dato momento
    int i;
    printf("COMANDO\t\tPROBABILITA' - aperti: %d su %d\n",aperti,numprocessi);
    for (i = 0; i < NCOMMANDS; i++) {
        if (strcmp(commands[i],"pnew") == 0 || strcmp(commands[i],"quit") == 0)
            printf("c: %s\t\t%f\n", commands[i], prob[i]);
        else
            printf("c: %s\t%f\n", commands[i], prob[i]);
    }
}
