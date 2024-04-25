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

int parent(int i); 
int left_child(int i); 
int right_child(int i);

/**
    * @brief Get the minimum element from the heap
    * @param heap 
    * @returns The minimum element
*/
Process getMin(MinHeap* heap);

/**
    * @brief Initialize the heap
    * @param capacity The capacity of the heap
    * @returns The heap
*/
MinHeap* initHeap(int capacity);

/**
    * @brief Resize the heap
    * @param heap 
*/
void resizeHeap(MinHeap* heap);

/**
    * @brief Insert an element in the heap
    * @param heap 
    * @param element The element to be inserted
    * @returns The heap
*/
MinHeap* insertHeap(MinHeap* heap, Process element);

/**
    * @brief restore the heap property
    * @param heap 
    * @param index The index to start the restore
    * @returns The heap
    
*/
MinHeap* heapify(MinHeap* heap, int index);

/**
    * @brief Delete the minimum element from the heap
    * @param heap 
    * @returns The heap
    
*/
MinHeap* deleteMin(MinHeap* heap);

/**
    * @brief Print the heap
    * @param heap
*/
void printHeap(MinHeap* heap);

/**
    * @brief Free the heap
    * @param heap 
*/
void freeHeap(MinHeap* heap) ;

/**
    * @brief Initialize the queue
    * @param q 
*/
void initQueue(Queue *q);

/**
    * @brief Enqueue a process in the queue
    * @param q 
    * @param process The process to be enqueued
*/
void enqueue(Queue *queue, Process process);


/**
    * @brief Dequeue a process from the queue
    * @param q 
    * @returns The process dequeued
*/
Process dequeue(Queue *queue);

/**
    * @brief Print the queue
    * @param q 
*/
void printQueue(Queue *queue);

/**
    * @brief status of the process
    * @param ArrayData The array of processes
    * @param ArrayDataSize The size of the array
*/
void status(Process **ArrayData, int ArrayDataSize);


void sendProcessArray(int fd, Process *array, int size);
#endif