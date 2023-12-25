#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <syscall.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>


struct arg_struct {
    const char* path;
    int  id;
};

int layer = 1;

pthread_mutex_t mutex;

void* search(void* arguments) {

    struct arg_struct* args = (struct arg_struct*)arguments;

    const char* path = args->path;

    int  id = args > id;

    DIR* dirFile;

    if (!(dirFile = opendir(path))) {
        return NULL;
    }

    struct dirent* hFile;

    while ((hFile = readdir(dirFile)) != NULL)
    {

        if (hFile->d_name[0] == '.')
            continue;

        if (hFile->d_type == DT_DIR) {

            pthread_mutex_lock(&mutex);

            char filepath[strlen(path) + 1 + strlen(hFile->d_name) + 1];

            snprintf(filepath, sizeof filepath, "%s/%s", path, hFile->d_name);

            const char* temp = (const char*)malloc(strlen(filepath) + 1);

            strcpy(temp, filepath);

            printf("folder : %s\n", hFile->d_name);

            if (layer == 0) {

                pid_t pid = fork();

            }
            else {



                struct arg_struct argsPass;

                argsPass.path = temp;

                argsPass.id = id + 1;

                pthread_mutex_unlock(&mutex);

                pthread_t thread;

                printf("id = %d\n", argsPass.id);

                pthread_create(&thread, NULL, search, (void*)&argsPass);

                pthread_join(thread, NULL);

                free((char*)temp);



            }


        }
        if (hFile->d_type == DT_REG) {

            printf("file : %s\n", hFile->d_name);
        }


    }

    layer = 1;

    closedir(dirFile);



}

int main()
{
    struct arg_struct args;

    args.path = "/";

    args.id = 0;

    pthread_mutex_init(&mutex, NULL);

    pthread_t thread;

    pthread_create(&thread, NULL, search, (void*)&args);


    pthread_join(thread, NULL);


    pthread_mutex_destroy(&mutex);
}