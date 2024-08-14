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

/*
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
*/


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
        // Skip '.' and '..'j 
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[MAX_PATH_LENGTH];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            // Increment count for this immediate directory
            count++;
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

    // for(int i = 0; i < subdir_count; i++) {
    //     pthread_data *data = malloc(sizeof(pthread_data));
    //     if (data == NULL) {
    //         perror("malloc");
    //         return 1;
    //     }
    //     data->iterator = subdir_count;
    //     data->path = strdup(source);  // Allocate and copy the source path

    //     ret = pthread_create(&threads[i], NULL, print_message_function, (void*)data);
    //     if (ret) {
    //         printf("Error creating thread %d: %d\n", i, ret);
    //         free(data->path);
    //         free(data);
    //         return 1;
    //     }
    // }

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


/*
######################################################################################################################################################
######################################################################################################################################################
######################################################################################################################################################
######################################################################################################################################################
*/

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

/*
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
*/


int count_subdirectories(const char *path) {
    // struct dirent *entry;
    // struct stat statbuf;
    // DIR *dp = opendir(path);
    // int count = 0;

    // if (dp == NULL) {
    //     perror("opendir");
    //     return -1;
    // }

    // while ((entry = readdir(dp)) != NULL) {
    //     // Skip '.' and '..'j 
    //     if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
    //         continue;

    //     char fullpath[MAX_PATH_LENGTH];
    //     snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

    //     if (stat(fullpath, &statbuf) == -1) {
    //         perror("stat");
    //         continue;
    //     }

    //     if (S_ISDIR(statbuf.st_mode)) {
    //         // Increment count for this immediate directory
    //         count++;
    //     }
    // }

    // closedir(dp);
    // return count;
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
        // if (path[strlen(path) - 1] == '/') {
            snprintf(fullpath, sizeof(fullpath), "%s%s", path, entry->d_name);
        // } else {
            // snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        // }

        // if (stat(fullpath, &statbuf) == -1) {
        //     perror("stat");
        //     continue;
        // }

        if (S_ISDIR(statbuf.st_mode)) {
            // Increment count for this immediate directory
            count++;
            // Print the path of the directory
            printf("Directory found: %s\n", fullpath);
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

    // for(int i = 0; i < subdir_count; i++) {
    //     pthread_data *data = malloc(sizeof(pthread_data));
    //     if (data == NULL) {
    //         perror("malloc");
    //         return 1;
    //     }
    //     data->iterator = subdir_count;
    //     data->path = strdup(source);  // Allocate and copy the source path

    //     ret = pthread_create(&threads[i], NULL, print_message_function, (void*)data);
    //     if (ret) {
    //         printf("Error creating thread %d: %d\n", i, ret);
    //         free(data->path);
    //         free(data);
    //         return 1;
    //     }
    // }

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

/*
###############################################################
###############################################################
###############################################################
*/#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAX_PATH_LENGTH 2048
#define COMMAND_LENGTH 8192 

// Function to count subdirectories and store their full paths
int collect_subdirectories(const char *path, char **paths, int *count) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp = opendir(path);
    int local_count = 0;

    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)) != NULL) {
       
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[MAX_PATH_LENGTH];
        if (path[strlen(path) - 1] == '/') {
            snprintf(fullpath, sizeof(fullpath), "%s%s", path, entry->d_name);
        } else {
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);
        }

        if (stat(fullpath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            // Allocate space for the new path
            paths[local_count] = strdup(fullpath);
            if (paths[local_count] == NULL) {
                perror("strdup");
                closedir(dp);
                return -1;
            }
            local_count++;

            // Recursively collect subdirectories
            int subdir_count = collect_subdirectories(fullpath, paths + local_count, count);
            if (subdir_count < 0) {
                closedir(dp);
                return -1;
            }

            local_count += subdir_count;
        }
    }

    closedir(dp);
    *count = local_count;
    return local_count;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_directory> <destination_directory>\n", argv[0]);
        return 1;
    }

    const char *source = argv[1];
    const char *destination = argv[2];

    char *paths[MAX_PATH_LENGTH];
    int count = 0;

    // Collect all subdirectories' paths
    int subdir_count = collect_subdirectories(source, paths, &count);
    if (subdir_count == -1) {
        fprintf(stderr, "Error collecting subdirectories from %s\n", source);
        return 1;
    }

    printf("\nNumber of subdirectories collected: %d\n\n", subdir_count);

    // Build the rsync command
    char command[COMMAND_LENGTH];
    snprintf(command, sizeof(command), "rsync -avzP");

    for (int i = 0; i < subdir_count; i++) {
        // Append each path to the command
        strncat(command, " ", sizeof(command) - strlen(command) - 1);
        strncat(command, paths[i], sizeof(command) - strlen(command) - 1);
        free(paths[i]); // Free the allocated memory
    }

    // Append the destination path
    strncat(command, " ", sizeof(command) - strlen(command) - 1);
    strncat(command, destination, sizeof(command) - strlen(command) - 1);

    // Execute the rsync command
    int result = system(command);
    if (result == -1) {
        perror("system");
        return 1;
    } else {
        printf("rsync command executed with exit status %d\n", WEXITSTATUS(result));
    }

    return 0;
}

// 333333333333#####################################################################################################################################
// monday 12 aug 24 \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

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
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[MAX_PATH_LENGTH];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &statbuf) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            count++;
            printf("Directory found: %s\n", fullpath);
        }
    }

    closedir(dp);
    return count;
}

/* 
typedef struct {
    int iterator;
    char* path;
} pthread_data;

void* print_message_function(void* ptr) {
    pthread_data* data = (pthread_data*)ptr;
    pthread_t thread_id = pthread_self();
    for (int i = 0; i < data->iterator; i++) {
        printf("%s\n", data->path);
        printf("Thread id: %lu\n\n", thread_id);
        sleep(1);
    }
    free(data->path);
    free(data);
    return NULL;
}
*/

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source_directory> <destination_directory>\n", argv[0]);
        return 1;
    }

    const char *source = argv[1];
    const char *destination = argv[2];

    int subdir_count = count_subdirectories(source);
    if (subdir_count == -1) {
        fprintf(stderr, "Error counting subdirectories in %s\n", source);
        return 1;
    }
    printf("\nNumber of subdirectories in %s: %d\n\n", source, subdir_count);

    pthread_t threads[subdir_count];
    int ret;

    /* 
    for (int i = 0; i < subdir_count; i++) {
        pthread_data *data = malloc(sizeof(pthread_data));
        if (data == NULL) {
            perror("malloc");
            return 1;
        }
        data->iterator = subdir_count;
        data->path = strdup(source);

        ret = pthread_create(&threads[i], NULL, print_message_function, (void*)data);
        if (ret) {
            printf("Error creating thread %d: %d\n", i, ret);
            free(data->path);
            free(data);
            return 1;
        }
    }

    for (int i = 0; i < subdir_count; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Threads finished executing.\n");
    */

    char command[COMMAND_LENGTH];
    snprintf(command, sizeof(command), "rsync -avzP %s %s", source, destination);

    int result;// = system(command);

    if (result == -1) {
        perror("system");
        return 1;
    } else {
        printf("rsync command executed with exit status %d\n", WEXITSTATUS(result));
    }

    return 0;
}



/*14 AUG 24 */
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


