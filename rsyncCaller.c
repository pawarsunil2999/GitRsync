#include "dependancies.h"

int count_subdirectories(const char *path, char subdirs[MAX_SUBDIRS][MAX_PATH_LENGTH]) {
    struct dirent *entry;
    struct stat statbuf;
    DIR *dp = opendir(path);
    int count = 0;

    if (dp == NULL) {
        perror("opendir");
        return -1;
    }

    //file name store karne ke liye file handling
    FILE *output_file = fopen("Sunil.txt", "w");
    if (output_file == NULL) {
        perror("fopen");
        return -1;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char fullpath[MAX_PATH_LENGTH];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &statbuf) == -1) {
            // perror("stat");
            continue;
        }

        if( count ==0 ) {
            snprintf(subdirs[count], MAX_PATH_LENGTH, "%s", path);
            printf("Subdirectory %d: %s\n", count, fullpath);
            count++;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            snprintf(subdirs[count], MAX_PATH_LENGTH, "%s", fullpath);
            printf("Subdirectory %d: %s\n", count, fullpath);
            count++;
        } else {
            // Write only the file name to Sunil.txt
            fprintf(output_file, "%s\n", entry->d_name);
            printf("File name written: %s\n", entry->d_name);
        }

    }

    closedir(dp);
    return count;
}

void* rsync_thread(void* arg) {
    thread_data_t *data = (thread_data_t*)arg;
    char command[COMMAND_LENGTH];

    // Build command for rsync
    int written;
    if (data->itr == 0) {
        written = snprintf(command, sizeof(command), "./sunilRsync -avzP --include-from=Sunil.txt %s %s", data->fullpath, data->destination);
    } else {
        written = snprintf(command, sizeof(command), "./sunilRsync -avzP %s %s", data->fullpath, data->destination);
    }

    if (written < 0 || written >= sizeof(command)) {
        fprintf(stderr, "Error: snprintf failed or command length exceeds buffer size.\n");
        free(data);
        return NULL;
    }

    // Print command for debugging
    printf("Executing command: %s\n", command);

    // Execute command and capture output for debugging
    int result = system(command);
    if (result == -1) {
        perror("system");
    } else {
        printf("Command executed with result code: %d\n", result);
    }

    free(data);
    return NULL;
}

// void* rsync_thread(void* arg) {
//     thread_data_t *data = (thread_data_t*)arg;
//     char command[COMMAND_LENGTH];

//     //COMMAND_LENGTH chi size check 
//     int required_length = snprintf(NULL, 0, "./sunilRsync -avzP %s %s", data->fullpath, data->destination) + 1;
//     if (required_length > sizeof(command)) {
//         fprintf(stderr, "Error: Command length exceeds buffer size.\n");
//         free(data);
//         return NULL;
//     }

//     // command construction
//     if( data->itr == 0 ) {
//         // printf("\nfiles from call \t Src : %s | Dest : %s ", data->fullpath, data->destination);
//         int written = snprintf(command, sizeof(command), "./sunilRsync -avzP --files-from=Sunil.txt %s %s", data->fullpath, data->destination);
//         if (written < 0 || written >= sizeof(command)) {
//             fprintf(stderr, "Error: snprintf failed or output truncated.\n");
//             free(data);
//             return NULL;
//         }

//     }
//     else {
//         // printf("\nnormal call from call");
//         int written = snprintf(command, sizeof(command), "./sunilRsync -avzP %s %s", data->fullpath, data->destination);
//         if (written < 0 || written >= sizeof(command)) {
//             fprintf(stderr, "Error: snprintf failed or output truncated.\n");
//             free(data);
//             return NULL;
//         }

//     }

//     printf("\n\nSystem : %s", command);
//     int result = system(command);

//     if (result == -1) 
//         perror("system");

//     free(data);
//     return NULL;
// }


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
    printf("\nNumber of subdirectories in %s: %d\n\n", source, subdir_count-1);
    
    //  code to print the paths
    for (int i = 0; i < subdir_count; i++) {
        // Print each path
        printf("Subdirectory %d: %s\n", i, subdirs[i]);
    }


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
        data->itr = i;

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

    printf("\nAll threads finished executing.\n");
    return 0;
}