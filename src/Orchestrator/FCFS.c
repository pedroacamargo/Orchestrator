#include "global.h"

int escalonamentoFCFS(int parallelTasks, char *comandos[]) {
    printf("Threads: %d\n", parallelTasks);

    int i = 0;
    char queue[10][100];

    // mudar isso dps
    while (i <= parallelTasks) {
        strcpy(queue[i], comandos[i]);
        i++;
    }

    // executa os comandos em paralelo
    // melhorar isso dps
    for (int index = 0; index < i; index++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Error on fork\n");
            return -1;
        }        

        if (pid == 0) {
            printf("Child process\n");
            printf("Comando %d: %s\n", index, queue[index]);

            int res = exec(queue[index]);
            if (res == -1) {
                perror("Error on exec\n");
                return -1;
            }

            _exit(0);
        }        
    }

    for (int index = 0; index < i; index++) {
        int status;
        int terminated_pid = wait(&status);
        printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));
    }

    return 0;
}