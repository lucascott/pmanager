#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include "list.h"

void initlist(List *ilist) {
    ilist->head = 0;
}

void insertfront(List *ilist, pid_t pid, char *name, int ppid) {
    Listitem *newitem;
    newitem = (Listitem *)malloc(sizeof(Listitem));
    newitem->next = ilist->head;
    strcpy((newitem->pname), name);
    newitem->pid = pid;
    newitem->ppid = ppid;
    ilist->head = newitem;
}

/*void insertback(List *ilist, int val) {
Listitem *ptr;
Listitem *newitem;
newitem = (Listitem *)malloc(sizeof(Listitem));
newitem->data = val;
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
}*/

int length(List ilist){      /* returns list length */
    Listitem *ptr;
    int count = 1;
    if (!ilist.head) return 0;
    ptr = ilist.head;
    while (ptr->next)  {
        ptr = ptr->next;
        count++;
    }
    return count;
}

void destroy(List *ilist) {          /* deletes list */
    Listitem *ptr1,*ptr2;
    if (!ilist->head) return;  /* nothing to destroy */
    ptr1 = ilist->head;        /* destroy one by one */
    while (ptr1)  {
        ptr2 = ptr1;
        ptr1 = ptr1->next;
        free(ptr2);
    }
    ilist->head = 0;
}

/*void setitem(List *ilist, int n, int val){
/// modifies a value
// assume length is at least n long
Listitem *ptr;
int count = 0;
if (!ilist->head) return;
ptr = ilist->head;
for (count = 0;count < n;count ++)
{
if (ptr) ptr = ptr->next;
else return;
}
if (ptr)
ptr->data = val;
}*/

Listitem getitem(List ilist, int n) {
    /* returns a list value,
    * assume length is at least n long */
    Listitem *ptr;
    Listitem temp;
    temp.pid = -1;
    int count = 0;
    if (!ilist.head) return temp;
    ptr = ilist.head;
    if (n==0) {
        temp = *ptr;
        return temp;
    }
    while (ptr->next) {
        ptr = ptr->next;
        count++;
        if (n == count)
        {
            temp = *ptr;
            return temp;
        }
    }
    return temp;
}

void printlist(List ilist) {
    Listitem *ptr;
    if (!ilist.head) return;
    ptr = ilist.head;
    while (ptr->next != 0) {
        printf("%d\t %s\t %d\n",ptr->pid,ptr->pname, ptr->ppid);
        ptr = ptr->next;
    }
    printf("%d\t %s\t %d\n",ptr->pid,ptr->pname, ptr->ppid);
}

pid_t removebyname(List *ilist, char *name) {
    Listitem *ptr;
    Listitem *tmp;
    pid_t found = -1;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    if(strcmp(ptr->pname, name) == 0) {
        ilist->head = ptr->next;
        found = ptr->pid;
        free(ptr);
        return found;
    }
    while (ptr->next != 0) {
        if(strcmp((ptr->next)->pname, name) == 0) {
            found = (ptr->next)->pid;
            tmp = ptr->next;
            ptr->next = tmp->next;
            free(tmp);
            return found;
        }
        ptr = ptr -> next;
    }
    return -1;
}

pid_t getPidbyName (List *ilist, char *name) {
    Listitem *ptr;
    Listitem *tmp;
    pid_t found = -1;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    if(strcmp(ptr->pname, name) == 0) {
        found = ptr->pid;
        return found;
    }
    while (ptr->next != 0) {
        if(strcmp((ptr->next)->pname, name) == 0) {
            found = (ptr->next)->pid;
            return found;
        }
        ptr = ptr -> next;
    }
    return -1;
}

int killAll(List *ilist) {
    Listitem *ptr;
    Listitem *tmp;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    ilist->head = 0;
    kill(ptr->pid, SIGKILL);
    while (ptr->next != 0) {
        tmp = ptr->next;
        free(ptr);
        ptr->next = tmp->next;

        ptr = tmp;
        kill(ptr->pid, SIGKILL);
    }
    free (ptr);
    return 1;
}
