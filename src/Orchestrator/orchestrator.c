#include "global.h"

// esses comandos sao so para testar, dps o client vai dar os comandos
char *comandos[] = {"sleep 4", "sleep 3", "sleep 4", "sleep 2", "sleep 3", "sleep 5", "sleep 1", "sleep 2", "ls -l -a -h", "sleep 1"};
 char *comandos_for_sjf[] = {"10 sleep 4", "11 sleep 4", "12 sleep 5", "13 sleep 2", "14 sleep 5", "10 sleep 5","1 ls /etc | wc -l" };
 // ./client execute 100 -u "prog-a arg-1 (...) arg-n" */

// arg 1 - path to the output file
int main(int argc, char *argv[]){

    if (argc < 4) {
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }
    //fifo(); //cria fifo para comunicacao com o client, se ja existir comentar a funcao para conseguir correr o programa
    readFifo();


    char *politica = argv[3];

    int policy = checkpolicy(politica);
    if (policy == INVALID_POLICY) {
        printf("Invalid policy, server suspended!\n");
        return 1;
    }

    if (policy == SJF) escalonamentoSJF(atoi(argv[2]), comandos_for_sjf);
    else if (policy == FCFS) escalonamentoFCFS(atoi(argv[2]), comandos);
    return 0;
}


/* 

struct timeval t1, t2;
double elapsedTime;
gettimeofday(&t1, NULL);

gettimeofday(&t2, NULL);

elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
printf("Time: %.3f ms\n", elapsedTime);

*/
