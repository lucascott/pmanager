#ifndef _INTLIST_H
#define _INTLIST_H

struct intlistitem {
        int pid;
        struct intlistitem *next;
};

typedef struct intlistitem intListitem;
struct intlist {
        intListitem *head;
};

// la libreria intlist è utilizzata per gestire i PID dei processi omonimi

typedef struct intlist intList;
void intinitlist (intList *);
void intinsertback(intList  *, int val);   
int intlength(intList);
void intdestroy(intList *);
int intgetitem(intList *ilist, int n);
void intprintlist(intList *ilist);
#endif  /* _INTLIST_H */
