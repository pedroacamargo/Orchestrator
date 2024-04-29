#include "global.h"

void childProccessFCFS(Process process,int* executing, Queue* queue, int fd_write, Process **ArrayData, char* output) {
    Process min = dequeue(queue);
    min.status = PROCESS_STATUS_RUNNING;
    (*ArrayData)[min.id - 1].status = PROCESS_STATUS_RUNNING;
    if (fork() == 0){
        min.pid = getpid();
        if (strcmp(min.pipe, "-p") == 0) execPipe(min,countProcesses(min),output);
        else exec(min,output,countProcesses(min));
        write(fd_write, &min, sizeof(Process));
        close(fd_write);
        exit(0);
    }

    else *executing+=1;  

} 
