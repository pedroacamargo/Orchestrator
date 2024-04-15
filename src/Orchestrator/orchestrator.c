#include "global.h"
#include "FCFS.h"
#include "SJF.h"

char *comandos[] = {"sleep 4", "sleep 3", "sleep 4", "sleep 2", "sleep 3", "sleep 5", "sleep 1", "sleep 2", "ls -l -a -h", "sleep 1"};
char *comandos_for_sjf[] = {"10 sleep 4", "11 sleep 4", "12 sleep 5", "13 sleep 2", "14 sleep 5", "10 sleep 5", "1 ls /etc | wc -l"};

int main(int argc, char *argv[])
{

    if (argc < 4)
    {
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    // Create the FIFO
    int fd = mkfifo(SERVER, 0666);
    if (fd == -1)
    {
        perror("open");
        _exit(1);
    }

    int fdIdle = open("tmp/idle.txt", O_CREAT | O_RDONLY, 0666);
    if (fdIdle == -1)
    {
        perror("open");
        return 1;
    }
    int fdScheduled = open("tmp/scheduled.txt", O_CREAT | O_RDONLY, 0666);
    if (fdScheduled == -1)
    {
        perror("open");
        return 1;
    }
    int fdExecuting = open("tmp/executing.txt", O_CREAT | O_RDONLY, 0666);
    if (fdExecuting == -1)
    {
        perror("open");
        return 1;
    }
    int fdCompleted = open("tmp/completed.txt", O_CREAT | O_RDONLY, 0666);
    if (fdCompleted == -1)
    {
        perror("open");
        return 1;
    }

    printf("Orchestrator started!\n");

    char buffer[256];
    ssize_t bytes_read;

    int policy = checkpolicy(argv[3]);
    if (policy == INVALID_POLICY)
    {
        printf("Invalid policy, server suspended!\n");
        return 1;
    }

    if (policy == SJF)
    {
        int commandsWritten = 1;
        printf("=============== SJF ===============\n");
        while (1)
        {

            int fd = open(SERVER, O_RDONLY);
            if (fd == -1)
            {
                perror("open");
                _exit(1);
            }

            int fd_client = open(CLIENT, O_WRONLY);
            if (fd_client == -1)
            {
                perror("open");
                _exit(1);
            }
            
            
            Process newProcess;
            memset(&newProcess, 0, sizeof(Process));
            printf("Waiting for new process...\n");

            if ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
            {
                
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

                if (write(fd_client, buffer, strlen(buffer)) == -1)
                {
                    perror("write");
                    _exit(1);
                }
                memset(buffer, 0, sizeof(buffer));
            }

            if (countLines("tmp/executing.txt") < atoi(argv[2]) && countLines("tmp/idle.txt") == 0)
            { 

                    printf("ola eu sou  foda\n");
                    int child_pid = fork();
                    if (child_pid == -1)
                    {
                        perror("Error on creating FCFS child process\n");
                    }

                    if (child_pid == 0)
                    {
                        processCommand_2(newProcess);
                        _exit(0);
                    }
                    commandsWritten++;
            }           
             else {
                newProcess.status = PROCESS_STATUS_IDLE;
                handleProcess(newProcess);
                commandsWritten ++;
            }
            
            memset(buffer, 0, sizeof(buffer));
            close(fd_client);
            close(fd);
        }
    }

    close(fdIdle);
    close(fdScheduled);
    close(fdExecuting);
    close(fdCompleted);

    return 0;
}
