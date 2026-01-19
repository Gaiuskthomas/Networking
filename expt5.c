#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>   /* for sleep() */

void *reader(void *);
void *writer(void *);
sem_t mutex1, mutex2;
sem_t writeblock;
sem_t readblock;
sem_t writepending;
int readcount = 0;
int writecount = 0;

int main(int argc, char *argv[])
{
    int i, j, k;
    int N_readers, N_writers;
    int readers_num[100], writers_num[100];
    pthread_t tid_readers[100], tid_writers[100];

    printf("Enter the no of readers: ");
    if (scanf("%d", &N_readers) != 1) return 1;
    printf("Enter the no of writers: ");
    if (scanf("%d", &N_writers) != 1) return 1;

    for (k = 0; k < N_readers; k++)
        readers_num[k] = k;
    for (k = 0; k < N_writers; k++)
        writers_num[k] = k;

    if (sem_init(&mutex1, 0, 1) < 0) {
        perror("could not init semaphore mutex1");
        exit(1);
    }
    if (sem_init(&mutex2, 0, 1) < 0) {
        perror("could not init semaphore mutex2");
        exit(1);
    }
    if (sem_init(&writeblock, 0, 1) < 0) {
        perror("Could not init semaphore writeBlock");
        exit(1);
    }
    if (sem_init(&readblock, 0, 1) < 0) {
        perror("Could not init semaphore readBlock");
        exit(1);
    }
    if (sem_init(&writepending, 0, 1) < 0) {
        perror("Could not init semaphore writePending");
        exit(1);
    }

    for (i = 0; i < N_readers; i++) {
        if (pthread_create(&tid_readers[i], NULL, reader, &readers_num[i])) {
            perror("could not create reader thread");
            exit(1);
        }
    }
    for (j = 0; j < N_writers; j++) {
        if (pthread_create(&tid_writers[j], NULL, writer, &writers_num[j])) {
            perror("could not create writer thread");
            exit(1);
        }
    }

    for (i = 0; i < N_readers; i++) {
        pthread_join(tid_readers[i], NULL);
    }
    for (j = 0; j < N_writers; j++) {
        pthread_join(tid_writers[j], NULL);
    }

    sem_destroy(&mutex1);
    sem_destroy(&mutex2);
    sem_destroy(&readblock);
    sem_destroy(&writeblock);
    sem_destroy(&writepending);

    return 0;
}

void *reader(void *param)
{
    int i = *((int *)param);

    while (1) {
        sleep(1);

        if (sem_wait(&writepending) < 0) {
            perror("cannot decrement the semaphore writePending");
            exit(1);
        }
        if (sem_wait(&readblock) < 0) {
            perror("cannot decrement the semaphore readBlock");
            exit(1);
        }
        if (sem_wait(&mutex1) < 0) {
            perror("cannot decrement the semaphore mutex1");
            exit(1);
        }
        readcount = readcount + 1;
        if (readcount == 1) {
            if (sem_wait(&writeblock) < 0) {
                perror("cannot decrement the semaphore writeBlock");
                exit(1);
            }
        }
        if (sem_post(&mutex1) < 0) {
            perror("cannot increment semaphore mutex1");
            exit(1);
        }
        if (sem_post(&readblock) < 0) {
            perror("cannot increment semaphore readBlock");
            exit(1);
        }
        if (sem_post(&writepending) < 0) {
            perror("cannot increment semaphore writePending");
            exit(1);
        }

        /* reads resources */
        printf("Reader %d is Reading\n", i);
        sleep(1);

        if (sem_wait(&mutex1) < 0) {
            perror("cannot decrement the semaphore mutex");
            exit(1);
        }
        readcount = readcount - 1;
        if (readcount == 0) {
            if (sem_post(&writeblock) < 0) {
                perror("cannot increment semaphore mutex");
                exit(1);
            }
        }
        if (sem_post(&mutex1) < 0) {
            perror("cannot increment semaphore mutex");
            exit(1);
        }
    }

    return NULL;
}

void *writer(void *param)
{
    int i = *((int *)param);

    while (1) {
        sleep(1);
        if (sem_wait(&mutex2) < 0) {
            perror("cannot decrement the semaphore mutex2");
            exit(1);
        }
        writecount = writecount + 1;
        if (writecount == 1) {
            if (sem_wait(&readblock) < 0) {
                perror("cannot decrement the semaphore readblock");
                exit(1);
            }
        }
        if (sem_post(&mutex2) < 0) {
            perror("cannot increment semaphore mutex2");
            exit(1);
        }
        if (sem_wait(&writeblock) < 0) {
            perror("cannot decrement the semaphore write block");
            exit(1);
        }
        /* write resources */
        printf("Writer %d is writing\n", i);
        if (sem_post(&writeblock) < 0) {
            perror("cannot increment semaphore writeblock");
            exit(1);
        }
        if (sem_wait(&mutex2) < 0) {
            perror("cannot decrement the semaphore mutex2");
            exit(1);
        }
        writecount = writecount - 1;
        if (writecount == 0) {
            if (sem_post(&readblock) < 0) {
                perror("cannot increment semaphore readblock");
                exit(1);
            }
        }
        if (sem_post(&mutex2) < 0) {
            perror("cannot increment semaphore mutex2");
            exit(1);
        }
    }

    return NULL;
}
