#include "global.h"


void processCommandSJF(Process process, int* idleProcesses, MinHeap** heap,Process** ArrayData) {
 
    (*idleProcesses)--;
    printf ("idleProcesses: %d\n", *idleProcesses);
    pid_t pid = fork();
    if (pid == -1) perror("Error on fork");
    if (pid == 0) childProccess(process);

    else {
        //ArrayData[process.pid - 1]->status = 1;

        int status;
        int terminated_pid = wait(&status);
        printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));

        /* if (idleProcesses > 0) {
            Process min = get_min(*heap);
            delete_minimum(*heap);
            printf("Mini: %s",min.command);


            int child_pid = fork();
            if (child_pid == -1) {
                perror("Error on creating FCFS child process");
                return;
            }

            if (child_pid == 0) {
                processCommandSJF(min, idleProcesses, heap, ArrayData);
                _exit(0);
            } 
        }   */

    }
}

