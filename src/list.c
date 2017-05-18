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

void insertfront(List *ilist, pid_t pid, char *name, pid_t ppid) {
    Listitem *newitem;
    newitem = (Listitem *)malloc(sizeof(Listitem));
    newitem->next = ilist->head;
    strcpy((newitem->pname), name);
    newitem->pid = pid;
    newitem->ppid = ppid;
    ilist->head = newitem;
}

void insertback(List *ilist, pid_t pid, char *name, pid_t ppid) {
    Listitem *ptr;
    Listitem *newitem;
    newitem = (Listitem *)malloc(sizeof(Listitem));
    strcpy((newitem->pname), name);
    newitem->pid = pid;
    newitem->ppid = ppid;
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

int rmallrec(List *ilist, char *name){
    if (!ilist->head) return -1;
    pid_t pid = getPidbyName(ilist,name);
    printf("Chiusura %s (pid: %d)\n",name, pid );
    Listitem * ptr = ilist->head;
    if(ptr->pid == pid && ptr->next == 0) { // elemento in testa lista vuota
        ilist->head = 0;
        kill(pid, SIGTERM);
        free(ptr);
        return 0;
    }
    else if(ptr->pid == pid) { // elemento in testa

        rmallrecchild(ptr->next,pid,ptr);
        ilist->head = ptr->next;
        kill(pid, SIGTERM);
        free(ptr);
    }
    else {
        while ((ptr->next) != 0 && (ptr->next)-> pid != pid) {
            ptr = ptr->next;
        }
        Listitem * tmp = ptr->next;
        rmallrecchild(tmp->next,pid,tmp);
        ptr->next = tmp->next;
        kill(tmp->pid, SIGTERM);
        free(tmp);
    }
    return 0;
}

void rmallrecchild(Listitem *elemento, pid_t pid, Listitem *prec){
    if (elemento->ppid == pid)
    {
        if (elemento->next != 0){
            rmallrecchild(elemento->next,elemento->pid, elemento); // cerco e killo i filgi
            rmallrecchild(elemento->next, pid, elemento);
        }
        printf("Chiusura figlio %s (pid: %d)\n", elemento->pname, elemento->pid);
        kill(elemento->pid, SIGTERM);
        prec->next = elemento->next;
        free(elemento);
    }
    else if (elemento->next != 0){
        rmallrecchild(elemento->next, pid, elemento);
    }
}

void treerecchild(Listitem *elemento, pid_t pid, int p, int pprec){
    if (elemento->ppid == pid)
    {
        int i;
        for(i=0; i < p; i++){
            if (i == p-1) printf("+-- ");
            else printf("│   ");
        }
        printf("%s\n", elemento->pname);
        if (elemento->next != 0){
            treerecchild(elemento->next,elemento->pid, p+1,p); // cerco e killo i figli
            treerecchild(elemento->next, pid, p,p);
        }
    }
    else if (elemento->next != 0){
        treerecchild(elemento->next, pid, p,p);
    }
}

void printlist(List ilist) {
    Listitem *ptr;
    if (!ilist.head) return;
    ptr = ilist.head;
    while (ptr->next != 0) {
        printf("%d\t %-15s\t %d\n",ptr->pid,ptr->pname, ptr->ppid);
        ptr = ptr->next;
    }
    printf("%d\t %-15s\t %d\n",ptr->pid,ptr->pname, ptr->ppid);
}

pid_t getPidbyName (List *ilist, char *name) {
    Listitem *ptr;
    Listitem *tmp;
    pid_t found = -1;
    if (!ilist->head) return -1;    //non trovato
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

pid_t getPPidbyName (List *ilist, char *name) {
    Listitem *ptr;
    Listitem *tmp;
    int found = -1;
    if (!ilist->head) return -1;    //non trovato
    ptr = ilist->head;
    if(strcmp(ptr->pname, name) == 0) {
        found = ptr->ppid;
        return found;
    }
    while (ptr->next != 0) {
        if(strcmp((ptr->next)->pname, name) == 0) {
            found = (ptr->next)->ppid;
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
    kill(ptr->pid, SIGTERM);
    while (ptr->next != 0) {
        tmp = ptr->next;
        free(ptr);
        ptr->next = tmp->next;

        ptr = tmp;
        kill(ptr->pid, SIGTERM);
    }
    free (ptr);
    return 1;
}

pid_t change_item_name (List *ilist, char *name, char * newname){
    Listitem *ptr;
    Listitem *tmp;
    pid_t found = -1;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    if(strcmp(ptr->pname, name) == 0) {
        found = ptr->pid;
        strcpy(ptr->pname, newname);
        return found;
    }
    while (ptr->next != 0) {
        if(strcmp((ptr->next)->pname, name) == 0) {
            found = (ptr->next)->pid;
            strcpy((ptr->next)->pname, newname);
            return found;
        }
        ptr = ptr -> next;
    }
    return -1;
}