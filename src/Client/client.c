#include "global.h"

int main(int argc, char *argv[]) {

    int fifo_client = mkfifo(CLIENT, 0666);
	if (fifo_client == -1) {
		perror("mkfifo");
		_exit(1);
	}

    int fd = open(SERVER, O_WRONLY);
	if (fd == -1) {
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
	if (fd_client == -1) {
		perror("open");
		_exit(1);
	}


	if (read(fd_client, BUFFER, sizeof(BUFFER)) == -1) {
    	perror("read");
    	_exit(1);
	}

	strcat(BUFFER, "\n");
	write(STDOUT_FILENO, BUFFER, strlen(BUFFER));
	memset(BUFFER, 0, sizeof(BUFFER));
	close(fd_client);
	unlink(CLIENT);
    return 0;
}

