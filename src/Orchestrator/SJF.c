#include "global.h"

int compare(const void *a, const void *b){
    // Cast the void pointers to char pointers
    const char *str_a = *(const char **)a;
    const char *str_b = *(const char **)b;

    int num_a = atoi(str_a);
    int num_b = atoi(str_b);
    return num_a - num_b;
}

void childProccessSJF(int childPid, int index, char array[][100], int parentPid){
    Process process;
    process.pid = childPid;
    strcpy(process.command, array[index]);
    process.status = PROCESS_STATUS_RUNNING;
    process.parentPid = parentPid;
    
    printf("(%d): Executando comando <%s>\n", process.pid, process.command);

    gettimeofday(&process.t1, 0);
    int res = exec(array[index]);
    gettimeofday(&process.t2, 0);

    process.elapsedTime = (process.t2.tv_sec - process.t1.tv_sec) * 1000.0;      // sec to ms
    process.elapsedTime += (process.t2.tv_usec - process.t1.tv_usec) / 1000.0;   // us to ms
    printf("Time: %.3f ms\n", process.elapsedTime);

    if (res == -1) printf("CHILD (%d): Error on exec\n", childPid);
    _exit(res);
    //Moniz Aqui terás de chamar as funções que estao no handleFiles.c para escreveres no ficheiro de output
    //Os processos que terminaram e os respetetivos tempos de execução reais (process.elapsedTime)
    //Duas vezes chamas as funcoes do HandleFiles.c, uma para escrever os processos que terminaram 
    //e outra para escrever os processos que ainda estão a correr (process.status = PROCESS_STATUS_RUNNING)
}

char* extract_process(const char* process) {
    const char* space = strchr(process, ' ');
    return strdup(space + 1);
}

int escalonamentoSJF(int parallelTasks, char *comandos[]){
    
    int num_comandos = 0;
    while (comandos[num_comandos] != NULL){  // size of commands
        num_comandos++;
    }

    int arraySize = num_comandos;
    char array[num_comandos][100];
    ProcessStatus statusArray[num_comandos];
    int actualProcessIndex = 0, finishedProcesses = 0;

    qsort(comandos, num_comandos, sizeof(char *), compare);

    for (int i = 0; i < arraySize; i++){
        strcpy(array[i],extract_process(comandos[i]));
        if (i < parallelTasks)
            statusArray[i] = PROCESS_STATUS_WAITING;
        else
            statusArray[i] = PROCESS_STATUS_IDLE;
    }

    while (actualProcessIndex < arraySize) {
        while (actualProcessIndex < num_comandos && statusArray[actualProcessIndex] == PROCESS_STATUS_WAITING){
            pid_t pid = fork();
            if (pid == -1)
                perror("Error on fork\n");
            if (pid == 0)
                childProccessSJF(getpid(), actualProcessIndex, array, getppid());
            actualProcessIndex++;
        }
        
        for (; finishedProcesses < actualProcessIndex && actualProcessIndex <= num_comandos; finishedProcesses++){
            int status;
            int terminated_pid = wait(&status);
            printf("Child process %d terminated with status %d\n", terminated_pid, WEXITSTATUS(status));
            statusArray[finishedProcesses] = PROCESS_STATUS_FINISHED;
            if (finishedProcesses + parallelTasks < num_comandos){
                statusArray[finishedProcesses + parallelTasks] = PROCESS_STATUS_WAITING;
            }

            // Simulate the creation of a new process (Thread) if there are still processes to be executed

            if (actualProcessIndex < num_comandos && statusArray[actualProcessIndex] == PROCESS_STATUS_WAITING){
                pid_t pid = fork();
                if (pid == -1)
                    perror("Error on fork\n");
                if (pid == 0)
                    childProccessSJF(getpid(), actualProcessIndex, array, getppid());
                actualProcessIndex++;
            }
            printf("actualProcessIndex: %d, finishedProcesses: %d\n", actualProcessIndex, finishedProcesses);
        }
        printf("actualProcessIndex: %d, finishedProcesses: %d\n", actualProcessIndex, finishedProcesses);
    } 

    return 0; 

}
