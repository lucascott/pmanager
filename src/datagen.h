#include <stdio.h>

#ifndef _DATAGEN_H
#define _DATAGEN_H

#define GENERA_MAX_C 10000
#define GENERA_MIN_C 9000
#define NCOMMANDS 9
#define STRLEN 100

void setProb(int numprocessi);
void normalizeProb(int numprocessi);
void normalizeExtr();
int pickOne();
void test(int numprocessi);
void chiamacomando(FILE *f, int numprocessi);

#endif
