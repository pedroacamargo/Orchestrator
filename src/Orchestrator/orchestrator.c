#include "global.h"

#define DEBUG_MODE 1
#define PRODUCTION_MODE 0

// change the mode here to switch between debug and production
#define CURRENT_MODE PRODUCTION_MODE


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
    } else { // valid policy


        char buffer[256];
        ssize_t bytes_read;
        int processesRegistered = 0;

        // ------------------------------------ DEBUG MODE ------------------------------------

        if (CURRENT_MODE == DEBUG_MODE){
            printf("Orchestrator started in debug mode!\n");

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

                    newProcess.pid = processesRegistered;
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
                        if (checkpolicy(argv[3]) == SJF) processCommandSJF(newProcess);
                        else if( checkpolicy(argv[3]) == FCFS) processCommandFCFSDebug(newProcess);
                        _exit(0);
                    }

                    processesRegistered++;
                
                }
                else {
                    newProcess.status = PROCESS_STATUS_IDLE;
                    handleProcess(newProcess);
                    processesRegistered++;
                }

                memset(buffer, 0, sizeof(buffer));
                close(fd_client);
                close(fd);
            }

        } else if (CURRENT_MODE == PRODUCTION_MODE) {
            printf("Orchestrator started in production mode!\n");

            // TODO: After fixing memory leak, change this to the correct implementation
            // DinamicProcessData *dinamicProcessesData = (DinamicProcessData *)malloc(sizeof(DinamicProcessData)); // Array to store all the processes in the current session
            // dinamicProcessesData->size = 1;
            // dinamicProcessesData->length = 0;
            // dinamicProcessesData->array = (Process *)malloc(sizeof(Process) * dinamicProcessesData->size);

            int processesDataSize = 1;
            int idleProcessesQueueSize = 1;
            Process *processesData = (Process *)malloc(sizeof(Process) * processesDataSize); // Array to store all the processes in the current session
            Process *processIdleQueue = (Process *)malloc(sizeof(Process) * idleProcessesQueueSize); // Queue to store the processes that are idle
            // Process *processExecutionQueue = malloc(sizeof(Process) * atoi(argv[2])); // Queue to store the processes that are being executed

            int idleProcesses = 0;
            int executingProcesses = 0;

            // ------------------ PRODUCTION MODE ------------------
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

                printf("Waiting for new process...\n");

                memset(buffer, 0, sizeof(buffer));

                // get a new process
                if ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0){
                    buffer[bytes_read] = '\0';
                    char *time = strtok(buffer, "-");
                    char *comando = strtok(NULL, "\0");
                    

                    Process newProcess = createNewProcess(processesData, &processesRegistered, time, comando, &processesDataSize);

                    memset(buffer, 0, sizeof(buffer));
                    sprintf(buffer, "TASK %d Received", newProcess.pid);

                    if (write(fd_client, buffer, strlen(buffer)) == -1){
                        perror("write");
                        _exit(1);
                    }
                    memset(buffer, 0, sizeof(buffer));
                }
                continue; // TODO: tirar isso dps de consertar o codigo de cima

                // printProcessesData(processesData, processesRegistered);

                printf("Idle processes: %d\n", idleProcesses);
                printProcessesData(processIdleQueue, idleProcesses);

                if (executingProcesses < atoi(argv[2]) && idleProcesses == 0){
                    // If there's space in the queue, add the process to the queue

                    int child_pid = fork();
                    if (child_pid == -1){
                        perror("Error on creating FCFS child process");
                    }

                    if (child_pid == 0){
                        if (checkpolicy(argv[3]) == SJF) processCommandFCFSProduction(&processesData[processesRegistered - 1], processIdleQueue, &idleProcesses, &executingProcesses);
                        else if( checkpolicy(argv[3]) == FCFS) processCommandFCFSProduction(&processesData[processesRegistered - 1], processIdleQueue, &idleProcesses, &executingProcesses);
                        _exit(0);
                    }
                } else {
                    // Add the process to the idle queue
                    addProcessToIdleQueue(processesData[processesRegistered], processIdleQueue, &idleProcessesQueueSize, &idleProcesses);
                }

                memset(buffer, 0, sizeof(buffer));
                close(fd_client);
                close(fd);
            }



        }
    }

    close(fdIdle);
    close(fdExecuting);
    close(fdCompleted);

    return 0;
}
