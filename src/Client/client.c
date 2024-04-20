#include "global.h"

int main(int argc, char *argv[]) {
    if(argc < 2) {
        printf("Usage: %s <command> [args]\n", argv[0]);
        return 1;
    }

    if(strcmp(argv[1], "status") == 0) {
        write(STDOUT_FILENO, "Executing\n", 10);
        print_file_contents("tmp/executing.txt");
        write(STDOUT_FILENO, "\n", 1);

        write(STDOUT_FILENO, "Scheduled\n", 10);
        print_file_contents("tmp/idle.txt");
        write(STDOUT_FILENO, "\n", 1);

        write(STDOUT_FILENO, "Completed\n", 10);
        print_file_contents("tmp/completed.txt");
        write(STDOUT_FILENO, "\n", 1);

    } else if(strcmp(argv[1], "execute") == 0) {
        if(argc < 5) {
            printf("Usage: %s execute <time> -u/p <task>\n", argv[0]);
            return 1;
        }

        int fifo_client = mkfifo(CLIENT, 0666);
        if(fifo_client == -1) {
            perror("mkfifo");
            _exit(1);
        }

        int fd = open(SERVER, O_WRONLY);
        if(fd == -1) {
            perror("open");
            _exit(1);
        }

        char BUFFER[256];
        memset(BUFFER, 0, sizeof(BUFFER));
        snprintf(BUFFER, sizeof(BUFFER), "%s-%s", argv[2], argv[4]);

        if (write(fd, BUFFER, sizeof(BUFFER)) == -1) {
            perror("write");
            _exit(1);
        }
        close(fd);
        memset(BUFFER, 0, sizeof(BUFFER));

        int fd_client = open(CLIENT, O_RDONLY);
        if(fd_client == -1) {
            perror("open");
            _exit(1);
        }

        if(read(fd_client, BUFFER, sizeof(BUFFER)) == -1) {
            perror("read");
            _exit(1);
        }

        strcat(BUFFER, "\n");
        write(STDOUT_FILENO, BUFFER, strlen(BUFFER));
        memset(BUFFER, 0, sizeof(BUFFER));
        close(fd_client);
        unlink(CLIENT);
    } else {
        printf("Unknown command: %s\n", argv[1]);
        return 1;
    }

    return 0;
}