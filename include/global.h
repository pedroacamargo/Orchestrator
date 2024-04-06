#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>


#include "exec.h"
#include "SJF.h"
#include "FCFS.h"
#include "handleFiles.h"
#include "checkpolicy.h"

#define INVALID_POLICY -1
#define SJF 1
#define FCFS 2