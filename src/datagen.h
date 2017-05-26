#include <stdio.h>

#ifndef _DATAGEN_H
#define _DATAGEN_H

#define GENERA_MAX_C 10000
#define GENERA_MIN_C 9000
#define NCOMMANDS 9
#define STRLEN 100

void setProb(int numprocessi);                  // funzione setup delle probabilità dinamiche assegnate a ciascun comando
void chiamacomando(FILE *f, int numprocessi);
void normalizeProb(int numprocessi);            // funzione di normalizzazione delle probabilità (sommma totale 100%)
int pickOne();                                  // funzione che seleziona un indice casuale basato sulle probabilità assegnate a ogni comando
void test(int numprocessi, int n_prove);        // funzione di test aleatorio. Genera un test casuale di scelta e stampa le probabilità attese con quelle otttenute
void normalizeExtr(float *extr);                // normalizza probabilità per modalità test

#endif
