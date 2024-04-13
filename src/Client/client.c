#include "global.h"

int main(int argc, char *argv[]) {
    int fifo_fd = open("fifo", O_WRONLY);
    if (fifo_fd == -1){
        perror("open");
        exit(1);
    }

    char BUFFER[256];
    memset(BUFFER, 0, sizeof(BUFFER));
    snprintf(BUFFER, sizeof(BUFFER), "%s-%s", argv[2], argv[4]);
    ssize_t bytes_written = write(fifo_fd, BUFFER, sizeof(BUFFER));

    if (bytes_written == -1){
        perror("write");
        exit(1);
    }

    close(fifo_fd);
    // writeFifo();
    return 0;
}
