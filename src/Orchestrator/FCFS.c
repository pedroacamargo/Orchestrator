#include "global.h"
#include "FCFS.h"

void childProccessFCFS(Process process) {
    printf("(%d): Executando comando <%s>\n", process.pid, process.command);
    process.status = PROCESS_STATUS_RUNNING;
    handleProcess(process, process.pid);

    int n = checkPipe(process.command);
    int res;

    gettimeofday(&process.t1, 0);
    if (n > 1){
        printf("PIPE DETECTED\n");
        execPipe(process.command, n); // no futuro deve retrornar um res 
    }
    else {
        res = exec(process.command);
        if (res == -1) printf("CHILD (%d): Error on exec\n", process.pid);
    } 
    gettimeofday(&process.t2, 0);

    process.elapsedTime = (process.t2.tv_sec - process.t1.tv_sec) * 1000.0;      // sec to ms
    process.elapsedTime += (process.t2.tv_usec - process.t1.tv_usec) / 1000.0;   // us to ms
    printf("Time: %.3f ms\n", process.elapsedTime);

    process.status = PROCESS_STATUS_FINISHED;
    handleProcess(process, process.pid);

    _exit(res);
}

void processCommand(char *comando, int id) {
    Process process = {
        .pid = id,
        .parentPid = getppid(),
        .status = PROCESS_STATUS_WAITING,
        .elapsedTime = 0.0f,
        .t1 = {0, 0},
        .t2 = {0, 0},
    };
    strcpy(process.command, comando);
    handleProcess(process, process.pid);

    pid_t pid = fork();
    if (pid == -1) perror("Error on fork\n");
    if (pid == 0) childProccessFCFS(process);
    else {
        int status;
        int terminated_pid = wait(&status);
        printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));
        process.status = PROCESS_STATUS_FINISHED;

        int fdIdle = open("tmp/idle.txt", O_RDONLY, 0644);
        int idle = countLines("tmp/idle.txt");
        lseek(fdIdle, 0, SEEK_SET);

        if (idle > 0) {
            char buffer[MAX_COMMAND_SIZE];
            int bytesRead = read(fdIdle, buffer, sizeof(buffer) - 1);
            buffer[bytesRead] = '\0';
            int processNumber;
            sscanf(buffer, "%d", &processNumber);

            char *command = strstr(buffer, " ")  + 1;
            printf("Command: %s", command);

            Process newProcess = {
                .pid = processNumber,
                .parentPid = 0,
                .status = PROCESS_STATUS_RUNNING,
                .elapsedTime = 0.0f,
                .t1 = {0, 0},
                .t2 = {0, 0},
            };
            strcpy(newProcess.command, retira_new_line(command));
            handleProcess(newProcess, newProcess.pid);

            int child_pid = fork();
            if (child_pid == -1) {
                perror("Error on creating FCFS child process\n");
                return;
            }

            if (child_pid == 0) {
                processCommand(newProcess.command, newProcess.pid);
                _exit(0);
            }
        }

        close(fdIdle);
    }
}

