#include "global.h"
#include "exec.h"

// arg 1 - path to the output file
// arg 2 - parallel tasks
// arg 3 - sched-policy
#define CLIENT "../data/your_file.txt"
struct timeval t1, t2;

int main(int argc, char *argv[]) {

    char comando1[] = "ls -l -a -h";
	char comando2[] = "sleeep 30";
	char comando3[] = "sleep 10";
	char comando4[] = "ps";

    int res = exec(comando1);
	if (res != 0) {
		printf("Erro ao executar comando 1\n");
		return 1;
	}
	printf("Comando 1 executado com sucesso | res = %d\n", res);

    return 0;
}


// tv_sec 
// 0 1 2 3 4 5 6 7 ...
// tc_usec
// 0 - 999999
// 999999 -> tv_sec + 1, tv_usec = 0