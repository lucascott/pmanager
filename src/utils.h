#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"

// DEFINIZIONE COSTANTI
#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

#define READ 0
#define WRITE 1

// COLORE ERRORI SHELL
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_INV     "\x1b[30;47m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// FIRME FUNZIONI
int isCommandWithParam(char * str); // controlla se il comando str richiede parametri
void getDate(char *data); // salva la data e l'ora su data
void print_list(); // stampa la lista processi
void tokenize(char * line, char ** tokens, int *argc); // divide la stringa in singole parole
void getMyPid(char * mystrpid); // salva in mystrpid il PID del processo che invoca la funzione
void handler (int signo); // handler SIGUSR1
void new_process(char *nome); // crea un nuovo processo chiamato "nome"
void info_process(char *nome); // stampa informazioni sul processo "nome"
void spawn_process(char *nome); // chiede al processo "nome" di clonarsi
void kill_process(char* nome); // chiude il processo "nome"
void rmall_process(char* nome); // termina a cascata processo "nome" e relativi cloni
void tree_process(); // stampa albero processi
void esegui(char *words[MAX_ARGS], int arg_counter); // interpreta ed esegue i comandi utente


#endif  /* _UTILS_H */
