#include "global.h"

void childProccessSJF(Process process,int* executing, MinHeap* heap, int fd_write, Process **ArrayData) {
    Process min = get_min(heap);
    delete_minimum(heap);
    min.status = PROCESS_STATUS_RUNNING;
    (*ArrayData)[min.id - 1].status = PROCESS_STATUS_RUNNING;
    if (fork() == 0){
        min.pid = getpid();
        exec(min.command);
        write(fd_write, &min, sizeof(Process));
        close(fd_write);
        exit(0);
    }

    else *executing+=1;
} 
