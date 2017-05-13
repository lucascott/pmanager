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
        while ((ptr->next)-> pid != pid  && (ptr->next) != 0) {
            ptr = ptr->next;
        }
        Listitem * tmp = ptr->next;
        printf("pid trovato: %d (%s)\n", tmp->pid, tmp->pname);
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
        printf("Analizzando elemento: %s (pid: %d, ppid: %d)\n", elemento->pname, elemento->pid, elemento->ppid);
        if (elemento->next != 0){
            printf("richiamo su succ %s\n",(elemento->next)->pname);
            rmallrecchild(elemento->next, pid, elemento);
            rmallrecchild(elemento->next,elemento->pid, elemento); // cerco e killo i filgi

            //printf("richiamo su prec succ %s\n",(prec->next)->pname);
            //rmallrecchild(prec->next, pid, prec);
        }
        kill(elemento->pid, SIGTERM);
        prec->next = elemento->next;
        //printf("%s - next: %s\n", prec->pname, (prec->next)->pname);
        free(elemento);
    }
    else{
        printf("Salto elemento: %s (pid: %d, ppid: %d)\n", elemento->pname, elemento->pid, elemento->ppid);
        if (elemento->next != 0){
            rmallrecchild(elemento->next, pid, elemento);
        }
    }


}


void treerecchild(Listitem *elemento, pid_t pid, int p, int pprec){
    if (elemento->ppid == pid)
    {
        int i;
        for(i=0; i < p; i++){
            printf("   ");
        }
        printf("%s\n", elemento->pname);
        if (elemento->next != 0){
            //printf("richiamo su succ %s\n",(elemento->next)->pname);
            treerecchild(elemento->next,elemento->pid, p+1,p); // cerco e killo i figli
            treerecchild(elemento->next, pid, p,p);

            //printf("richiamo su prec succ %s\n",(prec->next)->pname);
            //rmallrecchild(prec->next, pid, prec);
        }
        //printf("%s - next: %s\n", prec->pname, (prec->next)->pname);
    }
    else{
        //printf("Salto elemento: %s (pid: %d, ppid: %d)\n", elemento->pname, elemento->pid, elemento->ppid);
        if (elemento->next != 0){
            treerecchild(elemento->next, pid, p,p);
        }
    }


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
/*
pid_t removebyname(List *ilist, char *name) {
    Listitem *ptr;
    Listitem *tmp;
    pid_t found = -1;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    if(strcmp(ptr->pname, name) == 0) {
        ilist->head = ptr->next;
        found = ptr->pid;
        pid_t res = killbyppid(ilist, ptr->pid); // uccide i figli
        printf("figli di %d = %d\n",found, res );
        kill(found, SIGTERM);
        free(ptr);
        return found;
    }
    while (ptr->next != 0) {
        if(strcmp((ptr->next)->pname, name) == 0) {
            found = (ptr->next)->pid;
            pid_t res = killbyppid(ilist, (ptr->next)->pid); // uccide i figli
            printf("figli di %d = %d\n",found, res );
            kill(found, SIGTERM);
            tmp = ptr->next;
            ptr->next = tmp->next;
            free(tmp);
            return found;
        }
        ptr = ptr -> next;
    }
    return -1;
}

pid_t killbyppid(List *ilist, pid_t ppid) {
    Listitem *ptr;
    Listitem *tmp;
    pid_t found = -1;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    if(ptr->ppid == ppid) {
        ilist->head = ptr->next;
        found = ptr->pid;
        pid_t res = killbyppid(ilist, ptr->pid); // uccide i figli
        printf("figli di %d = %d\n",found, res );
        kill(found, SIGTERM);
        free(ptr);
        return found;
    }
    while (ptr->next != 0) {
        if((ptr->next)->ppid == ppid) {
            found = (ptr->next)->pid;
            pid_t res = killbyppid(ilist, (ptr->next)->pid); // uccide i figli
            printf("figli di %d = %d\n",found, res );
            kill(found, SIGTERM);
            tmp = ptr->next;
            ptr->next = tmp->next;
            free(tmp);
            return found;
        }
        ptr = ptr -> next;
    }
    return -1;
}

*/

pid_t removebyname(List *ilist, char *name) {
    Listitem *ptr;
    Listitem *tmp;
    pid_t found = -1;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    if(strcmp(ptr->pname, name) == 0) {
        ilist->head = ptr->next;
        found = ptr->pid;
        pid_t res = killbyppid(ilist, ptr->pid); // uccide i figli
        printf("figli di %d = %d\n",found, res );
        kill(found, SIGTERM);
        free(ptr);
        return found;
    }
    while (ptr->next != 0) {
        if(strcmp((ptr->next)->pname, name) == 0) {
            found = (ptr->next)->pid;
            pid_t res = killbyppid(ilist, (ptr->next)->pid); // uccide i figli
            printf("figli di %d = %d\n",found, res );
            kill(found, SIGTERM);
            tmp = ptr->next;
            ptr->next = tmp->next;
            free(tmp);
            return found;
        }
        ptr = ptr -> next;
    }
    return -1;
}

pid_t killbyppid(List *ilist, pid_t ppid) {
    Listitem *ptr;
    Listitem *tmp;
    pid_t found = -1;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    if(ptr->ppid == ppid) {
        ilist->head = ptr->next;
        found = ptr->pid;
        pid_t res = killbyppid(ilist, ptr->pid); // uccide i figli
        printf("figli di %d = %d\n",found, res );
        kill(found, SIGTERM);
        free(ptr);
        return found;
    }
    while (ptr->next != 0) {
        if((ptr->next)->ppid == ppid) {
            found = (ptr->next)->pid;
            pid_t res = killbyppid(ilist, (ptr->next)->pid); // uccide i figli
            printf("figli di %d = %d\n",found, res );
            kill(found, SIGTERM);
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

pid_t getPPidbyName (List *ilist, char *name) {
    Listitem *ptr;
    Listitem *tmp;
    int found = -1;
    if (!ilist->head) return -1;
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


void getNamebyPid (List *ilist, pid_t pid, char * found) {
    Listitem *ptr;
    Listitem *tmp;
    if (!ilist->head){
        strcpy(found,"");
        return;
    }
    ptr = ilist->head;
    if(ptr->pid == pid) {
        printf("trovato %s\n",  ptr->pname );
        strcpy(found, ptr->pname);
        return;
    }
    while (ptr->next != 0) {
        if(ptr->pid == pid) {
            printf("trovato %s\n", (ptr->next)->pname );
            strcpy(found, (ptr->next)->pname);
            return;
        }
        ptr = ptr -> next;
    }
    strcpy(found,"");
    return;
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
