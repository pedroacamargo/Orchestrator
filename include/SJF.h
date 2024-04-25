#ifndef SJF_H
#define SJF_H
#include "global.h"

void childProccessSJF(Process process,int* executing, MinHeap* heap, int fd_write, Process **ArrayData, char* output);

#endif