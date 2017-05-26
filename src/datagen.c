#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>


#define GENERA_MAX_C 10000
#define GENERA_MIN_C 9000
#define MAXPROCESSI 1000


#define NCOMMANDS 9
#define STRLEN 100

char        commands[NCOMMANDS][STRLEN]; // lista comandi
float       prob[NCOMMANDS]; // probabilità lancio comando

char        processi[MAXPROCESSI][STRLEN]; // lista processi
short       proc_aperto[MAXPROCESSI]; // lista processi aperti
short       p_open = 0; // counter processi aperti

 // per testing aleatorio
float       extr[NCOMMANDS];



void setProb() {
    float AMP = 1.3;
    float amp = 0.05;
    prob[0] = MAXPROCESSI/2*amp;       // phelp
    prob[1] = MAXPROCESSI/2*amp;       // plist
    prob[2] = MAXPROCESSI - p_open;    // pnew
    prob[3] = p_open*amp;              // pinfo
    prob[4] = p_open * AMP;            // pclose
    prob[5] = p_open*amp;              // pspawn
    prob[6] = p_open * AMP;            // prmall
    prob[7] = p_open*amp;              // ptree
    prob[8] = 0;                       // quit
}

void normalizeProb(){
    setProb();
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

void test(){
    normalizeProb();
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

void chiamacomando(){
    char    comando[STRLEN*2];
    int     r;
    int     r2;

    normalizeProb();
    r = pickOne();
    strcpy(comando, commands[r]);
    strcat(comando, " ");
    if (r == 2) { // PNEW
        do {
            r2 = rand() % MAXPROCESSI;
            //printf("%d\n", r2);
        } while (proc_aperto[r2]);
        proc_aperto[r2] = 1;
        p_open++;

        strcat(comando, processi[r2]);
        printf("\t\t%s\n", comando);
    }
    else if (r == 3 || r == 4 || r == 5 || r == 6) {
        // concateno nome processo
        do {
            r2 = rand() % MAXPROCESSI;
            //printf("%d\n", r2);
        } while (!proc_aperto[r2]);
        if (r == 4 || r == 6){
            proc_aperto[r2] = 0;
            p_open--;
        }
        strcat(comando, processi[r2]);
        printf("\t%s\n", comando);
    }
    else if (r != -1){
        printf("%s\n", comando);
    }
}

int main(){
    srand(time(NULL));   // should only be called once
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
        for (i = 0; i < MAXPROCESSI; i++) {
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
            chiamacomando();
        }
        //printf("%d\tquit\n",i);
        printf("quit\n");
    }

    return 0;
}
