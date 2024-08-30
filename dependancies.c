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
    int itr;
} thread_data_t;
