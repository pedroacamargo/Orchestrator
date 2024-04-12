#include "global.h"
//./client execute 100 -u "prog-a arg-1 (...) arg-n"
int main(int argc, char *argv[]){

    Process process = {
        .parentPid = 0,
        .time = atoi(argv[2]),
        .elapsedTime = 0.0f,
        .t1 = {0, 0},
        .t2 = {0, 0},
        };

    strcpy(process.command, argv[4]);

    int fifo_fd = open("fifo", O_WRONLY);
    if (fifo_fd == -1){
        perror("open");
        exit(1);
    }

    ssize_t bytes_written = write(fifo_fd, &process, sizeof(process));

    if (bytes_written == -1){
        perror("write");
        exit(1);
    }

    close(fifo_fd);

    return 0;
}