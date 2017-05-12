#include<stdio.h>
#include<signal.h>
#include<unistd.h>

void sig_handler(int signo)
{
    printf("fsaiodas SIGUSR1\n");
    if (signo == SIGUSR1){
        printf("received SIGUSR1\n");
        pid_t i = fork();
        if (i == 0){
            
        }
    }
}

int main(void)
{
    if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGUSR1\n");

    while(1)
        usleep(1);
    return 0;
}
