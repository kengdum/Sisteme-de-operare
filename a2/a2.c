#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"

pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond4 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex4 = PTHREAD_MUTEX_INITIALIZER;
 

void* threadFunction(void* arg) {
    int threadNumber = *(int*) arg;
    pthread_mutex_lock(&mutex1);
    if(threadNumber == 1) {
        pthread_cond_wait(&cond1, &mutex1);
        info(BEGIN, 7, threadNumber);
    } else {
        info(BEGIN, 7, threadNumber);
        if(threadNumber == 4){
            pthread_cond_signal(&cond1);
        }
    }

    pthread_mutex_unlock(&mutex1);
    pthread_mutex_lock(&mutex4);

    if(threadNumber == 4) {
        pthread_cond_wait(&cond4, &mutex4);
        info(END, 7, threadNumber);
    } else {
        info(END, 7, threadNumber);
        if(threadNumber == 1){
            pthread_cond_signal(&cond4);
        }
    }
    pthread_mutex_unlock(&mutex4);
    return NULL;
}


int main() {
    init();
    info(BEGIN, 1, 0);

    pid_t pid2, pid3, pid4, pid5, pid6, pid7;
    int status;

    pid2 = fork();
    if (pid2 == 0) {
        info(BEGIN, 2, 0);
        pid3 = fork();
        if (pid3 == 0) {
            info(BEGIN, 3, 0);

            pid7 = fork();
            if (pid7 == 0) {
                info(BEGIN, 7, 0);
            pthread_t threads[5];
            int threadNumbers[5] = {1, 2, 3, 4, 5};
            for (int i = 0; i < 5; i++){
                pthread_create(&threads[i], NULL, threadFunction, &threadNumbers[i]);
            }
           
            for (int i = 0; i < 5; i++) {
                pthread_join(threads[i], NULL);
            }
            info(END, 7, 0);
            return 0;
            }
            else {
                wait(&status);
            }

            info(END, 3, 0);
            return 0;
        }
        else {
            pid5 = fork();
            if (pid5 == 0) {
                info(BEGIN, 5, 0);
                info(END, 5, 0);
                return 0;
            }
            else {
                wait(&status);
                wait(&status);
            }
        }
        info(END, 2, 0);
        return 0;
    }
    else {
        pid4 = fork();
        if (pid4 == 0) {
            info(BEGIN, 4, 0);
            pid6 = fork();
        if (pid6 == 0) {
            info(BEGIN, 6, 0);
            info(END, 6, 0);
            return 0;
        }
        else {
            wait(&status);
        }
        info(END, 4, 0);
        return 0;
        }
    }
    wait(&status);
    wait(&status);

    info(END, 1, 0);
    pthread_mutex_destroy(&mutex1);
    pthread_mutex_destroy(&mutex4);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond4);
    return 0;
}

