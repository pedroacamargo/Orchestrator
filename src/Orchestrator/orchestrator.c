#include "global.h"
#include "FCFS.h"

char *comandos[] = {"sleep 4", "sleep 3", "sleep 4", "sleep 2", "sleep 3", "sleep 5", "sleep 1", "sleep 2", "ls -l -a -h", "sleep 1"};
char *comandos_for_sjf[] = {"10 sleep 4", "11 sleep 4", "12 sleep 5", "13 sleep 2", "14 sleep 5", "10 sleep 5", "1 ls /etc | wc -l"};

int main(int argc, char *argv[]){

    if (argc < 4){
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    fifo();

    int fifo_fd = open("fifo", O_RDWR);
    if (fifo_fd == -1){
        perror("open");
        return 1;
    }

    int fdIdle = open("tmp/idle.txt", O_CREAT | O_RDONLY, 0666);
    if (fdIdle == -1){
        perror("open");
        return 1;
    }
    int fdScheduled = open("tmp/scheduled.txt", O_CREAT | O_RDONLY, 0666);
    if (fdScheduled == -1){
        perror("open");
        return 1;
    }
    int fdExecuting = open("tmp/executing.txt", O_CREAT | O_RDONLY, 0666);
    if (fdExecuting == -1){
        perror("open");
        return 1;
    }
    int fdCompleted = open("tmp/completed.txt", O_CREAT | O_RDONLY, 0666);
    if (fdCompleted == -1){
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

    if (policy == SJF){
        while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1)) > 0){
            buffer[bytes_read] = '\0';

            printf("Received: %s\n", buffer);
            //run(argv[1], atoi(argv[2]), argv[3], retira_new_line(buffer));
            escalonamentoSJF(1,retira_new_line(buffer));

            // Limpe o buffer para a próxima leitura
            memset(buffer, 0, sizeof(buffer));
        }
    } else {

        int commandsWritten = 1;
        int executing = 0;
        int idle = 0;
        printf("=============== FCFS ===============\n");
        while(1) {
            printf("Commands written: %d\n", commandsWritten);
            if (idle > 0) {
                idle = countLines("tmp/idle.txt");
            }
            
            Process newProcess;
            memset(&newProcess, 0, sizeof(Process));
            printf("Waiting for new process...\n");

            if ((bytes_read = read(fifo_fd, buffer, sizeof(buffer))) > 0){;
                buffer[bytes_read] = '\0';
                char *first_string = strtok(buffer, "-");
                char *second_string = strtok(NULL, "\0");
                printf("---> New process received\n");
                printf("--> Time predicted: %s miliseconds\n", first_string);
                printf("-> Command to be executed: %s\n", second_string);    

                newProcess.pid = commandsWritten;
                strcpy(newProcess.command, second_string);
                newProcess.timePrediction = atoi(first_string);
                memset(buffer, 0, sizeof(buffer));
            }


            // Store the command in the idle file if all threads are executing
            if (executing >= atoi(argv[2])) { // Todas a threads estão sendo utilizadas

                newProcess.status = PROCESS_STATUS_IDLE;

                handleProcess(newProcess);

                memset(buffer, 0, sizeof(buffer));
                commandsWritten++;
            } else if (executing < atoi(argv[2])) {
                if (idle > 0) {
                    // read from idle file           
                    memset(buffer, 0, sizeof(buffer));         
                    int bytesRead = read(fdIdle, buffer, sizeof(buffer) - 1);
                    lseek(fdIdle, 0, SEEK_SET);
                    buffer[bytesRead] = '\0';
                    int processNumber;
                    sscanf(buffer, "%d", &processNumber);
                    printf("Process Number: %d\n", processNumber);

                    char *str = strstr(buffer, " ")  + 1;

                    char *command = strtok(str, "-");
                    char *commandEnd = strtok(NULL, "\0");

                    Process idleProcess = {
                        .pid = processNumber,
                        .parentPid = 0,
                        .status = PROCESS_STATUS_RUNNING,
                        .elapsedTime = 0.0f,
                        .t1 = {0, 0},
                        .t2 = {0, 0},
                        .timePrediction = atoi(commandEnd)
                    };
                    strcpy(idleProcess.command, command);
                    printf("Command: %s\n", idleProcess.command);
                    printf("Time Prediction: %d\n", idleProcess.timePrediction);
                    handleProcess(idleProcess);

                    int child_pid = fork();
                    if (child_pid == -1) {
                        perror("Error on creating FCFS child process\n");
                    }

                    if (child_pid == 0) {
                        processCommand(idleProcess);
                        _exit(0);
                    }

                    memset(buffer, 0, sizeof(buffer));

                    // --------------------------------------------

                    newProcess.status = PROCESS_STATUS_IDLE;
                    handleProcess(newProcess);

                    commandsWritten++;
                } else {
                    newProcess.status = PROCESS_STATUS_RUNNING;
                    newProcess.elapsedTime = 0.0f;
                    newProcess.t1 = (struct timeval){0, 0};
                    newProcess.t2 = (struct timeval){0, 0};

                    int child_pid = fork();
                    if (child_pid == -1) {
                        perror("Error on creating FCFS child process\n");
                        return 1;
                    }

                    if (child_pid == 0) {
                        processCommand(newProcess);
                        _exit(0);
                    }

                    commandsWritten++;
                }
            }

            executing = countLines("tmp/executing.txt");
            idle = countLines("tmp/idle.txt");
            printf("Executing: %d\n", executing);
            memset(buffer, 0, sizeof(buffer));
        }
    }

    if (bytes_read == -1){
        perror("read");
    }

    close(fifo_fd);
    close(fdIdle);
    close(fdScheduled);
    close(fdExecuting);
    close(fdCompleted);

return 0; 
}

