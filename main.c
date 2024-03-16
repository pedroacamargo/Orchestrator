#include <unistd.h>
#include <stdio.h>

// arg 1 - path to the file
// arg 2 - parallel tasks
// arg 3 - sched-policy
int main(int argc, char *argv[]) {

    if (argc < 4) {
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    printf("Output folder: %s\n", argv[1]);


    return 0;
}