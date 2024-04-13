#ifndef HandleFiles_H
#define HandleFiles_H

#include "global.h"

void writeProcess(Process p, int fd, int command_id);
int removeProcessFromFile(const char *filename, int command_id);
int handleProcess(Process p);
int countLines(const char *filename);

#endif