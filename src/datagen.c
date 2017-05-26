#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "datagen.h"

char        commands[NCOMMANDS][STRLEN]; // lista comandi
float       prob[NCOMMANDS]; // probabilità lancio comando

char        **processi; // array processi
short       *proc_aperto; // array processi aperti / chiudersi
short       p_open = 0; // counter processi aperti


int main(int argc, char *argv[]) {
    srand(time(NULL));
    int numero_processi;

    if (argc != 2)
    {
        printf("Errore. Uso: datagen nome_file\n");
        exit(-1);
    }
    else {
        FILE *of;
        of = fopen(argv[1], "w");
        if (of == NULL) {
            printf("Errore apertura \"%s\"\n", argv[1]);
            exit(-1);
        }
        else
        {
            printf("Inserisci il numero di processi da creare: \n>> ");
            scanf("%d",&numero_processi);

            int i;
            processi = (char **)malloc(numero_processi * sizeof(char*));
            for(i = 0; i < numero_processi; i++) processi[i] = (char *)malloc(50 * sizeof(char));

            proc_aperto = (short *)malloc(numero_processi * sizeof(short));

        }

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

        {
            int i;
            int end = GENERA_MIN_C + (rand() % (GENERA_MAX_C - GENERA_MIN_C+1));
            for (i = 1; i < end; i++){
                //printf("%d\t",i);
                chiamacomando(of,numero_processi);
            }
            //printf("%d\tquit\n",i);
            fprintf(of,"quit\n");
        }
    }
    return 0;
}

void setProb(int numprocessi) { // funzione setup delle probabilità dinamiche assegnate a ciascun comando
    float AMP = 1.3;
    float amp = 0.05;
    prob[0] = numprocessi/2*amp;       // phelp
    prob[1] = numprocessi/2*amp;       // plist
    prob[2] = numprocessi - p_open;    // pnew
    prob[3] = p_open*amp;              // pinfo
    prob[4] = p_open * AMP;            // pclose
    prob[5] = p_open*amp;              // pspawn
    prob[6] = p_open * AMP;            // prmall
    prob[7] = p_open*amp;              // ptree
    prob[8] = 0;                       // quit
}

void chiamacomando(FILE *f, int numprocessi){
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
