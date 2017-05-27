#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <signal.h>
#include "list.h"
#include "intlist.h"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void initlist(List *ilist) {
    ilist->head = 0;
    ilist->tail = 0;
}

void insertfront(List *ilist, pid_t pid, char *name, pid_t ppid) {
    Listitem *newitem;
    newitem = (Listitem *)malloc(sizeof(Listitem));
    newitem->next = ilist->head;
    strncpy((newitem->pname), name, 50);
    newitem->pname[50] = '\0';
    newitem->pid = pid;
    newitem->ppid = ppid;
    ilist->head = newitem;
    if (ilist->tail == 0) ilist->tail = ilist->head;
}

void insertback(List *ilist, pid_t pid, char *name, pid_t ppid, char *data) {
    Listitem *newitem;
    newitem = (Listitem *)malloc(sizeof(Listitem));
    strncpy((newitem->pname), name, sizeof(newitem->pname));
    newitem->pname[50] = '\0';
    newitem->pid = pid;
    newitem->ppid = ppid;
    strcpy(newitem->pdate,data);
    newitem->next = 0;
    if (ilist->tail == 0) {
        ilist->head = newitem;
        ilist->tail = newitem;
    }
    else {
        ilist->tail->next = newitem;
        ilist->tail = newitem;
    }
}

int length(List ilist){      /* returns list length */
    Listitem *ptr;
    int count = 1;
    if (!ilist.head) return 0;
    ptr = ilist.head;
    while (ptr->next) {
        ptr = ptr->next;
        count++;
    }
    return count;
}

void destroy(List *ilist) {
    Listitem *ptr1,*ptr2;
    if (!ilist->head) return;
    ptr1 = ilist->head;
    while (ptr1)  {
        ptr2 = ptr1;
        ptr1 = ptr1->next;
        free(ptr2);
    }
    ilist->head = 0;
    ilist->tail = 0;
}

int rmallrec(List *ilist, char *name){
    if (!ilist->head) return -1;
    int pid = checkDuplicates(ilist, name, "terminare a cascata");
    printf("Chiusura %s (pid: %d)\n",name, pid );
    Listitem * ptr = ilist->head;
    if(ptr->pid == pid && ptr->next == 0) { // elemento in testa
        ilist->head = 0;
        ilist->tail = 0;
        kill(pid, SIGTERM);
        free(ptr);
        return 0;
    }
    else if(ptr->pid == pid) { // elemento in testa

        rmallrecchild(ilist,ptr->next,pid,ptr);
        ilist->head = ptr->next;
        kill(pid, SIGTERM);
        free(ptr);
    }
    else {
        while ((ptr->next) != 0 && (ptr->next)-> pid != pid) {
            ptr = ptr->next;
        }
        Listitem * tmp = ptr->next;
        if (tmp->next == 0){ // se l'item da rimuovere è l'ultimo sposto il tail
            ilist->tail = ptr;
        }
        else{
            rmallrecchild(ilist, tmp->next,pid,tmp);
        }
        ptr->next = tmp->next;
        kill(tmp->pid, SIGTERM);
        free(tmp);
    }
    return 0;
}

void rmallrecchild( List *ilist, Listitem *elemento, pid_t pid, Listitem *prec){
    if (elemento->ppid == pid)
    {
        if (elemento->next != 0){
            rmallrecchild(ilist, elemento->next,elemento->pid, elemento); // cerco e killo i filgi
            rmallrecchild(ilist, elemento->next, pid, elemento);
        }
        else{
            ilist->tail = prec;
        }
        if (strcmp(elemento->pname, "XXX")!= 0) {
            printf("Chiusura figlio %s (pid: %d)\n", elemento->pname, elemento->pid);
            kill(elemento->pid, SIGTERM);
        }
        prec->next = elemento->next;
        free(elemento);
    }
    else if (elemento->next != 0){
        rmallrecchild(ilist, elemento->next, pid, elemento);
    }
}

void treerecchild(Listitem *elemento, pid_t pid, int p, int pprec){
    if (elemento->ppid == pid)
    {
        int i;
        for(i=0; i < p; i++){
            if (i == p-1) printf("└── ");
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
        printf("%d\t %-15s\t %d\t %s\n",ptr->pid,ptr->pname, ptr->ppid, ptr->pdate);
        ptr = ptr->next;
    }
    printf("%d\t %-15s\t %d\t %s\n",ptr->pid,ptr->pname, ptr->ppid, ptr->pdate);
}

pid_t getPidbyName (List *ilist, char *name) {
    Listitem *ptr;
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

void getInfos (List *ilist, char *name, pid_t *pid, pid_t *ppid, char *data) {
    int res = checkDuplicates(ilist, name, "visualizzare");
    Listitem *ptr;
    if (!ilist->head){
        *pid = -1;
        *ppid = -1;
        return;
    } //non trovato
    ptr = ilist->head;
    if(ptr->pid == res) {
        *pid = ptr->pid;
        *ppid = ptr->ppid;
        strcpy(data, ptr->pdate);
        return;
    }
    while (ptr->next != 0) {
        if((ptr->next)->pid == res) {
            *pid = (ptr->next)->pid;
            *ppid = (ptr->next)->ppid;
            strcpy(data, (ptr->next)->pdate);
            return;
        }
        ptr = ptr -> next;
    }
    *pid = -1;
    *ppid = -1;
    return;
}

int killAll(List *ilist) {
    Listitem *ptr;
    Listitem *tmp;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    ilist->head = 0;
    ilist->tail = 0;
    kill(ptr->pid, SIGTERM);
    while (ptr->next != 0) {
        tmp = ptr->next;
        free(ptr);
        ptr = tmp;
        kill(ptr->pid, SIGTERM);
    }
    free (ptr);
    return 1;
}

int myIsDigit(char *str){
    int i;
    int len = strlen(str);
    if (atoi(str)){
        return 1;
    }
    for (i = 0; i < len; i++){
        char c = str[i];
        if(!isdigit(c)){
            return 0;
        }
    }
    return 1;
}

int checkDuplicates(List *ilist, char *name, char *flag){
    Listitem *ptr;
    int t;
    intList temp;
    intinitlist(&temp);
    int ref = -1;
    if (!ilist->head) {
        return ref;
    }
    ptr = ilist->head;
    if (strcmp(ptr->pname, name) == 0) {
        t = (int) ptr->pid;
        intinsertback(&temp, t);
    }
    while (ptr->next != 0) {
        if(strcmp((ptr->next)->pname, name) == 0) {
            t = (int) (ptr->next)->pid;
            intinsertback(&temp, t);
        }
        ptr = ptr -> next;
    }
    if (intlength(temp) == 0) return -1;
    else if (intlength(temp) == 1) {
        intdestroy(&temp);
        ref = t;
    }
    else {
        char n[10];
        int len = intlength(temp);
        printf("Rilevati %d processi chiamati \"%s\" aperti:\n", len, name);
        intprintlist(&temp);
        printf("Inserisci indice processo da %s:\n",flag);
        int valid = 0;
        int num = -1;
        do{
            printf("\r>> ");
            scanf("%s",n);
            fflush(stdin);
            if (!myIsDigit(n)){
                printf(ANSI_COLOR_RED"\rErrore: caratteri non numerici non sono ammessi. Riprovare...\n"ANSI_COLOR_RESET );
            }
            else {
                num = atoi(n);
                if (num >= len || num < 0){
                    printf(ANSI_COLOR_RED"\rErrore: indice non presente in lista. Riprovare...\n"ANSI_COLOR_RESET );
                }
                else{
                    valid = 1;
                }
            }
        } while (!valid);
        ref = intgetitem(&temp, num);
    }
    return ref;
}

pid_t change_item_name (List *ilist, char *name, char * newname){
    Listitem *ptr;
    pid_t found;
    int ref = checkDuplicates(ilist, name, "terminare");
    if (ref == -1) return -1;
    if (!ilist->head) return -1;
    ptr = ilist->head;
    if(ptr->pid == ref) {
        found = ptr->pid;
        strcpy(ptr->pname, newname);
        return found;
    }
    while (ptr->next != 0) {
        if((ptr->next)->pid == ref) {
            found = (ptr->next)->pid;
            strcpy((ptr->next)->pname, newname);
            return found;
        }
        ptr = ptr -> next;
    }
    return -1;
}

int numActive(List *ilist) {
    Listitem *ptr;
    int count = 0;
    if (!ilist->head) return 0;
    ptr = ilist->head;
    if (strcmp((ptr->pname), "XXX") != 0) {
        count++;
    }
    while (ptr->next) {
        ptr = ptr->next;
        if (strcmp((ptr->pname), "XXX") != 0) {
            count++;
        }
    }
    return count;
}
