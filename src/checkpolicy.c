#include "global.h"

int checkpolicy(char* policy){
   
    if (strcmp(policy, "SJF") == 0){
        escalonamentoSJF();
        return 0;
    } 
    else if (strcmp(policy, "FCFS") == 0){
        escalonamentoFCFS();
        return 0;
    }
    return -1;
}
