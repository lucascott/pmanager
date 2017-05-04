#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <signal.h>
#include "list.h"

#define MAX_ARGS 256
#define MAX_LINE_SIZE 1024

List processi;

char    	line[1024];
char    	*pch[MAX_ARGS];
int     	pcount = 0;

int     	fd[2], nbytes;
char		readbuffer[80];

/// FUNZIONI

void print_list()
{
	printf("%d processi figli attivi\n", length(processi));
    printf("PID\t NAME\n=================\n");
	printlist(processi);
}

void new_process(char* nome)
{
	printf("Creazione del processo %s\n", nome);
    pid_t pid = fork();
	insertfront(&processi, pid, nome);
	if (getitem(processi, 0).pid < 0) {
		printf("Failed to fork process\n");
		exit(1);
	}
	if (getitem(processi, 0).pid == 0){ // child process

		while(1)
		{
			close(fd[1]);
			/* Read in a string from the pipe */
			nbytes = read(fd[0], readbuffer, sizeof(readbuffer));

			if (nbytes != -1){
				printf("Received string: %s\n", readbuffer);

				char * c_pch[MAX_ARGS];
				int c_argc = 0; // reinizializzo numero parole inserite
	            c_pch[c_argc] = strtok(readbuffer," ,.-\n\t");
	            while (c_pch[c_argc] != NULL) {
	                c_argc++;
	                //printf ("%s\n",pch[argc-1]);
	                c_pch[c_argc] = strtok(NULL, " ,.-\n\t");
	            }

				char mystrpid[MAX_LINE_SIZE];
				sprintf(mystrpid,"%d",getpid());

				printf("%s\n", mystrpid);

				if (c_argc == 2 && strcmp(c_pch[0],"clone") == 0 && strcmp(c_pch[1],mystrpid) == 0 ){
					printf("CLONAZIONE %d\n", getpid() );
				}

				nbytes = -1;
			}
		}
	}
}

/*
void swapandset0(int i, int last) { //DELSI
    if(last-i >= 0) { // solo per check
        processi[i] = processi[last];
        processi[last] = 0;
    }
}
*/

/*
void ChildProcess(char *n) {
    pid_t  pid;
    char   *name = n;

    pid = getpid();
    printf("Child process %s starts (pid = %d)\n", name, pid);
    printf("STILL RUNNING DC\n");
}
*/

void killProcess(char* nome){ // devo gestire se tolgo processi da in mezzo
	pid_t temp = removebyname(&processi, nome);
    kill(temp, SIGKILL);
}

void esegui(char *words[MAX_ARGS], int arg_counter) {
    if (arg_counter == 1 && strcmp(words[0],"phelp") == 0){
        printf("\nComandi disponibili:\nphelp​ : stampa un elenco dei comandi disponibili\nplist​ : elenca i processi generati dalla shell custom\npnew <nome>​ : crea un nuovo processo con nome <nome>\npinfo <nome>​ : fornisce informazioni sul processo <nome> (almeno ​ pid ​ e ​ ppid ​ )\npclose <nome>​ : chiede al processo <nome> di chiudersi\nquit​ : esce dalla shell custom\n\n");
    }
    else if (arg_counter == 1 && strcmp(words[0],"plist") == 0){
        print_list();

    }
    else if (arg_counter == 2 && strcmp(words[0],"pnew") == 0){
        new_process(words[1]);
    }
    else if (arg_counter == 2 && strcmp(words[0],"pinfo") == 0){
        printf("informazioni sul processo %s (almeno pid e ppid)\n", words[1]);

        // DEBUG
        pid_t main_p = getpid();
        printf("this process id = %d\n", main_p);
        // end debug
    }
	else if (arg_counter == 2 && strcmp(words[0],"pclone") == 0){
        printf("Clonazione processo %s\n", words[1]);
		/* Child process closes up input side of pipe */
		close(fd[0]);

		pid_t p = getPidbyName(&processi,words[1]);

		char str[MAX_LINE_SIZE] = "clone ";
		char strpid[MAX_LINE_SIZE];
		sprintf(strpid,"%d",p);
		strcat(str,strpid);

	 	write(fd[1], str ,(strlen(str)+1));
    }
    else if (arg_counter == 2 && strcmp(words[0],"pclose") == 0){
        printf("chiede al processo %s di chiudersi\n", words[1]);
        killProcess(words[1]);
    }
    else if (arg_counter == 1 && strcmp(words[0],"quit") == 0)
	{
		killAll(&processi);
    	exit(0);
	}
}

int main()
{
	prctl(PR_SET_PDEATHSIG, SIGHUP);
    int argc = 0;
	pipe(fd);

    pid_t main_p = getpid();
    printf("Main process id = %d\n", main_p);
    initlist(&processi);
    while (1 && getpid() == main_p) {
        /* Print the command prompt */
        printf("$> ");

        /* Read a command line */
        if (!fgets(line, MAX_LINE_SIZE, stdin))
        return 0;
        else {
            argc = 0; // reinizializzo numero parole inserite
            pch[argc] = strtok(line," ,.-\n\t");
            while (pch[argc] != NULL) {
                argc++;
                //printf ("%s\n",pch[argc-1]);
                pch[argc] = strtok (NULL, " ,.-\n\t");
            }
        }
        //printf ("END, words: %d\n", argc);
        esegui(pch, argc);
    }
    return 0;
}
