#include "global.h"

// arg 1 - path to the output file
// arg 2 - parallel tasks
// arg 3 - sched-policy
#define CLIENT "../data/your_file.txt"
struct timeval t1, t2;

int main(int argc, char *argv[])
{
    
    // esses comandos sao so para testar
    char *comandos[] = {"ls -l -a -h", "sleep 30", "sleep 10", "ps"};

    double elapsedTime;
    gettimeofday(&t1, NULL);

    if (argc < 4) {
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    char *politica = argv[3];

    int policy = checkpolicy(politica);
    if (policy == INVALID_POLICY) {
        printf("Invalid policy, server suspended!\n");
        return 1;
    }

    // 1 - SJF (Shortest Job First)
    if (policy == SJF) escalonamentoSJF();
    else if (policy == FCFS) escalonamentoFCFS();
    

    // EM breve isto vai sair daqui
    /* pid_t pid = fork();

    if (pid == 0) {
        printf("Child process\n");

        int res = exec(comando1);
        if (res == -1) {
            perror("Error on exec\n");
            return -1;
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
 */
    return 0;
}

// tv_sec
// 0 1 2 3 4 5 6 7 ...
// tc_usec
// 0 - 999999
// 999999 -> tv_sec + 1, tv_usec = 0