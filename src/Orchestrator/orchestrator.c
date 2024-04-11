#include "global.h"

char *comandos[] = {"sleep 4", "sleep 3", "sleep 4", "sleep 2", "sleep 3", "sleep 5", "sleep 1", "sleep 2", "ls -l -a -h", "sleep 1"};
char *comandos_for_sjf[] = {"10 sleep 4", "11 sleep 4", "12 sleep 5", "13 sleep 2", "14 sleep 5", "10 sleep 5", "1 ls /etc | wc -l"};

int main(int argc, char *argv[]){

    if (argc < 4){
        printf("Usage: %s <output-folder> <parallel-tasks> <sched-policy>\n", argv[0]);
        return 1;
    }

    fifo();

    int fifo_fd = open("fifo", O_RDONLY);
    if (fifo_fd == -1){
        perror("open");
        return 1;
    }

    printf("FIFO opened to read...\n");

    char buffer[256];
    ssize_t bytes_read;

    int policy = checkpolicy(argv[3]);
    if (policy == INVALID_POLICY) {
        printf("Invalid policy, server suspended!\n");
        return 1;
    }

    if (policy == SJF){
        while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1)) > 0){
            buffer[bytes_read] = '\0';

            printf("Received: %s\n", buffer);
            //run(argv[1], atoi(argv[2]), argv[3], retira_new_line(buffer));
            escalonamentoSJF(1,retira_new_line(buffer));

            // Limpe o buffer para a próxima leitura
            memset(buffer, 0, sizeof(buffer));
        }
    } else {
        while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer) - 1)) > 0){
            buffer[bytes_read] = '\0';

            printf("Received: %s\n", buffer);
            //run(argv[1], atoi(argv[2]), argv[3], retira_new_line(buffer));
            escalonamentoFCFS(1,retira_new_line(buffer));

            // Limpe o buffer para a próxima leitura
            memset(buffer, 0, sizeof(buffer));
        }
    }

    if (bytes_read == -1){
        perror("read");
    }

    close(fifo_fd);

return 0; 
}

