#include "global.h"
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    int fdCompleted = open("tmp/completed.txt", O_CREAT | O_WRONLY, 0666);
    if (fdCompleted == -1) {
        perror("Open completed.txt");
        return 1;
    } 

    int create = mkfifo(SERVER, 0666);
    if (create == -1) {
        perror("Open fifo server");
        _exit(1);
    }

    int fd = open(SERVER, O_RDONLY);
    int fd_write = open(SERVER, O_WRONLY);

    if (checkpolicy(argv[3]) == INVALID_POLICY) {
        printf("Invalid policy, server suspended!\n");
        return 1;
    } else {    

        char buffer[256];
        int id = 1;
        int executing = 0;
        Process newProcess;


        int ArrayDataSize = 0;
        Process *ArrayData = (Process *) malloc(sizeof(Process) * ArrayDataSize);
        MinHeap *heap = init_minheap(10);

        printf("Orchestrator started!\n");
        while (read(fd, &newProcess, sizeof(Process)) > 0) {

            if (newProcess.status == PROCESS_STATUS_IDLE){
                newProcess.id = id;
                addProcessToStatus(newProcess, &ArrayData, &ArrayDataSize);
                insert_minheap(heap, newProcess);
                id++;


                //************************** send msg to client **************************
                memset(buffer, 0, sizeof(buffer));
                sprintf(buffer, "TASK %d Received", newProcess.id);

                int fd_client = open(CLIENT, O_WRONLY);
                if (fd_client == -1) {
                    perror("open");
                    _exit(1);
                }

                if (write(fd_client, buffer, strlen(buffer)) == -1) {
                    perror("write");
                    _exit(1);   
                }
                memset(buffer, 0, sizeof(buffer));
                close(fd_client);
                //************************** send msg to client **************************

                //**************************Child Production**************************
                if (executing < atoi(argv[2]) && heap->size > 0) {
                    childProccess(newProcess, &executing, heap, fd_write, &ArrayData);
                } 
                //**************************Child Production**************************


            printProcessesData(ArrayData, ArrayDataSize);
            }

            // ********************* wait for process to finish *********************
            else if(newProcess.status == PROCESS_STATUS_RUNNING){

                int status;
                waitpid(newProcess.pid, &status, 0);
                newProcess.status = PROCESS_STATUS_FINISHED;
                printf("Process %d finished\n", newProcess.id);
                ArrayData[newProcess.id - 1].status = PROCESS_STATUS_FINISHED;
                ArrayData[newProcess.id - 1].pid = newProcess.pid;
                snprintf(buffer, sizeof(buffer), "TASK %d Finished\n", newProcess.id);
                write(fdCompleted, buffer, strlen(buffer));            
                executing--;
                // ********************* wait for process to finish *********************

                // **************************Child Production**************************
                if (executing < atoi(argv[2]) && heap->size > 0) {
                    childProccess(newProcess, &executing, heap, fd_write, &ArrayData);
                } 
                // **************************Child Production**************************
            }

        }
        close(fd);
        close(fd_write);
        unlink(SERVER);

    }
    close(fdCompleted);
    return 0;
}


