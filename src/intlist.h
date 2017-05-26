#ifndef _INTLIST_H
#define _INTLIST_H
struct intlistitem {
        int data;
        struct intlistitem *next;
};
typedef struct intlistitem intListitem;
struct intlist {
        intListitem *head;
};
typedef struct intlist intList;
void intinitlist (intList *);
void intinsertback(intList  *, int val);   
int intlength(intList);
void intdestroy(intList *);
int intgetitem(intList *ilist, int n);
void intprintlist(intList *ilist);
#endif  /* _INTLIST_H */
