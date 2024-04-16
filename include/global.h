#ifndef GLOBAL_H
#define GLOBAL_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>


/**
 * @brief Constantes para as politicas de escalonamento
*/

#define INVALID_POLICY -1 // Invalid policy alias
#define SJF 1 // Shortest Job First alias
#define FCFS 2 // First Come First Served alias
#define MAX_COMMAND_SIZE 300 // Maximum command size 300 bytes
#define SERVER "fifo_server" // Name of the server fifo
#define CLIENT "fifo_client"  // Name of the client fifo

typedef enum ProcessStatus {
    PROCESS_STATUS_ERROR = -1,
    PROCESS_STATUS_RUNNING = 0,
    PROCESS_STATUS_FINISHED = 1,
    PROCESS_STATUS_WAITING = 2,
    PROCESS_STATUS_IDLE = 3
} ProcessStatus;

/**
 * @brief Estrutura de um processo
 * @param pid Process ID
 * @param command Command to be executed
 * @param status Process status (0 - Running, 1 - Finished, 2 - Idle)
 * @param t1 Time 1
 * @param t2 Time 2
 * @param elapsedTime Elapsed time
*/
typedef struct process {
    int pid;
    int parentPid;
    char command[MAX_COMMAND_SIZE];
    ProcessStatus status;
    struct timeval t1, t2;
    double elapsedTime;
    int timePrediction;
} Process;

  

#include "handleFiles.h"
#include "lib.h"
#include "SJF.h"
#include "FCFS.h"


#endif