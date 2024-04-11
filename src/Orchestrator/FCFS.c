#include "global.h"
#include "FCFS.h"

void childProccessFCFS(Process process) {
    printf("(%d): Executando comando <%s>\n", process.pid, process.command);

    process.status = PROCESS_STATUS_RUNNING;
    handleProcess(process, process.pid);


    gettimeofday(&process.t1, 0);
    int res = exec(process.command);
    if (res == -1) printf("CHILD (%d): Error on exec\n", process.pid);
    gettimeofday(&process.t2, 0);

    process.elapsedTime = (process.t2.tv_sec - process.t1.tv_sec) * 1000.0;      // sec to ms
    process.elapsedTime += (process.t2.tv_usec - process.t1.tv_usec) / 1000.0;   // us to ms
    printf("Time: %.3f ms\n", process.elapsedTime);

    process.status = PROCESS_STATUS_FINISHED;
    handleProcess(process, process.pid);

    _exit(res);
}

// TODO: Return the processes to store in the output file
int escalonamentoFCFS(int parallelTasks, char *comando, int commandsWritten) {
    Process queue[parallelTasks];
    int actualProcessIndex = 0, finishedProcesses = 0;

    // Seed the queue with the processes to be executed
    for (int i = 0; i < parallelTasks; i++) {
        Process process = {
            .pid = commandsWritten, // TODO: Implement id
            .parentPid = getppid(),
            .status = PROCESS_STATUS_WAITING,
            .elapsedTime = 0.0f,
            .t1 = {0, 0},
            .t2 = {0, 0},
        };
        strcpy(process.command, comando);

        queue[i] = process;
        handleProcess(process, process.pid);
    }

    while (actualProcessIndex < parallelTasks) {
        pid_t pid = fork();
        if (pid == -1) perror("Error on fork\n");
        if (pid == 0) childProccessFCFS(queue[actualProcessIndex]);

        actualProcessIndex++;
    }

    for (; finishedProcesses < actualProcessIndex && actualProcessIndex <= parallelTasks; finishedProcesses++) {
        int status;
        int terminated_pid = wait(&status);
        printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));
        queue[finishedProcesses].status = PROCESS_STATUS_FINISHED;
        
        // printf("actualProcessIndex: %d, finishedProcesses: %d\n", actualProcessIndex, finishedProcesses);
    }

    // printf("actualProcessIndex: %d, finishedProcesses: %d\n", actualProcessIndex, finishedProcesses);
    return 0;
}