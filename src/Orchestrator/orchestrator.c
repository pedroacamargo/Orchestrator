#include "global.h"

int main(int argc, char *argv[]){
    

    if (argc < 4){
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    int fd = mkfifo(SERVER, 0666);
    if (fd == -1){
        perror("Open fifo server");
        _exit(1);
    }

    int fdIdle = open("tmp/idle.txt", O_CREAT | O_RDONLY, 0666);
    if (fdIdle == -1){
        perror("Open ide.txt");
        return 1;
    }

    int fdExecuting = open("tmp/executing.txt", O_CREAT | O_RDONLY, 0666);
    if (fdExecuting == -1){
        perror("Open executing.txt");
        return 1;
    }

    int fdCompleted = open("tmp/completed.txt", O_CREAT | O_RDONLY, 0666);
    if (fdCompleted == -1){
        perror("Open completed.txt");
        return 1;
    }



    if (checkpolicy(argv[3]) == INVALID_POLICY){
        printf("Invalid policy, server suspended!\n");
        return 1;
    }
    
    else{

        printf("Orchestrator started!\n");

        char buffer[256];
        ssize_t bytes_read;
        int commandsWritten = 1;

        while (1){

            int fd = open(SERVER, O_RDONLY);
            if (fd == -1){
                perror("Open fifo server");
                _exit(1);
            }

            int fd_client = open(CLIENT, O_WRONLY);
            if (fd_client == -1){
                perror("Open fifo client");
                _exit(1);
            }

            Process newProcess;
            memset(&newProcess, 0, sizeof(Process));
            printf("Waiting for new process...\n");

            if ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0){

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

                sprintf(buffer, "TASK %d Received", newProcess.pid);

                if (write(fd_client, buffer, strlen(buffer)) == -1){
                    perror("write");
                    _exit(1);
                }
                memset(buffer, 0, sizeof(buffer));
            }

            if (countLines("tmp/executing.txt") < atoi(argv[2]) && countLines("tmp/idle.txt") == 0){

                int child_pid = fork();

                if (child_pid == -1){
                    perror("Error on creating FCFS child process");
                    }

                if (child_pid == 0){
                    if (checkpolicy(argv[3]) == SJF) processCommandSJF(newProcess, argv[1]);
                    else if( checkpolicy(argv[3]) == FCFS) processCommandFCFS(newProcess, argv[1]);
                    _exit(0);
                }

                commandsWritten++;
            
            }
            else {
                newProcess.status = PROCESS_STATUS_IDLE;
                handleProcess(newProcess);
                commandsWritten++;
            }

            memset(buffer, 0, sizeof(buffer));
            close(fd_client);
            close(fd);
        }
    }

    close(fdIdle);
    close(fdExecuting);
    close(fdCompleted);

    return 0;
}
