#include "global.h"
#include "FCFS.h"

char *comandos[] = {"sleep 4", "sleep 3", "sleep 4", "sleep 2", "sleep 3", "sleep 5", "sleep 1", "sleep 2", "ls -l -a -h", "sleep 1"};
char *comandos_for_sjf[] = {"10 sleep 4", "11 sleep 4", "12 sleep 5", "13 sleep 2", "14 sleep 5", "10 sleep 5", "1 ls /etc | wc -l"};

int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    fifo();

    int fifo_fd = open("fifo", O_RDONLY);
    if (fifo_fd == -1) {
        perror("open");
        return 1;
    }

    int fdIdle = open("tmp/idle.txt", O_CREAT | O_RDONLY, 0666);
    if (fdIdle == -1) {
        perror("open");
        return 1;
    }
    int fdScheduled = open("tmp/scheduled.txt", O_CREAT | O_RDONLY, 0666);
    if (fdScheduled == -1) {
        perror("open");
        return 1;
    }
    int fdExecuting = open("tmp/executing.txt", O_CREAT | O_RDONLY, 0666);
    if (fdExecuting == -1) {
        perror("open");
        return 1;
    }
    int fdCompleted = open("tmp/completed.txt", O_CREAT | O_RDONLY, 0666);
    if (fdCompleted == -1) {
        perror("open");
        return 1;
    }

    printf("Orchestrator started!\n");

    char buffer[256];
    ssize_t bytes_read;

    int policy = checkpolicy(argv[3]);
    if (policy == INVALID_POLICY) {
        printf("Invalid policy, server suspended!\n");
        return 1;
    }

    if (policy == SJF) {
        while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';

            printf("Received: %s\n", buffer);
            //run(argv[1], atoi(argv[2]), argv[3], retira_new_line(buffer));
            escalonamentoSJF(1, retira_new_line(buffer));

            // Limpe o buffer para a próxima leitura
            memset(buffer, 0, sizeof(buffer));
        }
    } else {

        //int commandsWritten = 1;
        //int executing = 0;
        //int idle = 0;
        while (1){
        Process newProcess;
         while ((bytes_read = read(fifo_fd,&newProcess,sizeof(newProcess)) > 0)){
            printf("Received process: %s\n", newProcess.command);
            printf("Received time: %d\n", newProcess.time);   
        }

        // TODO: Fazer um loop infinito enquanto o fifo não for fechado
        /* while (1) {
            
            // Store the command in the idle file if all threads are executing
            if (executing == atoi(argv[2])) {
                Process newProcess;
                bytes_read = read(fifo_fd, &newProcess, sizeof(newProcess));

                newProcess.pid = commandsWritten;
                newProcess.status = PROCESS_STATUS_IDLE;

                handleProcess(newProcess, newProcess.pid);
                memset(&newProcess, 0, sizeof(newProcess));

                commandsWritten++;

            } else if (executing < atoi(argv[2])) {
                if (idle > 0) {
                    // read from idle file
                    int bytesRead = read(fdIdle, buffer, sizeof(buffer) - 1);
                    lseek(fdIdle, 0, SEEK_SET);
                    buffer[bytesRead] = '\0';
                    int processNumber;
                    sscanf(buffer, "%d", &processNumber);

                    char *command = strstr(buffer, " ")  + 1;
                    // printf("Command: %s", command);

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
                        return 1;
                    }

                    if (child_pid == 0) {
                        processCommand(newProcess.command, newProcess.pid);
                        _exit(0);
                    }

                    memset(buffer, 0, sizeof(buffer));

                    // --------------------------------------------
                    // read from fifo
                    Process fifoProcess;
                    bytes_read = read(fifo_fd, &fifoProcess, sizeof(fifoProcess));
                
                    fifoProcess.pid = commandsWritten;
                    fifoProcess.status = PROCESS_STATUS_IDLE;

                    handleProcess(fifoProcess, fifoProcess.pid);

                    commandsWritten++;
                    memset(&fifoProcess, 0, sizeof(fifoProcess));

                } else {
                    Process fifoProcess;
                    bytes_read = read(fifo_fd, &fifoProcess, sizeof(fifoProcess));

                    fifoProcess.status = PROCESS_STATUS_RUNNING;
                    fifoProcess.pid = commandsWritten;

                    handleProcess(fifoProcess, fifoProcess.pid);

                    int child_pid = fork();
                    if (child_pid == -1) {
                        perror("Error on creating FCFS child process\n");
                        return 1;
                    }

                    if (child_pid == 0) {
                        processCommand(fifoProcess.command, fifoProcess.pid);
                        _exit(0);
                    }

                    commandsWritten++;
                }
            }

            executing = countLines("tmp/executing.txt");
            idle = countLines("tmp/idle.txt");
            printf("Executing: %d\n", executing);
            memset(buffer, 0, sizeof(buffer));
        } */
    }

        close(fifo_fd);
    }
    close(fdIdle);
    close(fdScheduled);
    close(fdExecuting);
    close(fdCompleted);

    return 0;
}


