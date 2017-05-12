#ifndef _LIST_H
#define _LIST_H

#include <sys/types.h>

struct listitem {
        char pname[100];
        pid_t pid;
        pid_t ppid;
        struct listitem *next;
};
typedef struct listitem Listitem;

struct list {
        Listitem *head;
};
typedef struct list List;



pid_t removebyname(List *ilist, char *name);
void printlist(List ilist);
void initlist (List *);              /* initialize an empty list */
void insertfront(List * , pid_t pid, char *name, pid_t ppid);  /* insert val at front */
//void insertback(List  *, pid_t pid, char name);   /* insert val at back */
int length(List);                    /* returns list length */
void destroy(List *);                /* deletes list */
//void setitem(List *, int n, pid_t pid, char name);/* modifies item at n to val*/
Listitem getitem(List, int n);            /* returns value at n*/
pid_t getPidbyName (List *ilist, char *name);
pid_t getPPidbyName (List *ilist, char *name);
int killAll(List *);
void getNamebyPid (List *ilist, pid_t pid, char * found);
pid_t change_item_name (List *ilist, char *name, char * newname);
#endif  /* _LIST_H */
