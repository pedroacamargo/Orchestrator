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

void execPipe(const char *command, int number_processes){

    char** processArray = malloc((number_processes) * sizeof(char*));
    
    extractProcessPipe(command, number_processes, processArray);

    for(int i = 0; i < number_processes; i++){
        printf("Process %d: %s\n", i, processArray[i]); // so para debug 
    }

    int fd[2];
    pipe(fd);
    if (fork() == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        exec(processArray[0]);                 //implementado como esta no necc e como voces fizeram nas aulas 
        _exit(-1);                             //pode ser melhorado, quem quiser esta a vontade
    }	                   					   // deve retornar o res mas ns como se faz, uma vez
    close(fd[1]);                              //que no exec isso vem do wait e aqui nao temos wait "normal"
    wait(NULL);

    if (fork() == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        exec(processArray[1]);
        _exit(-1);
    }
    close(fd[0]);
    wait(NULL);

    // Free the allocated memory
    for (int i = 0; i < number_processes ; i++) {
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


int fifo(){
    int res = mkfifo("fifo", 0666);
        if (res == -1) {
            perror("mkfifo");
            exit(1);
        }
    return res;
}

int writeFifo(){
    int fifo_fd = open("fifo", O_WRONLY);
    printf("FIFO opened to write...\n");
    if (fifo_fd == -1) {
        perror("open");
        exit(1);
    }
    char BUFFER[256];
    size_t read_bytes;

    read_bytes = read(STDIN_FILENO, BUFFER, sizeof(BUFFER));

    if (write(fifo_fd, BUFFER, read_bytes) == -1) {
        perror("write");
        exit(1);
    }

    BUFFER[read_bytes] = '\0';
    printf("Sent: %s", BUFFER);

    
    close(fifo_fd);

    return 0;
} 

void run(char *output_folder, int parallel_tasks, char *sched_policy, char *command) {
    char *politica = sched_policy;
    int policy = checkpolicy(politica);
    if (policy == INVALID_POLICY){
        printf("Invalid policy, server suspended!\n");
    }
    if (policy == SJF)
        escalonamentoSJF(parallel_tasks, command);
    
    /* else if (policy == FCFS)
    escalonamentoFCFS(parallel_tasks, command); */
}
