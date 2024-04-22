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


void childProccess(Process process,int* executing, MinHeap* heap, int fd_write, Process **ArrayData) {
    Process min = get_min(heap);
    delete_minimum(heap);
    if (fork() == 0){
        min.pid = getpid();
        exec(min.command);
        executing++;
        min.status = PROCESS_STATUS_RUNNING;
        (*ArrayData)[min.id - 1].status = PROCESS_STATUS_RUNNING;
        (*ArrayData)[min.id - 1].pid = min.pid;
        write(fd_write, &min, sizeof(Process));
        close(fd_write);
        exit(0);
    }
    else *executing+=1;
} 

// ---------------------------- Child Production ----------------------------


/* Process createNewProcess(int id, char *time, char *comando) {

    Process newProcess = {
        .pid = id,
        .status = PROCESS_STATUS_IDLE,
        .elapsedTime = 0.0f,
        .timePrediction = atoi(time),
        .t1 = {0, 0},
        .t2 = {0, 0},
        .id = 0
    };
    
    strcpy(newProcess.command, comando);
    return newProcess;
} */


void printProcessesData(Process *processData, int processesRegistered) {
    printf("--------------------------\n");
    for (int i = 0; i < processesRegistered; i++) {
        printProcess(processData[i]);
    }
    printf("--------------------------\n");
}


void printProcess(Process process) {
    printf("ID: %d | Status: %d | Command: %s | PID: %d\n", process.id, process.status, process.command, process.pid);
}


void addProcessToStatus(Process process, Process **processData,int *processesRegistered) {
    *processesRegistered += 1;

    *processData = realloc(*processData, *processesRegistered * sizeof(Process));
    (*processData)[*processesRegistered - 1] = process;
} 

/// heaps a entrar em acao 



int parent(int i) {
    return (i - 1) / 2;
}

int left_child(int i) {
    return (2*i + 1);
}

int right_child(int i) {
    return (2*i + 2);
}

Process get_min(MinHeap* heap) {
    return heap->arr[0];
}


MinHeap* init_minheap(int capacity) {
    MinHeap* minheap = (MinHeap*)malloc(sizeof(MinHeap));
    minheap->arr = (Process*)malloc(capacity * sizeof(Process));
    minheap->capacity = capacity;
    minheap->size = 0;
    return minheap;
}


void resize_heap(MinHeap* heap) {
    heap->capacity *= 2;
    heap->arr = (Process*)realloc(heap->arr, heap->capacity * sizeof(Process));
}



MinHeap* insert_minheap(MinHeap* heap, Process element) {
    if (heap->size == heap->capacity) {
        resize_heap(heap);
    }
    heap->size++;
    int i = heap->size - 1;
    while (i && element.timePrediction < heap->arr[parent(i)].timePrediction) {
        heap->arr[i] = heap->arr[parent(i)];
        i = parent(i);
    }
    heap->arr[i] = element;
    return heap;
}


MinHeap* heapify(MinHeap* heap, int index) {
    int smallest = index;
    int left = left_child(index);
    int right = right_child(index);
    
    if (left < heap->size && heap->arr[left].timePrediction < heap->arr[index].timePrediction)
        smallest = left;
    if (right < heap->size && heap->arr[right].timePrediction < heap->arr[smallest].timePrediction)
        smallest = right;
    if (smallest != index) {
        Process temp = heap->arr[index];
        heap->arr[index] = heap->arr[smallest];
        heap->arr[smallest] = temp;
        heap = heapify(heap, smallest);
    }
    return heap;
}


MinHeap* delete_minimum(MinHeap* heap) {
    if (!heap || heap->size == 0)
        return heap;
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heap = heapify(heap, 0);
    return heap;
}


void print_heap(MinHeap* heap) {
    printf("Min Heap:\n");
    for (int i = 0; i < heap->size; i++) {
        printf("pid: %d, timePrediction: %d -> ", heap->arr[i].pid, heap->arr[i].timePrediction);
    }
    printf("\n");
}



void free_minheap(MinHeap* heap) {
    if (!heap)
        return;
    free(heap->arr);
    free(heap);
}
