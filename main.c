#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/time.h>

// arg 1 - path to the output file
// arg 2 - parallel tasks
// arg 3 - sched-policy
#define CLIENT "data/your_file.txt"
struct timeval t1, t2;

int main(int argc, char *argv[]) {
    double elapsedTime;
    gettimeofday(&t1, NULL);

    if (argc < 4) {
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        printf("Child process\n");

        int res = execlp("cat", "cat", CLIENT, NULL); 
        if (res == -1) {
            perror("execlp");
        }
    } else {
        int status;
        int terminated_pid = wait(&status);
        printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));
        printf("Parent process\n");
        gettimeofday(&t2, NULL);
    }

    
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    printf("Time: %.3f ms\n", elapsedTime);

    return 0;
}


// tv_sec 
// 0 1 2 3 4 5 6 7 ...
// tc_usec
// 0 - 999999
// 999999 -> tv_sec + 1, tv_usec = 0