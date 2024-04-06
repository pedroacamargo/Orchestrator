#include "global.h"

void childProccessFCFS(int childPid, int index, char queue[][100], int parentPid) {
    Process process;
    process.pid = childPid;
    strcpy(process.command, queue[index]);
    process.status = PROCESS_STATUS_RUNNING;
    process.parentPid = parentPid;

    printf("(%d): Executando comando <%s>\n", process.pid, process.command);

    int res = exec(queue[index]);
    if (res == -1) printf("CHILD (%d): Error on exec\n", childPid);

    _exit(res);
}

int escalonamentoFCFS(int parallelTasks, char *comandos[]) {
    int queueSize = 5;
    char queue[10][100];
    int actualProcessIndex = 0, finishedProcesses = 0;

    for (int i = 0; i < queueSize; i++) strcpy(queue[i], comandos[i]);

    while (actualProcessIndex < queueSize) {
        printf("actualProcessIndex: %d, finishedProcesses: %d\n", actualProcessIndex, finishedProcesses);
        
        for (; actualProcessIndex < parallelTasks + finishedProcesses && actualProcessIndex < queueSize; actualProcessIndex++) {
            pid_t pid = fork();
            if (pid == -1) perror("Error on fork\n");
            if (pid == 0) childProccessFCFS(getpid(), actualProcessIndex, queue, getppid());
        }

        for (finishedProcesses; finishedProcesses < actualProcessIndex && actualProcessIndex <= queueSize; finishedProcesses++) {
            int status;
            int terminated_pid = wait(&status);
            printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));
        }
    }

    return 0;
}