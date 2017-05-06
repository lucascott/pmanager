#include <stdio.h>
#include <string.h>


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

int main(int n_par, char *argv[])
{
    FILE *ifp;
    char str[50];
    printf("Pmanager run with %d params\n", n_par);
    if (n_par == 2){
        ifp = fopen(argv[1], "r");
        if (ifp == NULL) {
            printf("Can't open input file...\n");
            exit(1);
}
        while (fscanf(ifp,"%s", str) != EOF){

            if (isCommandWithParam(str)){
                strcat(str, " ");
                char temp[50];
                fscanf(ifp,"%s", temp);
                strcat(str, temp);
            }
            printf("Letto: %s\n", str);
        }
    }

    return(0);
}
