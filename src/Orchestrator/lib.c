#include "global.h"

int checkpolicy(char* policy){
    if (strcmp(policy, "SJF") == 0) return SJF;
    else if (strcmp(policy, "FCFS") == 0) return FCFS;

    return INVALID_POLICY;
}

int exec(Process process, char* outputFolder, int number_processes) {

    if (number_processes < 2) {
        char outputFolderWithPath[256];
        sprintf(outputFolderWithPath, "tmp/%s", outputFolder);
        mkdir(outputFolderWithPath, 0777);

        char outputFileName[256];
        sprintf(outputFileName, "tmp/%s/%d.txt", outputFolder, process.pid);

        int fd = open(outputFileName, O_WRONLY | O_CREAT, 0644);
        if (fd == -1) perror("Error opening file");


        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Error duplicating file descriptor to stdout");
            close(fd);
        
        }

        if (dup2(fd, STDERR_FILENO) == -1) {
            perror("Error duplicating file descriptor to stderr");
            close(fd);
        
        }

        close(fd);
    } 
   
	int res = -1, i = 0;
	char *exec_args[20], *string, *cmd, *tofree;

	tofree = cmd = strdup(process.command);


	while((string = strsep(&cmd," ")) != NULL){
	   exec_args[i]=string;
	   i++;
	}

	exec_args[i] = NULL;

	pid_t pid = fork();
	switch (pid) {
		case -1: {
			perror("Error on fork\n");
			break;
		}
		case 0: {
			execvp(exec_args[0], exec_args);
			_exit(-1); // So executa isso quando dá erro
			break;
		}
		default: {
			int status;
			waitpid(pid, &status, 0);


			res = WEXITSTATUS(status);
			if (res == 255) {
				perror("Error on execvp\n");
				return -1;
			}
			break;
		}
	}

	free(tofree);

	return res;
}

void execPipe(Process process, int number_processes, char* outputFolder) {

    char outputFolderWithPath[256];
    sprintf(outputFolderWithPath, "tmp/%s", outputFolder);
    mkdir(outputFolderWithPath, 0777);

    char outputFileName[256];
    sprintf(outputFileName, "tmp/%s/%d.txt", outputFolder, process.pid);

    int fd = open(outputFileName, O_WRONLY | O_CREAT, 0644);
    if (fd == -1) perror("Error opening file");


    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("Error duplicating file descriptor to stdout");
        close(fd);
    
    }

    if (dup2(fd, STDERR_FILENO) == -1) {
        perror("Error duplicating file descriptor to stderr");
        close(fd);
    
    }

    close(fd);

    char **processArray = malloc((number_processes) * sizeof(char*));

    extractProcessPipe(process.command, number_processes, processArray);


    // Create pipes
    int pipes[number_processes - 1][2];
    for (int i = 0; i < number_processes - 1; i++) {
        pipe(pipes[i]);
    }

    // Execute processes
    for (int i = 0; i < number_processes; i++) {
        if (fork() == 0) {
            // Redirect input
            if (i != 0) {
                dup2(pipes[i - 1][0], 0);
            }

            // Redirect output
            if (i != number_processes - 1) {
                dup2(pipes[i][1], 1);
            }

            // Close all pipe fds
            for (int j = 0; j < number_processes - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            Process newProcess;
            newProcess.pid = process.pid;
            strcpy(newProcess.command, processArray[i]);

            exec(newProcess, outputFolder, number_processes);
            _exit(EXIT_FAILURE);
        }
    }

    // Close all pipe fds
    for (int i = 0; i < number_processes - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all processes to finish
    for (int i = 0; i < number_processes; i++) {
        wait(NULL);
    }

    // Free allocated memory
    for (int i = 0; i < number_processes; i++) {
        free(processArray[i]);
    }
} 

int checkPipe(const char *command){
    int count = 0;
    for (int i = 0; i < strlen(command); i++){
        if (command[i] == '|')
            count++;
    }
    return count + 1;
}


char *space(char *str) {
    while (*str == ' ') {
        str++;
    }
    char *end = str + strlen(str) - 1;
    while (*end == ' ' || *end == '\n') {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}


void extractProcessPipe(const char *command, int number_processes, char **processArray) {

    char *command_copy = strdup(command); 
    int index = 0;
    char *token = strtok(command_copy, "|");
    while (token != NULL && index < number_processes) {      
        token = space(token);
        processArray[index] = strdup(token);
        index++;
        token = strtok(NULL, "|");
    }

}


void childProccess(Process process, char* outputFolder) {
    printf("CHILD PROCESS FCFS -> Executing (%d): <%s>\n", process.pid, process.command);
    process.status = PROCESS_STATUS_RUNNING;
    handleProcess(process);

    int n = checkPipe(process.command);
    int res;

    gettimeofday(&process.t1, 0);
    if (n > 1){
        printf("PIPE DETECTED\n");
        execPipe(process, n, outputFolder); // no futuro deve retrornar um res 
    }
    else {
        res = exec(process, outputFolder,n);
        if (res == -1) printf("(%d): Error on exec\n", process.pid);
    } 
    gettimeofday(&process.t2, 0);

    process.elapsedTime = (process.t2.tv_sec - process.t1.tv_sec) * 1000.0;      // sec to ms
    process.elapsedTime += (process.t2.tv_usec - process.t1.tv_usec) / 1000.0;   // us to ms

    process.status = PROCESS_STATUS_FINISHED;
    handleProcess(process);

    _exit(res);
}

