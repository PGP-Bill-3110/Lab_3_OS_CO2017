#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_PRODUCERS 2
#define NUM_CONSUMERS 2
#define NUM_ITEMS 10

int buffer[BUFFER_SIZE];
int count = 0; 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;

void write_buffer(int item, int producer_id) {
    pthread_mutex_lock(&mutex);


    while (count == BUFFER_SIZE) {
        pthread_cond_wait(&cond_var, &mutex);
    }

    buffer[count++] = item;
    printf("Producer %d wrote: %d (count=%d)\n", producer_id, item, count);

    pthread_cond_signal(&cond_var);

    pthread_mutex_unlock(&mutex);
}

int read_buffer(int consumer_id) {
    pthread_mutex_lock(&mutex);

    while (count == 0) {
        pthread_cond_wait(&cond_var, &mutex);
    }

    int item = buffer[--count];
    printf("Consumer %d read: %d (count=%d)\n", consumer_id, item, count);

    pthread_cond_signal(&cond_var);

    pthread_mutex_unlock(&mutex);

    return item;
}

void* producer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        write_buffer(i, id);
        usleep(100000); 
    }
    return NULL;
}

void* consumer(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < NUM_ITEMS; i++) {
        int item = read_buffer(id);
        usleep(150000); 
    }
    return NULL;
}

int main() {
    pthread_t producers[NUM_PRODUCERS], consumers[NUM_CONSUMERS];
    int ids[NUM_PRODUCERS > NUM_CONSUMERS ? NUM_PRODUCERS : NUM_CONSUMERS];

    for (int i = 0; i < NUM_PRODUCERS || i < NUM_CONSUMERS; i++)
        ids[i] = i + 1;
    for (int i = 0; i < NUM_PRODUCERS; i++)
        pthread_create(&producers[i], NULL, producer, &ids[i]);
    for (int i = 0; i < NUM_CONSUMERS; i++)
        pthread_create(&consumers[i], NULL, consumer, &ids[i]);
    for (int i = 0; i < NUM_PRODUCERS; i++)
        pthread_join(producers[i], NULL);
    for (int i = 0; i < NUM_CONSUMERS; i++)
        pthread_join(consumers[i], NULL);

    printf("All done.\n");
    return 0;
}
