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
#include <semaphore.h>



struct arg {

    char* path;

    int  id;

};

pthread_t threads[100000];

int threadCount = 0;

pthread_mutex_t mutexQueue;

void* search(void* arguments);


void submitThread(struct arg* argsPass) {

    pthread_mutex_lock(&mutexQueue);

    pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));

    pthread_create(thread, NULL, search, (void*)argsPass);

    threads[threadCount] = *thread;

    threadCount++;

    printf("%d\n\n", threadCount);

    pthread_mutex_unlock(&mutexQueue);
}



void* search(void* arguments) {

    struct arg* args = (struct arg*)malloc(sizeof(struct arg));

    args = (struct arg*)arguments;

    char* path = args->path;

    int id = args->id;

    DIR* dirFile;


    if (!(dirFile = opendir(path))) {
        return NULL;
    }

    struct dirent* hFile;

    while ((hFile = readdir(dirFile)) != NULL)
    {

        if (strcmp(hFile->d_name, ".") == 0 || strcmp(hFile->d_name, "..") == 0)
            continue;

        if (hFile->d_type == DT_REG) {

            printf("file : %s\n", hFile->d_name);


        }
        if (hFile->d_type == DT_DIR) {


            char filepath[strlen(path) + 1 + strlen(hFile->d_name) + 1];

            snprintf(filepath, sizeof filepath, "%s/%s", path, hFile->d_name);

            char* temp = (char*)malloc(strlen(filepath) + 1);

            strcpy(temp, filepath);

            printf("folder : %s\n", filepath);

            struct arg* argsPass = (struct arg*)malloc(sizeof(struct arg));

            argsPass->path = temp;

            argsPass->id = id + 1;

            submitThread(argsPass);



        }


    }


    closedir(dirFile);






}




int main()
{

    pthread_mutex_init(&mutexQueue, NULL);


    char* p = strdup("/home/hakir/Desktop/OS/OS1/");

    struct arg* args = (struct arg*)malloc(sizeof(struct arg));

    args->path = p;

    args->id = 0;

    submitThread(args);

    for (int i = 0; i < threadCount; i++) {

        if (pthread_join(threads[i], NULL) != 0) {
            printf("error");
        }
        pthread_exit(NULL);

    }


    pthread_mutex_destroy(&mutexQueue);





    return 0;
}