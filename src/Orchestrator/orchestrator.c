#include "global.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    int fdCompleted = open("tmp/completed.txt", O_CREAT | O_WRONLY | O_APPEND, 0666);
    if (fdCompleted == -1) {
        perror("Open completed.txt");
        return 1;
    } 

    // Criando o FIFO do servidor
    int fd = mkfifo(SERVER, 0666);
    if (fd == -1) {
        perror("Open fifo server");
        _exit(1);
    }

    // Verificando a política
    if (checkpolicy(argv[3]) == INVALID_POLICY) {
        printf("Invalid policy, server suspended!\n");
        return 1;
    } else {
        char buffer[256];
        int id = 1;
        int executingProcesses = 0;
        int idleProcesses = 0;
        Process newProcess;

        printf("Orchestrator started!\n");

        int ArrayDataSize = 0;
        Process *ArrayData = (Process *) malloc(sizeof(Process) * ArrayDataSize);
        MinHeap *heap = init_minheap(10);

        while (1) {
            int fd_server = open(SERVER, O_RDONLY);
            if (fd_server == -1) {
                perror("open");
                _exit(1);
            }

            if (read(fd_server, &newProcess, sizeof(Process)) == -1) {
                perror("read");
                _exit(1);
            }
            close(fd_server);
            if (newProcess.pid != 0) {
                printf("Received aaaaaaaaaaaaaaaaaaaprocess %d\n", newProcess.id);
            }
            // colocar nas estruturas de dados ********************************
            newProcess.id = id;
            addProcessToStatus(newProcess, &ArrayData, &ArrayDataSize);
            printProcessesData(ArrayData, ArrayDataSize);
            insert_minheap(heap, newProcess);
            Process min = get_min(heap); 
            //*****************************************************************

            int fd_client = open(CLIENT, O_WRONLY);
            if (fd_client == -1) {
                perror("open");
                _exit(1);
            }

            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "TASK %d Received", newProcess.id);

            if (write(fd_client, buffer, strlen(buffer)) == -1) {
                perror("write");
                _exit(1);
            }

            id++;
            idleProcesses++;

            //**************************************************************
            pid_t pid = fork();
            if (pid == -1) {
                perror("Error on fork");
            } else if (pid == 0) {
                newProcess.pid = getpid();
                newProcess.status = PROCESS_STATUS_RUNNING;
                ArrayData[ArrayDataSize - 1].status = PROCESS_STATUS_RUNNING;
                executingProcesses++;
                idleProcesses--;

                // Abre o FIFO do servidor para escrita
                int fd_server_write = open(SERVER, O_WRONLY);
                if (fd_server_write == -1) {
                    perror("open");
                    _exit(1);
                }

                // Envia a tarefa de volta para o servidor com o PID
                write(fd_server_write, &newProcess, sizeof(Process));
                close(fd_server_write);

                // Executa a tarefa
                childProccess(newProcess);
                exit(0);
            } else {

                if (newProcess.pid != 0) {
                
                    pid_t specific_pid_to_wait = newProcess.pid;
                    int status;
                    waitpid(specific_pid_to_wait, &status, 0);

                    if (WIFEXITED(status)) {
                        newProcess.status = PROCESS_STATUS_FINISHED;
                        ArrayData[ArrayDataSize - 1].status = PROCESS_STATUS_FINISHED;
                        executingProcesses--;
                        char resultado[256];
                        sprintf(resultado, "TASK %d Finished\n", newProcess.id);
                        write(fdCompleted, resultado, strlen(resultado));
                    }
                }
            }

            close(fd_client);
        }

    }

    close(fdCompleted);
    return 0;
}
