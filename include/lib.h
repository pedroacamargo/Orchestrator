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
void childProccess(Process process, char* outputFolder);

/**
 * @brief Print the contents of a file
 * @param filename The name of the file to be printed
*/
void print_file_contents(const char* filename);

#endif