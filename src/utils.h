#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/types.h>
//#include <prctl.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"

#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

#define READ 0
#define WRITE 1

int isCommandWithParam(char * str);
void getDate(char *data);
void print_list();
void tokenize(char * line, char ** tokens, int *argc);
void getMyPid(char * mystrpid);
int alphanumeric(char * str);
void handler (int signo);
void new_process(char *nome);
void info_process(char *nome);
void kill_process(char* nome);
void rmall_process(char* nome);
void tree_process();
void esegui(char *words[MAX_ARGS], int arg_counter);


#endif  /* _UTILS_H */
