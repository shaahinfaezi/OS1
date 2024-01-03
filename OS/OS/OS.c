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


struct arg {

    char* path;

    int  id;

};

pthread_t threads[10000];

int threadCount = 0;

int FileCount = 0;

pthread_mutex_t mutexQueue;

pthread_mutex_t mutexCount;

int FilePipeCount = 0;

int FilePipes[10000 + 1][2];



void* search(void* arguments);


void submitThread(struct arg* argsPass) {

    pthread_mutex_lock(&mutexQueue);

    pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));

    pthread_create(thread, NULL, search, (void*)argsPass);

    threads[threadCount] = *thread;

    threadCount++;

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

        if (strcmp(hFile->d_name, ".") == 0  strcmp(hFile->d_name, "..") == 0)
            continue;

        if (hFile->d_type == DT_REG) {

            printf("file : %s\n", hFile->d_name);

            pthread_mutex_lock(&mutexCount);


            read(FilePipes[FilePipeCount][0], &FileCount, sizeof(int));

            FileCount++;

            printf("%d\n%d\n\n\n\n\n", FilePipeCount, FileCount);

            write(FilePipes[FilePipeCount][1], &FileCount, sizeof(int));

            pthread_mutex_unlock(&mutexCount);



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

    pthread_mutex_init(&mutexCount, NULL);

    char* p = strdup("/home/hakir/Desktop");

    //

    int id = 0;

    struct arg* args = (struct arg*)malloc(sizeof(struct arg));

    args->path = p;

    args->id = 0;

    DIR* dirFile;


    if (!(dirFile = opendir(p))) {
        return NULL;
    }

    struct dirent* hFile;

    pid_t pids[10000];

    int procCount = 0;


    pipe(FilePipes[FilePipeCount]);

    write(FilePipes[FilePipeCount][1], &FileCount, sizeof(int));


    while ((hFile = readdir(dirFile)) != NULL)
    {

        if (strcmp(hFile->d_name, ".") == 0  strcmp(hFile->d_name, "..") == 0)
            continue;

        if (hFile->d_type == DT_REG) {

            printf("file : %s\n", hFile->d_name);

            read(FilePipes[0][0], &FileCount, sizeof(int));

            FileCount++;

            write(FilePipes[0][1], &FileCount, sizeof(int));


        }
        if (hFile->d_type == DT_DIR) {

            char filepath[strlen(p) + 1 + strlen(hFile->d_name) + 1];

            snprintf(filepath, sizeof filepath, "%s/%s", p, hFile->d_name);

            printf("folder : %s\n", filepath);

            if (pids[procCount] == 0) {
                //child

                char* temp = (char*)malloc(strlen(filepath) + 1);

                strcpy(temp, filepath);

                struct arg* argsPass = (struct arg*)malloc(sizeof(struct arg));

                argsPass->path = temp;

                argsPass->id = id + 1;

                submitThread(argsPass);


            }
            else if (pids[procCount] == 0 && getppid() != 0) {
                //parent         


                pid_t temp = fork();

                pids[procCount] = temp;

                FilePipeCount++;
                pipe(FilePipes[FilePipeCount]);

                write(FilePipes[FilePipeCount][1], &FileCount, sizeof(int));

                procCount++;



                printf("\n\n\n%d,%d", pids[procCount], getppid());

                printf("\n\n\n\n COUTNS : %d   %d", procCount, FilePipeCount);



            }


        }

    }



    if (pids[procCount] == 0) {

        for (int j = 0; j < threadCount; j++) {

            if (threads[j] != NULL) {

                pthread_join(threads[j], NULL);

            }

        }

        printf("PCOUNT%d\n\n\n\n", procCount);

    }


    for (int i = 0; i < procCount; i++) {

        printf("\n\n\n\nPROC%d", i);

        waitpid(pids[i], NULL, 0);

    }


    int temp = 0;

    int sum = 0;


    for (int i = 0; i <= FilePipeCount; i++) {

        read(FilePipes[i][0], &temp, sizeof(int));

        printf("\n\n\n\nTEMP %d ,%d", i, temp);

        sum += temp;



    }


    printf("\n\n\nRESSSS: %d", sum);









    printf("KIR");

    pthread_mutex_destroy(&mutexQueue);

    pthread_mutex_destroy(&mutexCount);




    return 0;
}