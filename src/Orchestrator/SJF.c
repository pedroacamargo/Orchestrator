#include "global.h"
#include "SJF.h"


void childProccessSJF(Process process) {
    printf("CHILD PROCESS SJF -> Executing (%d): <%s>\n", process.pid, process.command);
    process.status = PROCESS_STATUS_RUNNING;
    handleProcess(process);

    int n = checkPipe(process.command);
    int res;

    gettimeofday(&process.t1, 0);
    if (n > 1) execPipe(process.command, n);
    else {
        res = exec(process.command);
        if (res == -1) printf("(%d): Error on exec\n", process.pid);
    }   
    gettimeofday(&process.t2, 0);

    process.elapsedTime = (process.t2.tv_sec - process.t1.tv_sec) * 1000.0;      // sec to ms
    process.elapsedTime += (process.t2.tv_usec - process.t1.tv_usec) / 1000.0;   // us to ms

    process.status = PROCESS_STATUS_FINISHED;
    handleProcess(process);

    _exit(res);
}


void processCommand_2(Process process) {
    pid_t pid = fork();
    if (pid == -1) perror("Error on fork\n");
    if (pid == 0) childProccessSJF(process);

    else {
        int status;
        int terminated_pid = wait(&status);
        printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));

        int fdIdle = open("tmp/idle.txt", O_RDONLY, 0644);
        lseek(fdIdle, 0, SEEK_SET);

        if (countLines("tmp/idle.txt") > 0) {
            char buffer[MAX_COMMAND_SIZE];
            int bytesRead = read(fdIdle, buffer, sizeof(buffer) - 1);
            buffer[bytesRead] = '\0';
            int processNumber;
            sscanf(buffer, "%d", &processNumber);
            printf("Process Number: %d\n", processNumber);


            char *str = strstr(buffer, " ")  + 1; 
            char *command = strtok(str, "-");
            char *commandEnd = strtok(NULL, "\0");

            Process newProcess = {
                .pid = processNumber,
                .parentPid = 0,
                .status = PROCESS_STATUS_RUNNING,
                .elapsedTime = 0.0f,
                .t1 = {0, 0},
                .t2 = {0, 0},
                .timePrediction = atoi(commandEnd)
            };
            strcpy(newProcess.command, command);
            printf("Command: %s\n", newProcess.command);
            printf("Time Prediction: %d\n", newProcess.timePrediction);
            handleProcess(newProcess);

            int child_pid = fork();
            if (child_pid == -1) {
                perror("Error on creating FCFS child process\n");
                return;
            }

            if (child_pid == 0) {
                processCommand_2(newProcess);
                _exit(0);
            }
        }

    }
 
}


