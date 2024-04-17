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


void childProccess(Process process) {
    printf("CHILD PROCESS FCFS -> Executing (%d): <%s>\n", process.pid, process.command);
    process.status = PROCESS_STATUS_RUNNING;
    handleProcess(process);

    int n = checkPipe(process.command);
    int res;

    gettimeofday(&process.t1, 0);
    if (n > 1){
        printf("PIPE DETECTED\n");
        execPipe(process.command, n); // no futuro deve retrornar um res 
    }
    else {
        res = exec(process.command);
        if (res == -1) printf("(%d): Error on exec\n", process.pid);
    } 
    gettimeofday(&process.t2, 0);

    process.elapsedTime = (process.t2.tv_sec - process.t1.tv_sec) * 1000.0;      // sec to ms
    process.elapsedTime += (process.t2.tv_usec - process.t1.tv_usec) / 1000.0;   // us to ms

    process.status = PROCESS_STATUS_FINISHED;
    handleProcess(process);

    _exit(res);
}

// ---------------------------- Child Production ----------------------------

void childProccessProduction(Process *process, int *executingProcesses) {
    printf("CHILD PROCESS FCFS -> Executing (%d): <%s>\n", process->pid, process->command);
    process->status = PROCESS_STATUS_RUNNING;
    *executingProcesses += 1;
    printProcess(*process);

    int n = checkPipe(process->command);
    int res;

    gettimeofday(&process->t1, 0);
    if (n > 1){
        printf("PIPE DETECTED\n");
        execPipe(process->command, n); // no futuro deve retrornar um res 
    }
    else {
        res = exec(process->command);
        if (res == -1) printf("(%d): Error on exec\n", process->pid);
    } 
    gettimeofday(&process->t2, 0);

    process->elapsedTime = (process->t2.tv_sec - process->t1.tv_sec) * 1000.0;      // sec to ms
    process->elapsedTime += (process->t2.tv_usec - process->t1.tv_usec) / 1000.0;   // us to ms

    process->status = PROCESS_STATUS_FINISHED;
    *executingProcesses -= 1;
    printProcess(*process);

    _exit(res);
}


Process createNewProcess(Process *processData, int *processesRegistered, char *time, char *comando, int *processDataSize) {

    printf("---> New process created\n");
    printf("--> Time predicted: %s miliseconds\n", time);
    printf("-> Command to be executed: %s\n", comando);


    Process newProcess = {
        .pid = *processesRegistered,
        .status = PROCESS_STATUS_IDLE,
        .elapsedTime = 0.0f,
        .timePrediction = atoi(time),
        .parentPid = 0,
        .t1 = {0, 0},
        .t2 = {0, 0}
    };
    
    strcpy(newProcess.command, comando);


    addProcessToStatus(newProcess, processData, processDataSize, processesRegistered);

    *processesRegistered += 1;
    printProcessesData(processData, *processesRegistered);

    printf("Processes registered: %d\n", *processesRegistered);

    return newProcess;
}

void addProcessToStatus(Process process, Process *processData, int *processDataSize, int *processesRegistered) {
    if (*processesRegistered == *processDataSize) {
        *processDataSize *= 2;
        processData = (Process *)realloc(processData, *processDataSize * sizeof(Process));
    }

    processData[*processesRegistered] = process;
    
    printf("Processes data size: %d\n", *processDataSize);
}

void printProcessesData(Process *processData, int processesRegistered) {
    printf("--------------------------\n");
    for (int i = 0; i < processesRegistered; i++) {
        printProcess(processData[i]);
    }
    printf("--------------------------\n");
}

void printProcess(Process process) {
    printf("PID: %d | Status: %d | Command: %s\n", process.pid, process.status, process.command);
}

void addProcessToIdleQueue(Process process, Process *processIdleQueue, int *processIdleQueueSize, int *idleProcesses) {
    if (*idleProcesses == *processIdleQueueSize) {
        *processIdleQueueSize *= 2;
        processIdleQueue = realloc(processIdleQueue, *processIdleQueueSize * sizeof(Process));
    }

    processIdleQueue[*idleProcesses] = process;
    *idleProcesses += 1;
}

Process getNextProcessIdle(Process *processIdleQueue, int *idleProcesses) {
    Process newProcess = processIdleQueue[0];

    // Shift all processes in the queue to the left (removing the first one)
    for (int i = 0; i < *idleProcesses; i++) {
        processIdleQueue[i] = processIdleQueue[i + 1];
    }

    // Free the last process in the queue
    // free(&processIdleQueue[*idleProcesses - 1]);
    *idleProcesses -= 1;

    return newProcess;
}