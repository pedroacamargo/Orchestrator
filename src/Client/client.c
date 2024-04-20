#include "global.h"

int main(int argc, char *argv[]) {

	int fifo_client = mkfifo(CLIENT, 0666);
	if (fifo_client == -1) {
		perror("mkfifo");
		_exit(1);
	}

    Process newProcess = {
        .pid = 0,
        .status = PROCESS_STATUS_IDLE,
        .elapsedTime = 0.0f,
        .t1 = {0, 0},
        .t2 = {0, 0},
        .timePrediction = atoi(argv[2]),
        .id = 0,
    };
	strcpy(newProcess.command, argv[4]);
	strcpy(newProcess.pipe, argv[3]);

	int fd = open(SERVER, O_WRONLY);
	if (fd == -1) {
		perror("open");
		_exit(1);
	}

	if (write(fd, &newProcess, sizeof(Process)) == -1) {
		perror("write");
		_exit(1);
	}
	close(fd);

    int fd_client = open(CLIENT, O_RDONLY);
	if (fd_client == -1) {
		perror("open");
		_exit(1);
	}

    char buffer[256];
    // Aguardando resposta do servidor
    if (read(fd_client, buffer, sizeof(buffer)) == -1) {
        perror("read");
        _exit(1);
    }
    close(fd_client);

    strcat(buffer, "\n");
    write(STDOUT_FILENO, buffer, strlen(buffer));
	memset(buffer, 0, sizeof(buffer));
    unlink(CLIENT);
    return 0;
}
