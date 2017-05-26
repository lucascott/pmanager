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

float       extr[NCOMMANDS];  // per testing aleatorio

int main(int argc, char *argv[]){
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
        //test();

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


void setProb(int numprocessi) {
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

void normalizeProb(int numprocessi){
    setProb(numprocessi);
    float sum = 0;
    int i;
    for (i = 0; i < NCOMMANDS; i++)
        sum += prob[i];
    for (i = 0; i < NCOMMANDS; i++)
        prob[i] = prob[i]/sum*100;
}

void normalizeExtr(){
    float sum = 0;
    int i;
    for (i = 0; i < NCOMMANDS; i++)
        sum += extr[i];
    for (i = 0; i < NCOMMANDS; i++)
        extr[i] = extr[i]/sum*100;
}

int pickOne(){
    int index = 0;
    float r = rand() % 100;
    while(r > 0){
        r = r - prob[index];
        index++;
    }
    index--;
    return index;
}

void test(int numprocessi){
    normalizeProb(numprocessi);
    // probabilità attese
    int i;
    for (i = 0; i < NCOMMANDS; i++){
        printf("c: %s\t%f\n", commands[i],prob[i] );
        extr[i] = 0;
    }
    // eseguo esperimento aleatorio
    int index;
    i = 0;
    while (i < 10000) {
        index = pickOne();
        extr[index]++;
        i++;
    }
    // visualizzo probabilità ottenute
    normalizeExtr();
    for (i = 0; i < NCOMMANDS; i++)
        printf("c: %s\t%f\n", commands[i], extr[i] );
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
