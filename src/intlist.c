#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include "intlist.h"

void intinitlist(intList *ilist) {
  ilist->head = 0;
}

void intinsertback(intList *ilist, int val) {
   intListitem *ptr;
   intListitem *newitem;
   newitem = (intListitem *)malloc(sizeof(intListitem));
   newitem->pid = val;
   newitem->next = 0;
   if (!ilist->head) {
        ilist->head = newitem;
        return;
   }
   ptr = ilist->head;
   while (ptr->next)
   {
       ptr = ptr->next;
   }
   ptr->next = newitem;
}

int intlength(intList ilist) {
   intListitem *ptr;
   int count = 1;
   if (!ilist.head) return 0;
   ptr = ilist.head;
   while (ptr->next)  {
        ptr = ptr->next;
        count++;
   }
   return count;
}

void intdestroy(intList *ilist) {
   intListitem *ptr1,*ptr2;
   if (!ilist->head) return;
   ptr1 = ilist->head;
   while (ptr1)  {
        ptr2 = ptr1;
        ptr1 = ptr1->next;
        free(ptr2);
   }
   ilist->head = 0;
}

int intgetitem(intList *ilist, int n) {
   intListitem *ptr;
   int count = 0;
   if (!ilist->head) return 0;
   ptr = ilist->head;
   if (n==0) return ptr->pid;
   while (ptr->next) {
        ptr = ptr->next;
        count++;
        if (n == count)
             return (ptr->pid);
   }
   return 0;
}

void intprintlist(intList *ilist) {
  intListitem *ptr;
  int n;
  int count = 0;
  if (!ilist->head) return;
  ptr = ilist->head;
  do {
    n = ptr->pid;
    printf("%d: pid %d\n", count, n);
    count++;
    ptr = ptr-> next;
  } while (ptr->next != 0);
  n = ptr->pid;
  printf("%d: pid %d\n", count, n);
}
