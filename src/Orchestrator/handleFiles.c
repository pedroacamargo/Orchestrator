#include "global.h"

void writeProcess(Process p, int fd, int command_id) {
    char buffer[MAX_COMMAND_SIZE];
    int size;

    if (p.status != PROCESS_STATUS_FINISHED) {
        size = snprintf(buffer, sizeof(buffer), "%d: %s-%d\n", p.pid, p.command, p.timePrediction);
    } else {
        size = snprintf(buffer, sizeof(buffer), "%d: %s [%f]\n", p.pid, p.command, p.elapsedTime);
    }

    if (size < 0 || size >= sizeof(buffer)) {
        perror("Error formatting process data");
        return;
    }
    
    if (write(fd, buffer, size) != size) {
        perror("Error writing to file");
    }

    memset(buffer, 0, sizeof(buffer));

    close(fd);
}

int removeProcessFromFile(const char *filename, int command_id) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) { 
        perror("Error opening file to remove process");
        return -1;
    }

    
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char *file_content = malloc(file_size + 1);
    if (read(fd, file_content, file_size) != file_size) {
        perror("Error reading file");
        free(file_content);
        close(fd);
        return -1;
    }
    file_content[file_size] = '\0';

    
    int tmp_fd = open("tmp.txt", O_RDWR | O_CREAT, 0644);
    if (tmp_fd == -1) {
        perror("Error opening temporary file");
        free(file_content);
        close(fd);
        return -1;
    }

    // Write all lines except the one with the command_id to the temporary file
    char *line = strtok(file_content, "\n");
    while (line != NULL) {
        int pid;
        if (sscanf(line, "%d:", &pid) == 1 && pid != command_id) {
            write(tmp_fd, line, strlen(line));
            write(tmp_fd, "\n", 1);
        }
        line = strtok(NULL, "\n");
    }

    rename("tmp.txt", filename);

    free(file_content);
    close(fd);
    close(tmp_fd);

    return 0;
}

int handleProcess(Process p) {
    char filename[50];

    switch (p.status) {
        case PROCESS_STATUS_RUNNING:
            strcpy(filename, "tmp/executing.txt");
            break;
        case PROCESS_STATUS_IDLE:
            strcpy(filename, "tmp/idle.txt");
            break;
        case PROCESS_STATUS_FINISHED:
            strcpy(filename, "tmp/completed.txt");
            break;
        default:
            return 0;
    }

    for (int i = PROCESS_STATUS_RUNNING; i <= PROCESS_STATUS_IDLE; ++i) {
        char existing_filename[50];
        switch (i) {
            case PROCESS_STATUS_RUNNING:
                strcpy(existing_filename, "tmp/executing.txt");
                break;
            case PROCESS_STATUS_IDLE:
                strcpy(existing_filename, "tmp/idle.txt");
                break;
            case PROCESS_STATUS_FINISHED:
                strcpy(existing_filename, "tmp/completed.txt");
                break;
            default:
                continue;
        }

        removeProcessFromFile(existing_filename, p.pid);
        memset(existing_filename, 0, sizeof(existing_filename));
    }


    int fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0644); 
    if (fd == -1) { 
        perror("Error opening destination file");
        return -1;
    }

    writeProcess(p, fd, p.pid);

    close(fd);

    return 0;
}

int countLines(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file to count lines");
        return -1;
    }

    int lines = 1;
    char buffer[1];
    while (read(fd, buffer, 1) > 0) {
        if (buffer[0] == '\n') {
            lines += 1;
        }
    }

    close(fd);

    return lines - 1;
}