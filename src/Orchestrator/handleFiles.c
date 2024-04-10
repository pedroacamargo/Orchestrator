#include "global.h"

void writeProcess(Process p, int fd, int command_id) {
    char buffer[MAX_COMMAND_SIZE];
    int size;

    if (p.status != PROCESS_STATUS_FINISHED) {
        size = snprintf(buffer, sizeof(buffer), "(%d): %s\n", command_id, p.command);
    } else {
        size = snprintf(buffer, sizeof(buffer), "(%d): %s [%f]\n", command_id, p.command, p.elapsedTime);
    }

    if (size < 0 || size >= sizeof(buffer)) {
        perror("Error formatting process data");
        return;
    }
    
    if (write(fd, buffer, size) != size) {
        perror("Error writing to file");
    }
}

int removeProcessFromFile(const char *filename, int command_id) {
    int fd = open(filename, O_RDWR);
    if (fd == -1) { 
        //perror("Error opening file to remove process");
        return -1;
    }

    // Read the entire content of the file into memory
    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    char *file_content = malloc(file_size + 1);
    if (file_content == NULL) {
        perror("Error allocating memory");
        close(fd);
        return -1;
    }
    read(fd, file_content, file_size);
    file_content[file_size] = '\0';

    // Find the command to remove in the memory
    char command_to_remove[MAX_COMMAND_SIZE];
    sprintf(command_to_remove, "(%d):", command_id);
    char *command_start = strstr(file_content, command_to_remove);
    if (command_start == NULL) {
        // If the command is not found, just ignore and return
        free(file_content);
        close(fd);
        return 0;
    }
    char *command_end = strstr(command_start, "\n");
    if (command_end == NULL) {
        command_end = file_content + file_size;  // If there's no newline, the command is at the end of the file
    } else {
        command_end++;  // Include the newline in the command
    }

    // Remove the command from the memory
    memmove(command_start, command_end, file_content + file_size - command_end + 1);
    file_size -= command_end - command_start;

    // Write the memory content back to the file
    lseek(fd, 0, SEEK_SET);  // Go back to the start of the file
    write(fd, file_content, file_size);  // Write the new content to the file
    ftruncate(fd, file_size);  // Truncate the file to the new size

    // Clean up
    free(file_content);
    close(fd);

    return 0;
}

int handleFiles(char queue[][100], int i, ProcessStatus status) {
    Process p;
    p.pid = getpid(); // TODO: Implement PID retrieval
    p.parentPid = getppid(); // TODO: Implement parent PID retrieval
    strcpy(p.command, queue[i]);
    p.status = status;
    p.elapsedTime = 4000; // TODO: Implement time measurement

    int command_id = i;

    char filename[50];

    switch (p.status) {
        case PROCESS_STATUS_RUNNING:
            strcpy(filename, "tmp/executing.txt");
            break;
        case PROCESS_STATUS_WAITING:
            strcpy(filename, "tmp/scheduled.txt");
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
            case PROCESS_STATUS_WAITING:
                strcpy(existing_filename, "tmp/scheduled.txt");
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

        removeProcessFromFile(existing_filename, command_id);
    }

    int fd = open(filename, O_RDWR | O_CREAT | O_APPEND, 0644); 
    if (fd == -1) { 
        perror("Error opening destiny file");
        return -1;
    }

    writeProcess(p, fd, command_id);

    close(fd);

    return 0;
}