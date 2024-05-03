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
        .id = 0,
    };
	if (strcmp(argv[1],"execute") == 0){
		newProcess.timePrediction = atoi(argv[2]);
		strcpy(newProcess.command, argv[4]);
		strcpy(newProcess.pipe, argv[3]);
		strcpy(newProcess.mode, argv[1]);
	}
	else if (strcmp(argv[1],"status") == 0){
		strcpy(newProcess.mode, argv[1]);
	}

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


	if (strcmp(argv[1],"execute") == 0){
		int fd_client = open(CLIENT, O_RDONLY);
		if (fd_client == -1) {
			perror("open");
			_exit(1);
		}

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		ssize_t bytesRead = read(fd_client, buffer, sizeof(buffer) - 1);
		if (bytesRead == -1) {
			perror("read");
			_exit(1);
		}
		buffer[bytesRead] = '\0';  
		close(fd_client);

		write(STDOUT_FILENO, buffer, bytesRead);
		unlink(CLIENT);
	}

	if (strcmp(argv[1],"status") == 0){
		int fd_client = open(CLIENT, O_RDONLY);
		if (fd_client == -1) {
			perror("open");
			_exit(1);
		}

		Process Process;
		ssize_t bytesRead;
		char output[1024];
		int anterior = 7;
		while ((bytesRead = read(fd_client, &Process, sizeof(Process))) > 0) {
			if (Process.status != anterior) {
				anterior = Process.status;
				if (Process.status == PROCESS_STATUS_RUNNING) {
					write(STDOUT_FILENO, "Running\n", strlen("Running\n"));
				} else if (Process.status == PROCESS_STATUS_IDLE) {
					write(STDOUT_FILENO, "Idle\n", strlen("Idle\n"));
				} else if (Process.status == PROCESS_STATUS_FINISHED) {
				write(STDOUT_FILENO, "Completed\n", strlen("Completed\n"));
				}
			}
			int len;
			if (Process.elapsedTime != 0.0f) {
				len = snprintf(output, sizeof(output), "ID: %d, Command: %s, Time: %f ms\n", Process.id, Process.command, Process.elapsedTime);
			}
			else {
				len = snprintf(output, sizeof(output), "ID: %d, Command: %s\n", Process.id, Process.command);
			}

			write(STDOUT_FILENO, output, len);	
		}

		if (bytesRead == -1) {
			perror("read");
			_exit(1);
		}
		
		close(fd_client);
		unlink(CLIENT);
	}
return 0;
}
