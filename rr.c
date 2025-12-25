#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex;       // To protect readCount
sem_t wrt;         // To ensure mutual exclusion for writers
int readCount = 0; // Number of readers currently reading

void *reader(void *arg) {
    int f = *((int *)arg);

    sem_wait(&mutex);
    readCount++;
    if (readCount == 1)
        sem_wait(&wrt); // First reader locks the writer
    sem_post(&mutex);

    printf("Reader %d is reading\n", f);
    sleep(1); // Simulate reading
    printf("Reader %d has finished reading\n", f);

    sem_wait(&mutex);
    readCount--;
    if (readCount == 0)
        sem_post(&wrt); // Last reader unlocks the writer
    sem_post(&mutex);
    
    return NULL;
}

void *writer(void *arg) {
    int f = *((int *)arg);

    sem_wait(&wrt); // Only one writer can enter
    printf("Writer %d is writing\n", f);
    sleep(2); // Simulate writing
    printf("Writer %d has finished writing\n", f);
    sem_post(&wrt);

    return NULL;
}

int main() {
    pthread_t rtid[5], wtid[5];
    int i;
    int ids[5] = {1, 2, 3, 4, 5};

    sem_init(&mutex, 0, 1);
    sem_init(&wrt, 0, 1);

    for (i = 0; i < 5; i++) {
        pthread_create(&rtid[i], NULL, reader, &ids[i]);
        pthread_create(&wtid[i], NULL, writer, &ids[i]);
    }

    for (i = 0; i < 5; i++) {
        pthread_join(rtid[i], NULL);
        pthread_join(wtid[i], NULL);
    }

    sem_destroy(&mutex);
    sem_destroy(&wrt);

    return 0;
}
