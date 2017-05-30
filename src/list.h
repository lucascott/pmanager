#ifndef _LIST_H
#define _LIST_H

#include <sys/types.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_INV     "\x1b[30;47m"
#define ANSI_COLOR_RESET   "\x1b[0m"

struct listitem {
        char pname[50];
        pid_t pid;
        pid_t ppid;
        char pdate[13];
        struct listitem *next;
};
typedef struct listitem Listitem;

struct list {
        Listitem *head;
        Listitem *tail;
};
typedef struct list List;


void initlist(List *ilist); // inizializza la lista
void insertback(List *ilist, pid_t pid, char *name, pid_t ppid, char *data); // inserisce item in coda
int length(List ilist); // ritorna la lunghezza della lista
void destroy(List *ilist); // cancella la lista
int rmallrec(List *ilist, char *name); // elimina processo <name> e lancia la rimozione sui figli
void rmallrecchild(List *ilist, Listitem *elemento, pid_t pid, Listitem *prec); // rimuove ricorsivamente i processi figli di <elemento>
void treerecchild(Listitem *elemento, pid_t pid, int p, int pprec); // stampo ricorsivamente l'albero con indentazione
void printlist(List ilist); // stampa la lista di processi
pid_t getPidbyName (List *ilist, char *name); // ritorna il PID del processo <name>
void getInfos (List *ilist, char *name, pid_t *pid, pid_t *ppid, char *data); // ritorna informazioni sul processo <name>
int killAll(List *ilist); // killa i processi e dealloco l'intera lista
int myIsDigit(char *str); // controlla se un input è un numero
int checkDuplicates(List *ilist, char *name, char *flag); // gestisce l'omonimia dei processi
pid_t change_item_name (List *ilist, char *name, char * newname); //cambia il <name> del proceso in <newname> (XXX)
int numActive(List *ilist, char *str); // restituscisce il numero di processi attivi e in <str> la stringa formattata

#endif  /* _LIST_H */
