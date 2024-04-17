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
void childProccessProduction(Process *process, int *executingProcesses);


/**
 * @brief Create a new process
 * @param processData The array of processes
 * @param commandsWritten The number of commands written
 * @param time The time to be predicted
 * @param comando The command to be executed
 * @returns a new process
*/
Process createNewProcess(Process *processData, int *commandsWritten, char *time, char *comando, int *processDataSize);


/**
 * @brief Add a process to the status
 * @param process The process to be added
 * @param processData The array of processes
 * @param processDataSize The size of the array
 * @param processesRegistered The number of processes registered
*/
void addProcessToStatus(Process process, Process *processData, int *processDataSize, int *processesRegistered);


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


/**
 * @brief Add a process to the idle queue
 * @param process The process to be added
 * @param processIdleQueue The array of idle processes
 * @param processIdleQueueSize The size of the array
 * @param idleProcesses The number of idle processes
*/
void addProcessToIdleQueue(Process process, Process *processIdleQueue, int *processIdleQueueSize, int *idleProcesses);


/**
 * @brief Get the next process that is idle
 * @param processData The array of processes
 * @param processesRegistered The number of processes registered
 * @returns the next process that is idle
*/
Process getNextProcessIdle(Process *processIdleQueue, int *idleProcesses);

#endif