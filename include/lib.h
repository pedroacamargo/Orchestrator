#ifndef LIB_H
#define LIB_H

/**
 * @brief Check if the policy is valid
 * @param policy The policy to be checked
*/
int checkpolicy(char* policy);

/**
 * @brief Execute a command with fork
 * @param command The command to be executed
 * @returns 
 * -1 if there was an error
 * res if the command was executed successfully
*/
int exec(Process process, char* outputFolder, int number_processes);
/**
 * @brief Execute a command/pipe with fork
 * @param command The command to be executed and @param number_processes 
 * @returns no futuro devera ser igual ao exec
*/
void execPipe(Process process, int number_processes, char* outputFolder);
/**
 * @brief Check if the command is a pipe
 * @param command The command to be executed
 * @returns number os pipes in the command, default is 1
 * 
*/
int countProcesses(Process process);
/**
 * @brief take away the space
 * @param command The command to be executed
 * @returns the command without the space
 * 
*/
char *space(char *str);

/**
 * @brief Extract the process from the command
 * @param command The command to be executed, @param number_processes and @param processArray- array pointers to the processes 
*/
void extractProcessPipe(const char *command, int number_processes, char **processArray);

/**
 * @brief Add a process to the status
 * @param process The process to be added
 * @param processData The array of processes
 * @param processDataSize The size of the array
 * @param processesRegistered The number of processes registered
*/
void addProcessToStatus(Process process, Process **processData,int *processesRegistered);


/**
 * @brief Print the processes data
 * @param processData The array of processes
 * @param processesRegistered The number of processes registered
*/
void printProcessesData(Process *processData, int processesRegistered);

/**
 * @brief Print a process
 * @param process The process to be printed
*/
void printProcess(Process process);
/// heaps a entrar em açao

int parent(int i);
int left_child(int i);
int right_child(int i);
Process get_min(MinHeap* heap);
MinHeap* initHeap(int capacity);
void resize_heap(MinHeap* heap);
MinHeap* insert_minheap(MinHeap* heap, Process element);
MinHeap* heapify(MinHeap* heap, int index);
MinHeap* delete_minimum(MinHeap* heap);
void print_heap(MinHeap* heap);
void free_minheap(MinHeap* heap) ;
void enqueue(Queue *queue, Process process);
void initQueue(Queue *q);
Process dequeue(Queue *queue);
void display(Queue *queue);

#endif