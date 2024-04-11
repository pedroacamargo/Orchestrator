#ifndef HandleFiles_H
#define HandleFiles_H

#include "global.h"

void writeProcess(Process p, int fd, int command_id);
int removeProcessFromFile(const char *filename, int command_id);
int handleFiles(char queue[][100], int i, ProcessStatus status);

#endif