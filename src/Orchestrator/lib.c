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
        sprintf(outputFileName, "tmp/%s/%d.txt", outputFolder, process.id);

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
    sprintf(outputFileName, "tmp/%s/%d.txt", outputFolder, process.id);

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


int countProcesses(Process process) {
    int count = 0;
    for (int i = 0; i < strlen(process.command); i++){
        if (process.command[i] == '|')
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


int parent(int i) {
    return (i - 1) / 2;
}


int left_child(int i) {
    return (2*i + 1);
}


int right_child(int i) {
    return (2*i + 2);
}


Process getMin(MinHeap* heap) {
    return heap->arr[0];
}


MinHeap* initHeap(int capacity) {
    MinHeap* minheap = (MinHeap*)malloc(sizeof(MinHeap));
    minheap->arr = (Process*)malloc(capacity * sizeof(Process));
    minheap->capacity = capacity;
    minheap->size = 0;
    return minheap;
} 


void resizeHeap(MinHeap* heap) {
    heap->capacity *= 2;
    heap->arr = (Process*)realloc(heap->arr, heap->capacity * sizeof(Process));
}



MinHeap* insertHeap(MinHeap* heap, Process element) {
    if (heap->size == heap->capacity) {
        resizeHeap(heap);
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


MinHeap* deleteMin(MinHeap* heap) {
    if (!heap || heap->size == 0)
        return heap;
    heap->arr[0] = heap->arr[heap->size - 1];
    heap->size--;
    heap = heapify(heap, 0);
    return heap;
}


void printHeap(MinHeap* heap) {
    printf("Min Heap:\n");
    for (int i = 0; i < heap->size; i++) {
        printf("pid: %d, timePrediction: %d -> ", heap->arr[i].pid, heap->arr[i].timePrediction);
    }
    printf("\n");
}


void freeHeap(MinHeap* heap) {
    if (!heap)
        return;
    free(heap->arr);
    free(heap);
}

void initQueue(Queue *q) {
    q->front = NULL;
    q->back = NULL;
}


void enqueue(Queue *queue, Process process) {
    Queue *new_node = (Queue *)malloc(sizeof(Queue));
    new_node->process = process;
    new_node->next = NULL;

    if (queue->back == NULL) {
        queue->front = new_node;
    } else {
        queue->back->next = new_node;
    }
    queue->back = new_node;
}


Process dequeue(Queue *queue) {
    if (queue->front == NULL) {
        printf("\n Queue is Underflow\n");
        exit(EXIT_FAILURE);
    }

    Process data = queue->front->process;
    Queue *temp = queue->front;
    queue->front = queue->front->next;
    free(temp);

    if (queue->front == NULL) {
        queue->back = NULL;
    }
    return data;
}


void printQueue(Queue *queue) {
    Queue *temp = queue->front;

    if (temp == NULL) {
        printf("\n Queue is empty\n");
        return;
    }

    while (temp != NULL) {
        printf("%s\n", temp->process.command);
        temp = temp->next;
    }
    printf("\n");
}


void status(Process **ArrayData, int ArrayDataSize) {

    int sizeIdle = 0;
    int sizeRunning = 0;
    int sizeCompleted = 0;
    Process *ArrayIdle = (Process *) malloc(sizeof(Process) * sizeIdle);
    Process *ArrayRunning = (Process *) malloc(sizeof(Process) * sizeRunning);
    Process *ArrayTerminated = (Process *) malloc(sizeof(Process) * sizeCompleted);
         

   for(int i = 0; i < ArrayDataSize; i++){
        if ((*ArrayData)[i].status == PROCESS_STATUS_IDLE){
            sizeIdle++;
            ArrayIdle = realloc(ArrayIdle, sizeof(Process) * sizeIdle);
            ArrayIdle[sizeIdle- 1] = (*ArrayData)[i];
        }
        else if ((*ArrayData)[i].status == PROCESS_STATUS_RUNNING){
            sizeRunning++;
            ArrayRunning = realloc(ArrayRunning, sizeof(Process) * sizeRunning);
            ArrayRunning[sizeRunning - 1] = (*ArrayData)[i];
        }
        else if ((*ArrayData)[i].status == PROCESS_STATUS_FINISHED){
            sizeCompleted++;
            ArrayTerminated = realloc(ArrayTerminated, sizeof(Process) * sizeCompleted);
            ArrayTerminated[sizeCompleted - 1] = (*ArrayData)[i];
        }
    }

    char buffer[1024];
    int fd_client = open(CLIENT, O_WRONLY);
    if (fd_client == -1) {
        perror("open");
        _exit(1);
    }

    //write(fd_client,"Completed\n",strlen("Completed\n"));
    printf("Completed\n");
    for (int i = 0; i < sizeCompleted; i++){
        printf("%d - %s\n", ArrayTerminated[i].id, ArrayTerminated[i].command);
        //memset(buffer, 0, sizeof(buffer));
        //snprintf(buffer, sizeof(buffer), "%d - %s\n", ArrayTerminated[i].id, ArrayTerminated[i].command);
        //write(fd_client, buffer, strlen(buffer));
    }
    memset(buffer, 0, sizeof(buffer));
    //write(fd_client,"Running\n",strlen("Running\n"));
    printf("Running\n");
    for (int i = 0; i < sizeRunning; i++){
        printf("%d - %s\n", ArrayRunning[i].id, ArrayRunning[i].command);
        //memset(buffer, 0, sizeof(buffer));
        //snprintf(buffer, sizeof(buffer), "%d - %s\n", ArrayRunning[i].id, ArrayRunning[i].command);
        //write(fd_client, buffer, strlen(buffer));
    }

    memset(buffer, 0, sizeof(buffer)); 
    //write(fd_client,"Idle\n",strlen("Idle\n"));
    printf("Idle\n");
    for (int i = 0; i < sizeIdle; i++){
        printf("%d - %s\n", ArrayIdle[i].id, ArrayIdle[i].command);
        //memset(buffer, 0, sizeof(buffer));
        //snprintf(buffer, sizeof(buffer), "%d - %s\n", ArrayIdle[i].id, ArrayIdle[i].command);
        //write(fd_client, buffer, strlen(buffer));
    }
    close(fd_client);
}