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
void intinsertback(intList  *, int val);   /* insert val at back */
int intlength(intList);                    /* returns list length */
void intdestroy(intList *);                /* deletes list */
int intgetitem(intList *ilist, int n);            /* returns value at n*/
void intprintlist(intList *ilist);
#endif  /* _INTLIST_H */
