#include "global.h"

int checkpolicy(char* policy){
    if (strcmp(policy, "SJF") == 0) return SJF;
    else if (strcmp(policy, "FCFS") == 0) return FCFS;

    return INVALID_POLICY;
}

int exec(const char* command) {

	int res = -1, i = 0;
	char *exec_args[20], *string, *cmd, *tofree;

	tofree = cmd = strdup(command);


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

void execPipe(const char *command, int number_processes) {
    char **processArray = malloc((number_processes) * sizeof(char*));

    extractProcessPipe(command, number_processes, processArray);

    // Create pipes
    int pipes[number_processes - 1][2];
    for (int i = 0; i < number_processes - 1; i++) {
        pipe(pipes[i]);
    }

    for (int i = 0; i < number_processes; i++) {
        printf("Process %d: %s\n", i, processArray[i]); // for debugging
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

            exec(processArray[i]);
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
char *retira_new_line(char *str) {
    char *new = malloc(strlen(str) + 1); 
    if (new == NULL) {
        return NULL;
    }
    int i;
    for(i = 0; str[i] != '\n'; i++) {
        new[i] = str[i];
    }
    new[i] = '\0'; 
    return new;
}

char* extractTimeProcess(char* command) {
    char *space = strchr(command, ' ');
    if (space == NULL) {
        return NULL;
    }
    while (*space == ' ') {
        space++;
    }
    return strdup(space);
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
    processArray[index] = NULL;  

}





