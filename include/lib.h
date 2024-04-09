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
 * @brief Extract the time from the command
 * @param command The command to be executed
 * @returns the command without the time to be executed
*/
char* extractTimeProcess(const char* command);

#endif