#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include "list.h"
#include "utils.h"

List processi; // dichiarazione lista processi

int         counter = 0; // contatore processi cloni generati (solo per figli)

// PIPE (proc. figlio -> padre)
int         fc[2];
char		readbuffer_c[80];

// PIPE (proc. padre -> figlio)
int         fn[2];
char        readbuffer_n[80];

// FUNZIONI
int isCommandWithParam(char * str) { // controlla se il comando str richiede parametri
    if (strcmp(str, "pnew") == 0 ||
    strcmp(str, "pinfo") == 0 ||
    strcmp(str, "pclose") == 0 ||
    strcmp(str, "pspawn") == 0 ||
    strcmp(str, "prmall") == 0 ) {
        return 1;
    }
    return 0;
}

void getDate(char *data) { // salva la data e l'ora su data
	struct timeval tv;
	struct tm* ptm;
	long milliseconds;

	// ottiene il tempo e lo converte in una struct tm
	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);

	strftime (data, 104, "%H:%M:%S", ptm); // formatta la data e l'ora fino ai secondi
	milliseconds = tv.tv_usec / 1000; // calcola i millisecondi partendo dai microsecondi
	sprintf(data, "%s.%03ldms", data, milliseconds); // stampa la data e il tempo in secondi seguito da virgola e millisecondi
}

void print_list() { // stampa la lista processi
    printf("\t    %d processi figli attivi\n", numActive(&processi)); // stampa numero processi attivi
    printf("PID\t NAME           \t PPID \tCREATED\n=======================================================\n");
    printlist(processi); // invoca la funzione che stampa la lista
}

void tokenize(char * line, char ** tokens, int *argc) { // divide la stringa in singole parole
    *argc = 0; // inizializzo numero parole inserite
    tokens[*argc] = strtok(line," \n\t"); // itilizzo strtok per troncare la stringa al primo spazio
    while (tokens[*argc] != NULL) // itero tutti gli argomenti
    {
        (*argc)++;
        tokens[*argc] = strtok (NULL, " \n\t");
    }
}

void getMyPid(char * mystrpid) { // salva in mystrpid il PID del processo che invoca la funzione
    sprintf(mystrpid,"%d",getpid());
}

void handler (int signo) {  // handler per SIGUSR1
    if (signo == SIGUSR1) { // se il segnale ricevuto è SIGUSR1
        int nbytes_n = read(fn[READ], readbuffer_n, sizeof(readbuffer_n)); // legge dalla pipe il nome del processo da clonare
        fflush(stdout);
        if (nbytes_n == -1) { // se non legge nulla dalla pipe da errore
            printf(ANSI_COLOR_RED"Clonazione processo non riuscita...\n"ANSI_COLOR_RESET);
            exit(-1);
        }
        pid_t tpid = fork(); // altrimenti fa un fork
        fflush(stdout);
        if (tpid < 0) { // se la system call fallisce da errore
            printf(ANSI_COLOR_RED"Clonazione processo non riuscita...\n"ANSI_COLOR_RESET);
            exit(-1);
        }
        else if (tpid == 0) { // processo clone del figlio
            counter = 0; // azzera il contatore di processi del figlio
        }
        else {// processo padre "figlio del padre"
            counter++; // aumento contatore processi clonati

            // messaggio di ritorno al padre
            char str[MAX_LINE_SIZE];
            char strcounter[MAX_LINE_SIZE];
            char strtpid[MAX_LINE_SIZE];
            sprintf(strcounter,"%d",counter); // converto counter in stringa per strcat
            sprintf(strtpid,"%d",tpid);

            // Messaggio di risposta: "pid nome_counter"
            strcpy(str,strtpid);
            strcat(str," ");
            strcat(str,readbuffer_n);
            if (strlen(readbuffer_n) < 50)
            {
                strcat(str,"_");
                strcat(str,strcounter);
            }
            else
                printf(ANSI_COLOR_RED"Impossibile concatenare, nome clone troppo lungo. Tronco \"_%d\"\n"ANSI_COLOR_RESET,counter);

            fflush(stdout);

            // invio messaggio al padre
            write(fc[WRITE], str,(strlen(str)+1));
        }
    }
}

void new_process(char *nome) { // crea un nuovo processo chiamato "nome"
    if (strcmp(nome, "XXX") == 0 || strcmp(nome, "xxx") == 0) { // controllo correttezza nome inserito (XXX riservato al programma)
        printf(ANSI_COLOR_RED"Nome \"%s\" riservato al sistema. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else {
        printf("Creazione del processo \"%s\"\n", nome); // conferma creazione processo
    	if (strlen(nome) > 50)
    	{
    		strncpy(nome, nome, 50);
        	nome[50] = '\0';
    		printf("Nome troppo lungo. Creo \"%s\"\n",nome); // avviso troncamento nome
    	}
        pid_t pid = fork(); // FORK DEL PROCESSO
        pid_t ppid = getpid(); // ottengo ppid del figlio dal padre
        if (pid < 0) { // gestione errori creazione processo
           printf(ANSI_COLOR_RED "Creazione processo non riuscita...\n"ANSI_COLOR_RESET);
           exit(-1); // termine programma in caso di errore
        }
        else if (pid == 0) { // PROCESSO FIGLIO
            destroy(&processi); // dealloca la lista dei figli per risparmiare memoria
            while(1) {
                sleep(-1); // metto il processo in loop di attesa infinito
            }
        }
        else{
            char d[MAX_LINE_SIZE]; // PROCESSO PADRE
            getDate(d); // recupero data creazione figlio
            insertback(&processi, pid, nome, ppid, d); // aggiungo figlio alla lista
            printf("Processo \"%s\" creato con successo (pid: %d, ore: %s)\n", nome, pid, d);
            fflush(stdout); // libero il buffer stdout
        }

    }
}

void info_process(char *nome) { // stampa informazioni sul processo "nome"
    if (strcmp(nome, "XXX")== 0 || strcmp(nome, "xxx")== 0) { // controllo correttezza nome inserito (XXX riservato al programma)
        printf(ANSI_COLOR_RED"Nome \"%s\" riservato al sistema. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else if (strlen(nome) > 50) {
        printf(ANSI_COLOR_RED"Nome processo troppo lungo. Inserisci un nome di massimo 50 caratteri\n"ANSI_COLOR_RESET); // avviso nome troppo lungo
    }
    else {
        pid_t pid, ppid;
        char d[MAX_LINE_SIZE];
        getInfos(&processi, nome, &pid, &ppid, d); // richiedo informazioni sul processo
        if (pid == -1 && ppid == -1) { // gestione processo inesistente
            printf(ANSI_COLOR_RED"Processo \"%s\" inesistente. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
        }
        else{
            printf("Processo %s (pid: %d, ppid: %d, creato alle: %s)\n", nome, pid, ppid, d); // stampo i dati del processo
        }
    }
}

void spawn_process(char *nome) { // chiede al processo "nome" di clonarsi
    if (strcmp(nome, "XXX") == 0 || strcmp(nome, "xxx") == 0) { // controllo correttezza nome inserito (XXX riservato al programma)
        printf(ANSI_COLOR_RED"Nome \"%s\" riservato al sistema. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else {
        if (strlen(nome) > 50)
        {
                strncpy(nome, nome, 50);
                nome[50] = '\0';
                printf("Nome troppo lungo. Uso \"%s\"\n",nome); // avviso troncamento nome
        }
        pid_t p = checkDuplicates(&processi, nome, "clonare"); // gestisco casi di omonimia
        if (p == -1) { // gestione processo da clonare inesistente
            printf(ANSI_COLOR_RED"Errore processo \"%s\" non esistente. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
        }
        else {
            printf("Clonazione processo \"%s\" (pid: %d)\n", nome, p);
            // mando messaggio a processo che deve clonarsi
            kill(p,SIGUSR1);
            write(fn[WRITE], nome,(strlen(nome)+1)); // scrive nella pipe il nome del processo che l'utente ha chiesto di clonare
            signal(SIGUSR1,handler);
            int nbytes_c = read(fc[READ], readbuffer_c, sizeof(readbuffer_c)); // legge dalla pipe il messaggio del padre che dovrebbe essere nel formato "pid nome_processo"
            if (nbytes_c == -1) { // se il padre non scrive nulla
                printf(ANSI_COLOR_RED"Clonazione processo non riuscita...\n"ANSI_COLOR_RESET);
                exit(-1);
            }
            char * ris [MAX_ARGS];
            int arg_ris;
            char d[MAX_LINE_SIZE];
            getDate(d); // trova l'ora attuale
            tokenize(readbuffer_c, ris, &arg_ris); // divide in un array la risposta del padre. Nel posto zero ci sarà il pid, nel posto 1 il nome del processo
            insertback(&processi, atoi(ris[0]), ris[1], p, d); // lo inserisce nella lista
            printf("Processo clone \"%s\" creato con successo (pid: %s, ppid: %d ore: %s)\n",ris[1], ris[0], p, d);
        }
    }
}

void kill_process(char* nome) { // chiude il processo "nome"
    if (strcmp(nome, "XXX") == 0 || strcmp(nome, "xxx") == 0) { // controllo correttezza nome inserito (XXX riservato al programma)
        printf(ANSI_COLOR_RED"Nome \"%s\" riservato al sistema. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else if (strlen(nome) > 50) {
        printf(ANSI_COLOR_RED"Nome processo troppo lungo. Inserisci un nome di massimo 50 caratteri\n"ANSI_COLOR_RESET); // avviso nome troppo lungo
    }
    else {
        pid_t temp = change_item_name(&processi, nome, "XXX"); // cambio nome processo in XXX (convenzione per processo terminato)

        if (temp == -1) { // gestione processo inesistente
            printf(ANSI_COLOR_RED"Processo \"%s\" inesistente. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
        }
        else{
            printf("Chiedo al processo \"%s\" di chiudersi...\n", nome);
            kill(temp, SIGTERM); // invio segnale di kill al processo
            printf("Processo \"%s\" chiuso correttamente.\n", nome); // comunico corretta chiusura
        }
    }
}

void rmall_process(char* nome) { // termina a cascata processo "nome" e relativi cloni
    if (strcmp(nome, "XXX") == 0 || strcmp(nome, "xxx") == 0) { // controllo correttezza nome inserito (XXX riservato al programma)
        printf(ANSI_COLOR_RED"Nome \"%s\" riservato al sistema. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else if (strlen(nome) > 50) {
        printf(ANSI_COLOR_RED"Nome processo troppo lungo. Inserisci un nome di massimo 50 caratteri\n"ANSI_COLOR_RESET); // avviso nome troppo lungo
    }
    else if (getPidbyName(&processi, nome) == -1) { // gestione processo inesistente
        printf(ANSI_COLOR_RED"Processo \"%s\" inesistente. Comando ignorato...\n"ANSI_COLOR_RESET, nome);
    }
    else{
        printf("Chiusura albero processi di \"%s\"...\n", nome);
        rmallrec(&processi, nome); // chiamata funzione di eliminazione del processo "nome" e relativi figli
    }
}

void tree_process() { // stampa albero processi
    if (processi.head == 0) { // gestione in caso di coda vuota
        printf("\nNessun processo attivo.\n\n");
    }
    else {
        printf("\nAlbero processi:\n");
        treerecchild(processi.head, getpid(),0,0); // chiamata funzione stampa albero processi
        printf("\n");
    }
}

void esegui(char *words[MAX_ARGS], int arg_counter) { // interpreta ed esegue i comandi utente
    if (arg_counter != 0) { // controllo che vi sia almeno un argomento
        if (strcmp(words[0],"phelp") == 0) { // esecuzione comando "phelp"
            if (arg_counter == 1) { // controllo numero argomenti
                printf("\nComandi disponibili:\n\nphelp​:\t\tstampa un elenco dei comandi disponibili\nplist​:\t\telenca i processi generati dalla shell custom\npnew <nome>:\tcrea un nuovo processo con nome <nome>\npinfo <nome>​:\tfornisce informazioni sul processo <nome>\npclose <nome>:\tchiede al processo <nome> di chiudersi\npspawn <nome>:\tchiede al processo <nome> di clonarsi creando <nome_i> con 'i' progressivo\nprmall <nome>:\tchiede al processo <nome> di chiudersi chiudendo anche eventuali cloni\nptree:\t\tmostra la gerarchia completa dei processi generati attivi\nquit​:\t\tesce dalla shell custom\n\n");
            }
            else {
                printf(ANSI_COLOR_RED"Comando errato. Uso corretto: phelp\n"ANSI_COLOR_RESET);
            }
        }
        else if (strcmp(words[0],"plist") == 0) { // esecuzione comando "plist"
            if (arg_counter == 1) { // controllo numero argomenti
                print_list();
            }
            else {
                printf(ANSI_COLOR_RED"Comando errato. Uso corretto: plist\n"ANSI_COLOR_RESET);
            }
        }
        else if (strcmp(words[0],"pnew") == 0) { // esecuzione comando "pnew"
            if (arg_counter == 2) { // controllo numero argomenti
                    new_process(words[1]);
            }
            else {
                printf (ANSI_COLOR_RED"Comando errato. Uso corretto: pnew <nome>\n"ANSI_COLOR_RESET);
            }
        }
        else if (strcmp(words[0],"pinfo") == 0) { // esecuzione comando "pinfo"
            if (arg_counter == 2) { // controllo numero argomenti
                info_process(words[1]);
            }
            else {
                printf (ANSI_COLOR_RED"Comando errato. Uso corretto: pinfo <nome>\n"ANSI_COLOR_RESET);
            }
        }
        else if (strcmp(words[0],"pspawn") == 0) { // esecuzione comando "pspawn"
            if (arg_counter == 2) { // controllo numero argomenti
                spawn_process(words[1]);
            }
            else {
                printf (ANSI_COLOR_RED"Comando errato. Uso corretto: pspawn <nome>\n"ANSI_COLOR_RESET);
            }
        }
        else if (strcmp(words[0],"pclose") == 0) { // esecuzione comando "pclose"
            if (arg_counter == 2) { // controllo numero argomenti
                kill_process(words[1]); // chiudo processo
            }
            else {
                printf(ANSI_COLOR_RED"Comando errato. Uso corretto: pclose <nome>\n"ANSI_COLOR_RESET);
            }
        }
        else if (strcmp(words[0],"prmall") == 0) { // esecuzione comando "prmall"
            if (arg_counter == 2) { // controllo numero argomenti
                rmall_process(words[1]);
            }
            else {
                printf(ANSI_COLOR_RED"Comando errato. Uso corretto: prmall <nome>\n"ANSI_COLOR_RESET);
            }
        }
        else if (strcmp(words[0],"ptree") == 0) { // esecuzione comando "ptree"
            if (arg_counter == 1) { // controllo numero argomenti
                tree_process();
            }
            else {
                printf(ANSI_COLOR_RED"Comando errato. Uso corretto: ptree\n"ANSI_COLOR_RESET);
            }
        }
        else if (strcmp(words[0],"quit") == 0) { // esecuzione comando "quit"
            if (arg_counter == 1) { // controllo numero argomenti
                printf("Chiudo processi aperti e termino pmanager...\n");
                killAll(&processi); // chiudo tutti i processi aperti rimanenti
                exit(0); // termino il programma
            }
            else {
                printf(ANSI_COLOR_RED"Comando errato. Uso corretto: quit\n"ANSI_COLOR_RESET);
            }
        }
        else { // gestione comando errato
            printf(ANSI_COLOR_RED"Comando non riconosciuto. Digita \"phelp\" per la lista dei comandi disponibili.\n"ANSI_COLOR_RESET);
        }
    }
}
