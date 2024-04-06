#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>

#include "lib.h"
#include "SJF.h"
#include "FCFS.h"
#include "handleFiles.h"

/**
 * @brief Constantes para as politicas de escalonamento
*/

#define INVALID_POLICY -1 // Invalid policy alias
#define SJF 1 // Shortest Job First alias
#define FCFS 2 // First Come First Served alias
