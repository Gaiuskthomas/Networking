#include<stdio.h>
#include<pthread.h>
#include<semaphore.h>
#include<stdlib.h>
#include <unistd.h>

void * reader (void *);
void * writer (void *);
sem_t mutex;
sem_t writeBlock;
int readCount =0;
 int main(int argc, char * argv[])
{
int i, j, k;
int N_readers, N_writers;
int readers_num[100], writers_num[100];
pthread_t tid_readers[100], tid_writers[100];
printf("Enter the number of readers:");
scanf("%d", &N_readers);
printf("Enter the number of writers:");
scanf("%d", &N_writers);
for(k =0; k < N_readers; k++)
readers_num[k] = k;
for(k =0; k < N_writers; k++)
writers_num[k] = k;
if(sem_init(&mutex, 0, 1) < 0)
{
perror("Could not init semaphore mutex");
exit(1);
}
if(sem_init(&writeBlock, 0, 1) < 0)
{
perror("Could not init semaphore writeBlock");
exit(1);
}
for( i =0; i < N_readers; i++)
{
if(pthread_create(&tid_readers[i], NULL, reader, &readers_num[i] ))
{
perror("could not create reader thread");
exit(1);
}
}
for( j=0; j < N_writers; j++)
{
if(pthread_create(&tid_writers[j], NULL, writer, &writers_num[j]))
{
perror("could not create writer thread");
exit(1);
}
}
for (i =0; i < N_readers; i++)
{
pthread_join(tid_readers[i], NULL);
}
for ( j=0; j < N_writers; j++)
{
pthread_join(tid_writers[j], NULL);
}
sem_destroy(&mutex);
sem_destroy(&mutex);
}
void * reader (void* param)
{
int i = *((int *) param);
while(1)
{
sleep(1);
if(sem_wait(&mutex) < 0)
{
perror("cannot decrement the semaphore mutex");
exit(1);
}
readCount = readCount + 1;
if(readCount == 1)
{
if(sem_wait(&writeBlock) < 0)
{
perror("cannot decrement the semaphore writeBlock");
exit(1);
}
}
if( sem_post(&mutex) < 0)
{
perror("cannot increment semaphore mutex");
exit(1);
}
// READ RESOURCES
printf("READER %d is READING \n", i);
sleep(1);
if(sem_wait(&mutex) < 0)
{
perror("cannot decrement the semaphore mutex");
exit(1);
}
readCount = readCount - 1;
if(readCount == 0)
{
if( sem_post(&writeBlock) < 0)
{
perror("cannot increment semaphore mutex");
exit(1);
}
}
if( sem_post(&mutex) < 0)
{
perror("cannot increment semaphore mutex");
exit(1);
}
}
}
void * writer (void * param)
{
int i = *((int *) param);
while(1)
{
sleep(1);
if(sem_wait(&writeBlock) < 0)
{
perror("cannot decrement the semaphore writeBlock");
exit(1);
}
// WRITE RESOURCES
printf("WRITER %d IS WRITING \n", i);
if( sem_post(&writeBlock) < 0)
{
perror("cannot increment semaphore writeBlock");
exit(1);
}
}
}
