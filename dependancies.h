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

char subdirs[MAX_SUBDIRS][MAX_PATH_LENGTH];

typedef struct {
    char fullpath[MAX_PATH_LENGTH];
    const char *destination;
    const char *options;
    int itr;
} thread_data_t;
