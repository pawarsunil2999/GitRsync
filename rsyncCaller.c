#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_PATH_LENGTH 2048
#define COMMAND_LENGTH 2048
#define MAX_SUBDIRS 1024

typedef struct {
    char fullpath[MAX_PATH_LENGTH];
    const char *destination;
} thread_data_t;

int count_subdirectories(const char *path, char subdirs[MAX_SUBDIRS][MAX_PATH_LENGTH]) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp = opendir(path);
    int count = 0;

    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[MAX_PATH_LENGTH];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            snprintf(subdirs[count], MAX_PATH_LENGTH, "%s", fullpath);
            count++;
        }
    }

    closedir(dp);
    return count;
}

void* rsync_thread(void* arg) {
    thread_data_t *data = (thread_data_t*)arg;
    char command[COMMAND_LENGTH];

    // Make sure COMMAND_LENGTH is large enough
    int required_length = snprintf(NULL, 0, "rsync -avzP %s %s", data->fullpath, data->destination) + 1;
    if (required_length > sizeof(command)) {
        fprintf(stderr, "Error: Command length exceeds buffer size.\n");
        free(data);
        return NULL;
    }

    // Construct the command
    int written = snprintf(command, sizeof(command), "rsync -avzP %s %s", data->fullpath, data->destination);
    if (written < 0 || written >= sizeof(command)) {
        fprintf(stderr, "Error: snprintf failed or output truncated.\n");
        free(data);
        return NULL;
    }

    int result = system(command);

    if (result == -1) 
        perror("system");

    free(data);
    return NULL;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_directory> <destination_directory>\n", argv[0]);
        return 1;
    }

    const char *source = argv[1];
    const char *destination = argv[2];

    char subdirs[MAX_SUBDIRS][MAX_PATH_LENGTH];
    int subdir_count = count_subdirectories(source, subdirs);
    if (subdir_count == -1) {
        fprintf(stderr, "Error counting subdirectories in %s\n", source);
        return 1;
    }
    printf("\nNumber of subdirectories in %s: %d\n\n", source, subdir_count);

    pthread_t threads[subdir_count];
    int ret;

    for (int i = 0; i < subdir_count; i++) {
        thread_data_t *data = malloc(sizeof(thread_data_t));
        if (data == NULL) {
            perror("malloc");
            return 1;
        }
        snprintf(data->fullpath, MAX_PATH_LENGTH, "%s", subdirs[i]);
        data->destination = destination;

        ret = pthread_create(&threads[i], NULL, rsync_thread, (void*)data);
        if (ret) {
            fprintf(stderr, "Error creating thread %d: %d\n", i, ret);
            free(data);
            return 1;
        }
    }

    for (int i = 0; i < subdir_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("All threads finished executing.\n");
    return 0;
}
