#ifndef _DATAGEN_H
#define _DATAGEN_H

#include <stdio.h>

#define NCOMMANDS 9
#define STRLEN 100

// RANGE DI NUMERO COMANDI DA GENERARE
#define GENERA_MAX_C 10000
#define GENERA_MIN_C 9000


//COLORE ERRORI SHELL
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void setProb(int numprocessi);                  // funzione setup delle probabilità dinamiche assegnate a ciascun comando
void chiamacomando(FILE *f, int numprocessi);   // funzione che genera un comando casuale e lo stampa nel file di output passato come argomento
void normalizeProb(int numprocessi);            // funzione di normalizzazione delle probabilità (sommma totale 100%)
int pickOne();                                  // funzione che seleziona un indice casuale basato sulle probabilità assegnate a ogni comando
int myIsDigit(char *str);                       // funzione che controlla se la stringa è un numero
void test(int numprocessi, int n_prove);        // funzione di test aleatorio. Genera un test casuale di scelta e stampa le probabilità attese con quelle otttenute
void normalizeExtr(float *extr);                // normalizza probabilità per modalità test
void printProb(int numprocessi, int aperti);    // stampa probabilità processi nel dato momento
#endif
