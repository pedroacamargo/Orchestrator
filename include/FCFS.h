#ifndef FCFS_H
#define FCFS_H
#include "global.h"

void processCommandFCFSDebug(Process process);

void processCommandFCFSProduction(Process *process, Process *processIdleQueue ,int *idleProcesses, int *executingProcesses);

#endif