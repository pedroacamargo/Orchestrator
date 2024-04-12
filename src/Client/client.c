#include "global.h"
//./client execute 100 -u "prog-a arg-1 (...) arg-n"
int main(int argc, char *argv[]){

    

    int fifo_fd = open("fifo", O_WRONLY);
    if (fifo_fd == -1){
        perror("open");
        exit(1);
    }

    char BUFFER[256];
    memset(BUFFER, 0, sizeof(BUFFER));
    snprintf(BUFFER, sizeof(BUFFER), "%s-%s", argv[2], argv[4]);
    ssize_t bytes_written = write(fifo_fd, BUFFER, sizeof(BUFFER));

    if (bytes_written == -1){
        perror("write");
        exit(1);
    }

    close(fifo_fd);

    return 0;



    
}

   /*    while ((bytes_read = read(fifo_fd,buffer,sizeof(buffer)) > 0)){

                printf("Received: %s\n", buffer);
                char *first_string = strtok(buffer, "-");
                char *second_string = strtok(NULL, "\0");
                printf("First string: %s\n", first_string);
                printf("Second string: %s\n", second_string);

           } */