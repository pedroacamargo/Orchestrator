#ifndef FCFS_H
#define FCFS_H
#include "global.h"

void childProccessFCFS(Process process,int* executing, Queue* queue, int fd_write, Process **ArrayData, char* output);

#endif