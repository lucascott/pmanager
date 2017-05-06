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

// pipe1 (proc. padre -> figli)
int    		fd[2], nbytes;
char		readbuffer[80];

// pipe2 (proc. figlio -> padre)
int     	fc[2], nbytes_c;
char		readbuffer_c[80];

// contatore processi cloni generati (solo per figli)
int 		counter = 0;

/// FUNZIONI
int isCommandWithParam(char * str){
    if (strcmp(str, "pnew") == 0 ||
    strcmp(str, "pinfo") == 0 ||
    strcmp(str, "pclose") == 0 ||
    strcmp(str, "pspawn") == 0 ||
    strcmp(str, "prmall") == 0 ){
        return 1;
    }
    return 0;
}

void print_list(){
	printf("%d processi figli attivi\n", length(processi));
    printf("PID\t NAME\t PPID\n=====================\n");
	printlist(processi);
}

void tokenize(char * line, char ** tokens, int *argc){
    *argc = 0; // reinizializzo numero parole inserite
    tokens[*argc] = strtok(line," ,.-\n\t");
    while (tokens[*argc] != NULL) {
        (*argc)++;
        tokens[*argc] = strtok (NULL, " ,.-\n\t");
    }
}

void new_process(char *nome){
	printf("Creazione del processo %s\n", nome);
    pid_t pid = fork();
	pid_t ppid = getpid();
	insertfront(&processi, pid, nome, ppid);
	if (getitem(processi, 0).pid < 0) {
		printf("Failed to fork process\n");
		exit(1);
	}
	if (getitem(processi, 0).pid == 0){ // PROCESSO FIGLIO

		while(1)
		{
			// apro la pipe del figlio in attesa di messaggio di clonazione
			close(fd[1]);
			nbytes = read(fd[0], readbuffer, sizeof(readbuffer));

			if (nbytes != -1){
				printf("Received string: %s\n", readbuffer);

				char * c_pch[MAX_ARGS];
				int c_argc = 0;
				tokenize(readbuffer, c_pch, &c_argc);

				// converto il pid del processo in stringa
				char mystrpid[MAX_LINE_SIZE];
				sprintf(mystrpid,"%d",getpid());
				printf("%s\n", mystrpid);

				if (c_argc == 3 && strcmp(c_pch[0],"clone") == 0 && strcmp(c_pch[1],mystrpid) == 0 ){
					printf("CLONAZIONE %d\n", getpid() );
					char tname[MAX_LINE_SIZE];
					pid_t tpid;
					tpid = fork();
					if (tpid < 0)
					{
						printf("Failed to fork clone process\n");
						exit(1);
					}
					else if (tpid == 0) // processo clone del figlio cicla all'infinito
					{
						while (1) {}
					}
					else // processo padre "figlio del padre"
					{
						counter++; // aumento contatore processi clonati

						//messaggio di ritorno al padre
						char str[MAX_LINE_SIZE];
						char strcounter[MAX_LINE_SIZE];
						char strtpid[MAX_LINE_SIZE];
						sprintf(strcounter,"%d",counter); // converto counter in stringa per strcat
						sprintf(strtpid,"%d",tpid);
						//Messaggio di risposta: "pid nome_counter"
						strcat(str,strtpid);
						strcat(str," ");
						strcat(str,c_pch[2]);
						strcat(str,"_");
						strcat(str,strcounter);
						// invio messaggio al padre
						close(fc[0]);
						write(fc[1], str ,(strlen(str)+1));
					}
				}
				// resetto counter bytes letti per rimanere nel loop del processo
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
        printf("\nComandi disponibili:\nphelp​ : stampa un elenco dei comandi disponibili\nplist​ : elenca i processi generati dalla shell custom\npnew <nome>​ : crea un nuovo processo con nome <nome>\npinfo <nome>​ : fornisce informazioni sul processo <nome>\npclose <nome>​ : chiede al processo <nome> di chiudersi\npspawn <nome>: chiede al processo <nome> di clonarsi creando <nome_i> con i progressivo\nprmall <nome>: chiede al processo <nome> di chiudersi chiudendo anche eventuali cloni\nptree: mostra la gerarchia completa dei processi generati attivi\nquit​ : esce dalla shell custom\n\n");
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
	else if (arg_counter == 2 && strcmp(words[0],"pspawn") == 0){

		// ottengo pid processo dal nome passato dall'utente
		pid_t p = getPidbyName(&processi,words[1]);

		if (p == -1){
			printf("Errore processo %s non esistente\n", words[1]);
		}
		else {
			printf("Clonazione processo %s\n", words[1]);

			close(fd[0]);

			// genero messaggio da inviare
			char str[MAX_LINE_SIZE] = "clone ";
			char strpid[MAX_LINE_SIZE];
			sprintf(strpid,"%d",p);
			strcat(str,strpid);
			strcat(str," ");
			strcat(str,words[1]);
			// invio messaggio in pipe1 ai processi figli
		 	write(fd[1], str ,(strlen(str)+1));

			// apetura pipe2 in lettura risposta clonazione
			do {
				close(fc[1]);
				nbytes_c = read(fc[0], readbuffer_c, sizeof(readbuffer_c));
				if (nbytes_c != -1){
					printf("risposta child: %s\n", readbuffer_c);
				}
			} while (nbytes_c == -1);
		}

    }
    else if (arg_counter == 2 && strcmp(words[0],"pclose") == 0){
        printf("Chiedo al processo %s di chiudersi\n", words[1]);
        killProcess(words[1]);
    }
	else if (arg_counter == 2 && strcmp(words[0],"prmall") == 0){
        printf("NON DISPONIBILE - chiude processo e eventuali cloni\n");
    }
	else if (arg_counter == 1 && strcmp(words[0],"ptree") == 0){
        printf("NON DISPONIBILE - mostra la gerarchia completa dei processi generati attivi\n" );
    }
    else if (arg_counter == 1 && strcmp(words[0],"quit") == 0){
		killAll(&processi);
    	exit(0);
	}
}

int main(int n_par, char *argv[]){
    int 	argc = 0;
	char    line[MAX_LINE_SIZE];
	char    *pch[MAX_ARGS];

	prctl(PR_SET_PDEATHSIG, SIGHUP);

	// apro le pipes
	pipe(fd);
	pipe(fc);

	// DEBUG
    pid_t main_p = getpid();
    printf("Main process id = %d\n", main_p);
	// end debug

	// inizializzo lista processi del padre
    initlist(&processi);

	FILE *ifp;
	printf("Pmanager run with %d params\n", n_par);
	if (n_par == 2){
		ifp = fopen(argv[1], "r");
		if (ifp == NULL) {
			printf("Impossibile aprire \"%s\"...\n",argv[1]);
			exit(1);
		}
        while (fscanf(ifp,"%s", line) != EOF){

            if (isCommandWithParam(line)){
                strcat(line, " ");
                char temp[50];
                fscanf(ifp,"%s", temp);
                strcat(line, temp);
            }
			// tokenizzo il comando passato
			char a[10];
			//scanf("%s", a);
			tokenize(line, pch, &argc);
			printf("ARGS: %d\n", argc);
			printf("$> %s %s\n", pch[0],pch[1]);

			// wait for testing


			// eseguo operazioni richieste
	        esegui(pch, argc);
        }
	}
	else {
	    while (1 && getpid() == main_p) {
	        // stampo il command prompt
	        printf("$> ");
	        // lettura comandi
	        if (!fgets(line, MAX_LINE_SIZE, stdin)){
	        	exit (1);
			}
			// tokenizzo il comando passato
			tokenize(line, pch, &argc);
			// eseguo operazioni richieste
	        esegui(pch, argc);
	    }
	}
    return 0;
}
