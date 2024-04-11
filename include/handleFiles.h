#ifndef HandleFiles_H
#define HandleFiles_H

#include "global.h"

void writeProcess(Process p, int fd, int command_id);
int removeProcessFromFile(const char *filename, int command_id);
int handleFiles(char queue[][100], int i, ProcessStatus status);
int handleProcess(Process p, int command_id);
int countLines(const char *filename);

#endif