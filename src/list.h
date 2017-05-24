#ifndef _LIST_H
#define _LIST_H

#include <sys/types.h>

struct listitem {
        char pname[100];
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


void initlist(List *ilist); //inizializza la lista
void insertfront(List *ilist, pid_t pid, char *name, pid_t ppid);  //inserisce item in testa
void insertback(List *ilist, pid_t pid, char *name, pid_t ppid, char *data); //inserisce item in coda
int length(List ilist); //ritorna la lunghezza della lista
void destroy(List *ilist); //cancella la lista
int rmallrec(List *ilist, char *name);
void rmallrecchild(List *ilist, Listitem *elemento, pid_t pid, Listitem *prec);
void treerecchild(Listitem *elemento, pid_t pid, int p, int pprec);
void printlist(List ilist);
pid_t getPidbyName (List *ilist, char *name);
void getInfos (List *ilist, char *name, pid_t *pid, pid_t *ppid, char *data);
int killAll(List *ilist);
int checkDuplicates(List *ilist, char *name, char *flag);
pid_t change_item_name (List *ilist, char *name, char * newname);


#endif  /* _LIST_H */
