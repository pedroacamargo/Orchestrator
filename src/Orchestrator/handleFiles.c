#include "global.h"

#define BUFFER_SIZE 1024

void copyFileContent(int fd_origin, int fd_destiny) {
    char buffer[BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(fd_origin, buffer, BUFFER_SIZE)) > 0) {
        write(fd_destiny, buffer, bytesRead);
    }
}

int handleFiles(const char* origin_path, const char* destiny_path) {
    int fd_origin = open(origin_path, O_RDONLY);
    if (fd_origin == -1) {
        perror("Error opening origin file");
        return -1;
    }

    int fd_destiny = open(destiny_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_destiny == -1) {
        perror("Error opening destiny file");
        close(fd_origin);
        return -1;
    }

    copyFileContent(fd_origin, fd_destiny);
    printf("Tijolo\n");

    close(fd_origin);
    close(fd_destiny);

    return 0;
}