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

pthread_t threads[100000];

int threadCount = 0;

int FileCount;

pthread_mutex_t mutexQueue;

pthread_mutex_t mutexCount;

int pCount=0;

int pipes[10000+1][2];

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

            FileCount++;

            printf("%d\n%d\n\n\n\n\n",pCount,FileCount);

            pthread_mutex_unlock(&mutexCount);

            write(pipes[pCount][1],&FileCount,sizeof(int));

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

    char* p = strdup("/home/hakir/Desktop/Main (2)/");

    int id=0;

    struct arg* args = (struct arg*)malloc(sizeof(struct arg));

    args->path = p;

    args->id = 0;

    DIR* dirFile;


    if (!(dirFile = opendir(p))) {
        return NULL;
    }

    struct dirent* hFile;

    pid_t pids[10000];

    pids[pCount]=getpid();

    pipe(pipes[pCount]);

    write(pipes[pCount][1],&FileCount,sizeof(int));


    while ((hFile = readdir(dirFile)) != NULL)
    {

        if (strcmp(hFile->d_name, ".") == 0  strcmp(hFile->d_name, "..") == 0)
            continue;

        if (hFile->d_type == DT_REG) {

            printf("file : %s\n", hFile->d_name);
    
        }
        if (hFile->d_type == DT_DIR) {

            char filepath[strlen(p) + 1 + strlen(hFile->d_name) + 1];

            snprintf(filepath, sizeof filepath, "%s/%s", p, hFile->d_name);

            printf("folder : %s\n", filepath);

            if(pids[pCount]==0){
            //child

            char* temp = (char*)malloc(strlen(filepath) + 1);

            strcpy(temp, filepath);

            struct arg* argsPass = (struct arg*)malloc(sizeof(struct arg));

            argsPass->path = temp;

            argsPass->id = id + 1;
                
            submitThread(argsPass);



                
            }
            else{
                //parent
                pCount++;

                printf("\n\n\n\n p count = %d\n\n\n\n",pCount);

                pipe(pipes[pCount]);

                write(pipes[pCount][1],&FileCount,sizeof(int));

                pids[pCount]=fork();


            }


        }

    }


    printf("\n\n\n\n p count = %d\n\n\n\n",pCount);


    for(int i=0;i<pCount;i++){
        wait(NULL);
    }   




     for(int i=0;i<threadCount;i++){
    
        if(threads[i]!=NULL){
            pthread_join(threads[i], NULL);

        }

        pthread_exit(NULL);
    }    

    int temp=0;

    for(int i=0;i<pCount;i++){


        read(pipes[i][0],&FileCount,sizeof(int));

        temp+=FileCount;

        close(pipes[i][0]);
        close(pipes[i][1]);

    }
        

    printf("\n\n\n%d",temp);

    
    pthread_mutex_destroy(&mutexQueue);

    pthread_mutex_destroy(&mutexCount);





    return 0;
}