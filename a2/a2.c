#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include "a2_helper.h"
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex5 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond5 = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex6 = PTHREAD_MUTEX_INITIALIZER;

sem_t semafor;
sem_t *semafor63;
sem_t *semafor72;
sem_t *semafor62;

int started4 = 0;
int started1 = 0;
int contor = 0;
int started11 = 0;
int pornit72 = 0;
int terminat72 = 0;
int terminat63 = 0;
int thhreaduriCareRuleaza = 0;

void* threadFunction(void* arg) {
    int threadNumber = *(int*) arg;
    if(threadNumber == 2) {
        sem_wait(semafor72);
    }
    pthread_mutex_lock(&mutex);
    if(threadNumber == 1) {
        if(started4 == 0)
            pthread_cond_wait(&cond, &mutex);
        info(BEGIN, 7, threadNumber);
    } else {
        info(BEGIN, 7, threadNumber);
        if(threadNumber == 4){
            started4 = 1;
            pthread_cond_signal(&cond);
        }
    }

    if(threadNumber == 4) {
        if(started1 == 0) 
             pthread_cond_wait(&cond, &mutex);
        info(END, 7, threadNumber);
    } else {
        info(END, 7, threadNumber);
        if(threadNumber == 2) {
            sem_post(semafor62);
        }
        if(threadNumber == 1){
            started1 = 1;
            pthread_cond_signal(&cond);
        }
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}
void* threadFunction5(void *arg) {
    int threadNumber = *(int *) arg;
    sem_wait(&semafor);
    info(BEGIN, 5, threadNumber);
    pthread_mutex_lock(&mutex5);
    contor ++;
    if(threadNumber == 11)
         started11 = 1;
    if(contor <= 34 && threadNumber != 11) {
        //printf("%d %d\n", contor, threadNumber);
        info(END, 5, threadNumber);
        pthread_mutex_unlock(&mutex5);
    }
    else {
        thhreaduriCareRuleaza ++;
        if((thhreaduriCareRuleaza == 6 && started11 == 1) || contor == 39){
            pthread_mutex_unlock(&mutex5);
            pthread_cond_broadcast(&cond5);
        }
        else {
            //printf("%d %d %d\n", threadNumber, contor, thhreaduriCareRuleaza);
            pthread_cond_wait(&cond5, &mutex5);
            pthread_mutex_unlock(&mutex5);
        }
        info(END, 5, threadNumber);
    }
    sem_post(&semafor);
    return NULL;
}
void* threadFunction6(void *arg) {

    int threadNumber = *(int *) arg;

    if (threadNumber == 3) {
        sem_wait(semafor63);
    }
    if(threadNumber == 2) {
        sem_wait(semafor62);
    }
    info(BEGIN, 6, threadNumber);
    info(END, 6, threadNumber);
    if (threadNumber == 3) {
        sem_post(semafor72);
    }


    return NULL;
}

int main() {
    init();
    info(BEGIN, 1, 0);
    sem_unlink("/semafor63");
    semafor63 = sem_open("/semafor63", O_CREAT, 0644, 1);
    sem_unlink("/semafor72");
    semafor72 = sem_open("/semafor72", O_CREAT, 0644, 0);
    sem_unlink("/semafor62");
    semafor62 = sem_open("/semafor62", O_CREAT, 0644, 0);
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
                pthread_t threads5[39];
                sem_init(&semafor, 5, 6);
                int threadNumbers5[39];
                for (int i = 0; i < 39; i++){
                    threadNumbers5[i] = i + 1;
                    pthread_create(&threads5[i], NULL, threadFunction5, &threadNumbers5[i]);
                }
                for (int i = 0; i < 39; i++) {
                    pthread_join(threads5[i], NULL);
                }
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
            pthread_t threads6[5];
            int threadNumbers6[5] = {1, 2, 3, 4, 5};
            for (int i = 0; i < 5; i++){
                pthread_create(&threads6[i], NULL, threadFunction6, &threadNumbers6[i]);
            }

            for (int i = 0; i < 5; i++) {
                pthread_join(threads6[i], NULL);
            }
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

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    pthread_cond_destroy(&cond5);
    pthread_mutex_destroy(&mutex5);
    pthread_mutex_destroy(&mutex6);

    return 0;
}
