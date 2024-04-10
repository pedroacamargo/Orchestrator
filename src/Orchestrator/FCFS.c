#include "global.h"

void childProccessFCFS(int childPid, int index, char queue[][100], int parentPid) {
    Process process;
    process.pid = childPid;
    strcpy(process.command, queue[index]);
    process.status = PROCESS_STATUS_RUNNING;
    process.parentPid = parentPid;

    printf("(%d): Executando comando <%s>\n", process.pid, process.command);

    gettimeofday(&process.t1, 0);
    int res = exec(queue[index]);
    if (res == -1) printf("CHILD (%d): Error on exec\n", childPid);
    gettimeofday(&process.t2, 0);

    process.elapsedTime = (process.t2.tv_sec - process.t1.tv_sec) * 1000.0;      // sec to ms
    process.elapsedTime += (process.t2.tv_usec - process.t1.tv_usec) / 1000.0;   // us to ms
    printf("Time: %.3f ms\n", process.elapsedTime);



    _exit(res);
}

// TODO: Return the processes to store in the output file
// TODO: Implement pipes to communicate with the child processes and get the Process struct
int escalonamentoFCFS(int parallelTasks, char *comandos[]) {
    int queueSize = 8;
    char queue[10][100];
    ProcessStatus statusQueue[10];
    int actualProcessIndex = 0, finishedProcesses = 0;

    for (int i = 0; i < queueSize; i++) {
        strcpy(queue[i], comandos[i]);
        if (i < parallelTasks) statusQueue[i] = PROCESS_STATUS_WAITING;
        else statusQueue[i] = PROCESS_STATUS_IDLE;
    }

    while (actualProcessIndex < queueSize) {
        while (actualProcessIndex < queueSize && statusQueue[actualProcessIndex] == PROCESS_STATUS_WAITING) {
            pid_t pid = fork();
            if (pid == -1) perror("Error on fork\n");
            if (pid == 0) childProccessFCFS(getpid(), actualProcessIndex, queue, getppid());
            actualProcessIndex++;
        }

        for (finishedProcesses; finishedProcesses < actualProcessIndex && actualProcessIndex <= queueSize; finishedProcesses++) {
            int status;
            int terminated_pid = wait(&status);
            printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));
            statusQueue[finishedProcesses] = PROCESS_STATUS_FINISHED;
            if (finishedProcesses + parallelTasks < queueSize) {
                statusQueue[finishedProcesses + parallelTasks] = PROCESS_STATUS_WAITING;
            }


            // Simulate the creation of a new process (Thread) if there are still processes to be executed
            if (actualProcessIndex < queueSize && statusQueue[actualProcessIndex] == PROCESS_STATUS_WAITING) {
                pid_t pid = fork();
                if (pid == -1) perror("Error on fork\n");
                if (pid == 0) childProccessFCFS(getpid(), actualProcessIndex, queue, getppid());
                actualProcessIndex++;
            }

            printf("actualProcessIndex: %d, finishedProcesses: %d\n", actualProcessIndex, finishedProcesses);
        }
        printf("actualProcessIndex: %d, finishedProcesses: %d\n", actualProcessIndex, finishedProcesses);
    }

    return 0;
}