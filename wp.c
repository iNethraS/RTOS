#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex, writeMutex, rw_mutex, readTry;
int readCount = 0;
int writeCount = 0;

void *reader(void *arg) {
    int id = *((int *)arg);

    sem_wait(&readTry);       // Wait here if writers are waiting
    sem_wait(&mutex);         // Lock readCount
    readCount++;
    if (readCount == 1)
        sem_wait(&rw_mutex);  // First reader locks resource
    sem_post(&mutex);         // Unlock readCount
    sem_post(&readTry);       // Allow others to try

    printf("Reader %d is reading\n", id);
    sleep(1);
    printf("Reader %d finished reading\n", id);

    sem_wait(&mutex);         // Lock readCount
    readCount--;
    if (readCount == 0)
        sem_post(&rw_mutex);  // Last reader unlocks
    sem_post(&mutex);         // Unlock readCount

    return NULL;
}

void *writer(void *arg) {
    int id = *((int *)arg);

    sem_wait(&writeMutex);     // Lock writeCount
    writeCount++;
    if (writeCount == 1)
        sem_wait(&readTry);    // Block readers if first writer
    sem_post(&writeMutex);     // Unlock writeCount

    sem_wait(&rw_mutex);       // Lock resource
    printf("Writer %d is writing\n", id);
    sleep(2);
    printf("Writer %d finished writing\n", id);
    sem_post(&rw_mutex);       // Unlock resource

    sem_wait(&writeMutex);     // Lock writeCount
    writeCount--;
    if (writeCount == 0)
        sem_post(&readTry);    // Allow readers again
    sem_post(&writeMutex);     // Unlock writeCount

    return NULL;
}

int main() {
    pthread_t rtid[5], wtid[5];
    int ids[5] = {1, 2, 3, 4, 5};

    sem_init(&mutex, 0, 1);
    sem_init(&writeMutex, 0, 1);
    sem_init(&rw_mutex, 0, 1);
    sem_init(&readTry, 0, 1);

    for (int i = 0; i < 5; i++) {
        pthread_create(&rtid[i], NULL, reader, &ids[i]);
        pthread_create(&wtid[i], NULL, writer, &ids[i]);
    }

    for (int i = 0; i < 5; i++) {
        pthread_join(rtid[i], NULL);
        pthread_join(wtid[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&writeMutex);
    sem_destroy(&rw_mutex);
    sem_destroy(&readTry);

    return 0;
}
