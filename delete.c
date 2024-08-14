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

int count_subdirectories(const char *path) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp = opendir(path);
    int count = 0;

    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)) != NULL) {
        // Skip '.' and '..'
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[MAX_PATH_LENGTH];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            // Increment count for this directory
            count++;

            // Recursively count subdirectories
            int subdir_count = count_subdirectories(fullpath);
            if (subdir_count < 0) {
                closedir(dp);
                return subdir_count; // Propagate error code from recursive call
            }

            count += subdir_count; // Add the count from the recursive call
        }
    }

    closedir(dp);
    return count;
}

typedef struct {
    int iterator;
    char* path;
    
} pthread_data;

void* print_message_function(void* ptr) {
    pthread_data* data = (pthread_data*)ptr;
    pthread_t thread_id = pthread_self();
    for(int i = 0; i < data->iterator; i++) {
        printf("%s\n", data->path);
        printf("Thred id : %lu\n\n", thread_id);
        sleep(1);
    }
    free(data->path);  // Free allocated memory
    free(data);        // Free the pthread_data structure
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_directory> <destination_directory>\n", argv[0]);
        return 1;
    }

    // Define the source and destination directories
    const char *source = argv[1];
    const char *destination = argv[2];

    // Count the number of subdirectories in the source directory
    int subdir_count = count_subdirectories(source);
    if (subdir_count == -1) {
        fprintf(stderr, "Error counting subdirectories in %s\n", source);
        return 1;
    }
    printf("\nNumber of subdirectories in %s: %d\n\n", source, subdir_count);

    pthread_t threads[subdir_count];
    int ret;

    for(int i = 0; i < subdir_count; i++) {
        pthread_data *data = malloc(sizeof(pthread_data));
        if (data == NULL) {
            perror("malloc");
            return 1;
        }
        data->iterator = subdir_count;
        data->path = strdup(source);  // Allocate and copy the source path

        ret = pthread_create(&threads[i], NULL, print_message_function, (void*)data);
        if (ret) {
            printf("Error creating thread %d: %d\n", i, ret);
            free(data->path);
            free(data);
            return 1;
        }
    }

    for(int i = 0; i < subdir_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Threads finished executing.\n");

    // Construct the rsync command
    char command[COMMAND_LENGTH];
    snprintf(command, sizeof(command), "rsync -avzP %s %s", source, destination);

    // Execute the rsync command
    int result ;//= system(command);

    // Check the result of the system call
    if (result == -1) {
        perror("system");
        return 1;
    } else {
        printf("rsync command executed with exit status %d\n", WEXITSTATUS(result));
    }

    return 0;
}
