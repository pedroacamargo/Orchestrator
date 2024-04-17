#include "global.h"


void processCommandSJF(Process process, char* outputFolder) {
    pid_t pid = fork();
    if (pid == -1) perror("Error on fork");
    if (pid == 0) childProccess(process, outputFolder);

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
                perror("Error on creating FCFS child process");
                return;
            }

            if (child_pid == 0) {
                processCommandSJF(newProcess, outputFolder);
                _exit(0);
            }
        }

    }
 
}


