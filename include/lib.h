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
int exec(const char* command);

/**
 * @brief Execute a command/pipe with fork
 * @param command The command to be executed and @param number_processes 
 * @returns no futuro devera ser igual ao exec
*/
void execPipe(const char* command, int number_processes);

/**
 * @brief Check if the command is a pipe
 * @param command The command to be executed
 * @returns number os pipes in the command, default is 1
 * 
*/
int checkPipe(const char *command);

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
 * @brief Execute a command with fork
 * @param command The command to be executed
*/
void childProccess(Process process);

/**
 * @brief Create a new process
 * @param processData The array of processes
 * @param commandsWritten The number of commands written
 * @param time The time to be predicted
 * @param comando The command to be executed
 * @returns a new process
*/
Process createNewProcess(int processesRegistered, char *time, char *comando);

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
MinHeap* init_minheap(int capacity);
void resize_heap(MinHeap* heap);
MinHeap* insert_minheap(MinHeap* heap, Process element);
MinHeap* heapify(MinHeap* heap, int index);
MinHeap* delete_minimum(MinHeap* heap);
void print_heap(MinHeap* heap);
void free_minheap(MinHeap* heap) ;




#endif